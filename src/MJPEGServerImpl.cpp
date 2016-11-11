/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MJPEGServerImpl.h"

#include <chrono>

#include "llvm/SmallString.h"
#include "llvm/StringExtras.h"
#include "support/raw_socket_istream.h"
#include "support/raw_socket_ostream.h"
#include "tcpsockets/TCPAcceptor.h"

#include "cscore_cpp.h"
#include "Handle.h"
#include "Log.h"
#include "SourceImpl.h"

using namespace cs;


// The boundary used for the M-JPEG stream.
// It separates the multipart stream of pictures
#define BOUNDARY "boundarydonotcross"

class MJPEGServerImpl::ConnThread : public wpi::SafeThread {
 public:
  void Main();

  bool ProcessCommand(llvm::raw_ostream& os, SourceImpl& source,
                      llvm::StringRef parameters, bool respond);
  void SendJSON(llvm::raw_ostream& os, SourceImpl& source, bool header);
  void SendStream(wpi::raw_socket_ostream& os);
  void ProcessRequest();

  std::unique_ptr<wpi::NetworkStream> m_stream;
  std::shared_ptr<SourceImpl> m_source;
  bool m_streaming = false;

 private:
  std::shared_ptr<SourceImpl> GetSource() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_source;
  }

  void StartStream() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_source->EnableSink();
    m_streaming = true;
  }

  void StopStream() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_source->DisableSink();
    m_streaming = false;
  }
};

// Standard header to send along with other header information like mimetype.
//
// The parameters should ensure the browser does not cache our answer.
// A browser should connect for each file and not serve files from its cache.
// Using cached pictures would lead to showing old/outdated pictures.
// Many browsers seem to ignore, or at least not always obey, those headers.
static void SendHeader(llvm::raw_ostream& os, int code,
                       llvm::StringRef codeText, llvm::StringRef contentType,
                       llvm::StringRef extra = llvm::StringRef{}) {
  os << "HTTP/1.0 " << code << ' ' << codeText << "\r\n";
  os << "Connection: close\r\n"
        "Server: CameraServer/1.0\r\n"
        "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, "
        "post-check=0, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n";
  os << "Content-Type: " << contentType << "\r\n";
  if (!extra.empty()) os << extra << "\r\n";
  os << "\r\n";  // header ends with a blank line
}

// Send error header and message
// @param code HTTP error code (e.g. 404)
// @param message Additional message text
static void SendError(llvm::raw_ostream& os, int code,
                      llvm::StringRef message) {
  llvm::StringRef codeText, extra, baseMessage;
  switch (code) {
    case 401:
      codeText = "Unauthorized";
      extra = "WWW-Authenticate: Basic realm=\"CameraServer\"";
      baseMessage = "401: Not Authenticated!";
      break;
    case 404:
      codeText = "Not Found";
      baseMessage = "404: Not Found!";
      break;
    case 500:
      codeText = "Internal Server Error";
      baseMessage = "500: Internal Server Error!";
      break;
    case 400:
      codeText = "Bad Request";
      baseMessage = "400: Not Found!";
      break;
    case 403:
      codeText = "Forbidden";
      baseMessage = "403: Forbidden!";
      break;
    default:
      code = 501;
      codeText = "Not Implemented";
      baseMessage = "501: Not Implemented!";
      break;
  }
  SendHeader(os, code, codeText, "text/plain", extra);
  os << baseMessage << "\r\n" << message;
}

// Read a line from an input stream (up to a maximum length).
// The returned buffer will contain the trailing \n (unless the maximum length
// was reached).
static bool ReadLine(wpi::raw_istream& is, llvm::SmallVectorImpl<char>& buffer,
                     int maxLen) {
  buffer.clear();
  for (int i = 0; i < maxLen; ++i) {
    char c;
    is.read(c);
    if (is.has_error()) return false;
    buffer.push_back(c);
    if (c == '\n') break;
  }
  return true;
}

// Unescape a %xx-encoded URI.  Returns false on error.
static bool UnescapeURI(llvm::StringRef str, llvm::SmallVectorImpl<char>& out) {
  for (auto i = str.begin(), end = str.end(); i != end; ++i) {
    // pass non-escaped characters to output
    if (*i != '%') {
      // decode + to space
      if (*i == '+')
        out.push_back(' ');
      else
        out.push_back(*i);
      continue;
    }

    // are there enough characters left?
    if (i + 2 < end) return false;

    // replace %xx with the corresponding character
    unsigned val1 = llvm::hexDigitValue(*++i);
    if (val1 == -1U) return false;
    unsigned val2 = llvm::hexDigitValue(*++i);
    if (val2 == -1U) return false;
    out.push_back((val1 << 4) | val2);
  }

  return true;
}

// Perform a command specified by HTTP GET parameters.
bool MJPEGServerImpl::ConnThread::ProcessCommand(llvm::raw_ostream& os,
                                                 SourceImpl& source,
                                                 llvm::StringRef parameters,
                                                 bool respond) {
  llvm::SmallString<256> responseBuf;
  llvm::raw_svector_ostream response{responseBuf};
  // command format: param1=value1&param2=value2...
  while (!parameters.empty()) {
    // split out next param and value
    llvm::StringRef rawParam, rawValue;
    std::tie(rawParam, parameters) = parameters.split('&');
    if (rawParam.empty()) continue;  // ignore "&&"
    std::tie(rawParam, rawValue) = rawParam.split('=');
    if (rawParam.empty() || rawValue.empty()) continue;  // ignore "param="
    DEBUG4("HTTP parameter \"" << rawParam << "\" value \"" << rawValue
                               << "\"");

    // unescape param
    llvm::SmallString<64> param;
    if (!UnescapeURI(rawParam, param)) {
      llvm::SmallString<128> error;
      llvm::raw_svector_ostream oss{error};
      oss << "could not unescape parameter \"" << rawParam << "\"";
      SendError(os, 500, error.str());
      DEBUG(error.str());
      return false;
    }

    // unescape value
    llvm::SmallString<64> value;
    if (!UnescapeURI(rawValue, value)) {
      llvm::SmallString<128> error;
      llvm::raw_svector_ostream oss{error};
      oss << "could not unescape value \"" << rawValue << "\"";
      SendError(os, 500, error.str());
      DEBUG(error.str());
      return false;
    }

    // handle resolution and FPS; these are handled via separate interfaces
    // rather than as properties
    if (param == "resolution") {
      llvm::StringRef widthStr, heightStr;
      std::tie(widthStr, heightStr) = value.str().split('x');
      int width, height;
      if (widthStr.getAsInteger(10, width)) {
        response << param << ": \"width is not integer\"\r\n";
        WARNING("HTTP parameter \"" << param << "\" width \"" << widthStr
                                    << "\" is not integer");
        continue;
      }
      if (heightStr.getAsInteger(10, height)) {
        response << param << ": \"height is not integer\"\r\n";
        WARNING("HTTP parameter \"" << param << "\" height \"" << heightStr
                                    << "\" is not integer");
        continue;
      }
      CS_Status status = 0;
      if (!source.SetResolution(width, height, &status)) {
        response << param << ": \"error\"\r\n";
        WARNING("Could not set resolution to " << width << "x" << height);
      } else {
        response << param << ": \"ok\"\r\n";
      }
      continue;
    }

    if (param == "fps") {
      int fps;
      if (value.str().getAsInteger(10, fps)) {
        response << param << ": \"invalid integer\"\r\n";
        WARNING("HTTP parameter \"" << param << "\" value \"" << value
                                    << "\" is not an integer");
        continue;
      }
      CS_Status status = 0;
      if (!source.SetFPS(fps, &status)) {
        response << param << ": \"error\"\r\n";
        WARNING("Could not set FPS to " << fps);
      } else {
        response << param << ": \"ok\"\r\n";
      }
      continue;
    }

    // try to assign parameter
    auto prop = source.GetPropertyIndex(param);
    if (!prop) {
      response << param << ": \"ignored\"\r\n";
      WARNING("ignoring HTTP parameter \"" << param << "\"");
      continue;
    }

    CS_Status status = 0;
    auto type = source.GetPropertyType(prop);
    switch (type) {
      case CS_PROP_BOOLEAN:
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM: {
        int val;
        if (value.str().getAsInteger(10, val)) {
          response << param << ": \"invalid integer\"\r\n";
          WARNING("HTTP parameter \"" << param << "\" value \"" << value
                                      << "\" is not an integer");
        } else {
          response << param << ": " << val << "\r\n";
          DEBUG4("HTTP parameter \"" << param << "\" value " << value);
          source.SetProperty(prop, val, &status);
        }
        break;
      }
      case CS_PROP_STRING: {
        response << param << ": \"ok\"\r\n";
        DEBUG4("HTTP parameter \"" << param << "\" value \"" << value << "\"");
        source.SetStringProperty(prop, value, &status);
        break;
      }
      default:
        break;
    }
  }

  // Send HTTP response
  if (respond) {
    SendHeader(os, 200, "OK", "text/plain");
    os << response.str() << "\r\n";
  }

  return true;
}

// Send a JSON file which is contains information about the source parameters.
void MJPEGServerImpl::ConnThread::SendJSON(llvm::raw_ostream& os,
                                           SourceImpl& source, bool header) {
  if (header) SendHeader(os, 200, "OK", "application/x-javascript");

  os << "{\n\"controls\": [\n";
  llvm::SmallVector<int, 32> properties_vec;
  bool first = true;
  CS_Status status = 0;
  for (auto prop : source.EnumerateProperties(properties_vec, &status)) {
    if (first)
      first = false;
    else
      os << ",\n";
    os << "{";
    llvm::SmallString<128> name_buf;
    auto name = source.GetPropertyName(prop, name_buf, &status);
    auto type = source.GetPropertyType(prop);
    os << "\n\"name\": \"" << name << '"';
    os << ",\n\"id\": \"" << prop << '"';
    os << ",\n\"type\": \"" << type << '"';
    os << ",\n\"min\": \"" << source.GetPropertyMin(prop, &status) << '"';
    os << ",\n\"max\": \"" << source.GetPropertyMax(prop, &status) << '"';
    os << ",\n\"step\": \"" << source.GetPropertyStep(prop, &status) << '"';
    os << ",\n\"default\": \"" << source.GetPropertyDefault(prop, &status)
       << '"';
    os << ",\n\"value\": \"";
    switch (type) {
      case CS_PROP_BOOLEAN:
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM:
        os << source.GetProperty(prop, &status);
        break;
      case CS_PROP_STRING: {
        llvm::SmallString<128> strval_buf;
        os << source.GetStringProperty(prop, strval_buf, &status);
        break;
      }
      default:
        break;
    }
    os << '"';
    // os << ",\n\"dest\": \"0\"";
    // os << ",\n\"flags\": \"" << param->flags << '"';
    // os << ",\n\"group\": \"" << param->group << '"';

    // append the menu object to the menu typecontrols
    if (source.GetPropertyType(prop) == CS_PROP_ENUM) {
      os << ",\n\"menu\": {";
      auto choices = source.GetEnumPropertyChoices(prop, &status);
      int j = 0;
      for (auto choice = choices.begin(), end = choices.end(); choice != end;
           ++j, ++choice) {
        if (j != 0) os << ", ";
        // replace any non-printable characters in name with spaces
        llvm::SmallString<128> ch_name;
        for (char ch : *choice) ch_name.push_back(isprint(ch) ? ch : ' ');
        os << '"' << j << "\": \"" << ch_name << '"';
      }
      os << "}\n";
    }
    os << '}';
  }
  os << "\n]\n}\n";
  os.flush();
}

MJPEGServerImpl::MJPEGServerImpl(llvm::StringRef name,
                                 llvm::StringRef description,
                                 std::unique_ptr<wpi::NetworkAcceptor> acceptor)
    : SinkImpl{name}, m_acceptor{std::move(acceptor)} {
  m_active = true;
  SetDescription(description);
  m_serverThread = std::thread(&MJPEGServerImpl::ServerThreadMain, this);
}

MJPEGServerImpl::~MJPEGServerImpl() { Stop(); }

void MJPEGServerImpl::Stop() {
  m_active = false;

  // wake up server thread by shutting down the socket
  m_acceptor->shutdown();

  // join server thread
  if (m_serverThread.joinable()) m_serverThread.join();

  // close streams
  for (auto& connThread : m_connThreads) {
    if (auto thr = connThread.GetThread()) {
      if (thr->m_stream) thr->m_stream->close();
    }
    connThread.Stop();
  }

  // wake up connection threads by forcing an empty frame to be sent
  if (auto source = GetSource())
    source->Wakeup();
}

// Send HTTP response and a stream of JPG-frames
void MJPEGServerImpl::ConnThread::SendStream(wpi::raw_socket_ostream& os) {
  os.SetUnbuffered();

  llvm::SmallString<256> header;
  llvm::raw_svector_ostream oss{header};

  SendHeader(oss, 200, "OK", "multipart/x-mixed-replace;boundary=" BOUNDARY,
             "Access-Control-Allow-Origin: *");
  os << oss.str();

  DEBUG("HTTP: Headers send, sending stream now");

  StartStream();
  while (m_active && !os.has_error()) {
    auto source = GetSource();
    if (!source) {
      // Source disconnected; sleep for one second
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    DEBUG4("HTTP: waiting for frame");
    Frame frame = source->GetNextFrame();  // blocks
    if (!m_active) break;
    if (!frame) {
      // Bad frame; sleep for 10 ms so we don't consume all processor time.
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    const char* data = frame.data();
    std::size_t size = frame.size();
    switch (frame.GetPixelFormat()) {
      case VideoMode::kMJPEG:
        break;
      case VideoMode::kYUYV:
      case VideoMode::kRGB565:
      default:
        // Bad frame; sleep for 10 ms so we don't consume all processor time.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
    }

    DEBUG4("HTTP: sending frame size=" << size);

    // print the individual mimetype and the length
    // sending the content-length fixes random stream disruption observed
    // with firefox
    double timestamp = frame.time() / 10000000.0;
    header.clear();
    oss << "\r\n--" BOUNDARY "\r\n"
        << "Content-Type: image/jpeg\r\n"
        << "Content-Length: " << size << "\r\n"
        << "X-Timestamp: " << timestamp << "\r\n"
        << "\r\n";
    os << oss.str();
    os << llvm::StringRef(data, size);
    // os.flush();
  }
  StopStream();
}

void MJPEGServerImpl::ConnThread::ProcessRequest() {
  wpi::raw_socket_istream is{*m_stream};
  wpi::raw_socket_ostream os{*m_stream, true};

  // Read the request string from the stream
  llvm::SmallString<128> buf;
  if (!ReadLine(is, buf, 4096)) {
    DEBUG("HTTP error getting request string");
    return;
  }

  enum { kCommand, kStream, kGetSettings } type;
  llvm::StringRef parameters;
  size_t pos;

  // Determine request type.  Most of these are for mjpgstreamer
  // compatibility.
  if ((pos = buf.find("POST /stream")) != llvm::StringRef::npos) {
    type = kStream;
    parameters = buf.substr(buf.find('?', pos + 12)).substr(1);
  } else if ((pos = buf.find("GET /?action=stream")) != llvm::StringRef::npos) {
    type = kStream;
    parameters = buf.substr(buf.find('&', pos + 19)).substr(1);
  } else if (buf.find("GET /input") != llvm::StringRef::npos &&
             buf.find(".json") != llvm::StringRef::npos) {
    type = kGetSettings;
  } else if (buf.find("GET /output") != llvm::StringRef::npos &&
             buf.find(".json") != llvm::StringRef::npos) {
    type = kGetSettings;
  } else if ((pos = buf.find("GET /?action=command")) !=
             llvm::StringRef::npos) {
    type = kCommand;
    parameters = buf.substr(buf.find('&', pos + 20)).substr(1);
  } else {
    DEBUG("HTTP request resource not found");
    SendError(os, 404, "Resource not found");
    return;
  }

  // Parameter can only be certain characters.  This also strips the EOL.
  pos = parameters.find_first_not_of(
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
      "-=&1234567890%./");
  parameters = parameters.substr(0, pos);
  DEBUG("command parameters: \"" << parameters << "\"");

  // Read the rest of the HTTP request.
  // The end of the request is marked by a single, empty line with "\r\n"
  llvm::SmallString<128> buf2;
  do {
    if (!ReadLine(is, buf2, 4096)) return;
  } while (!buf2.startswith("\r\n"));

  // Send response
  switch (type) {
    case kStream:
      if (auto source = GetSource()) {
        DEBUG("request for stream " << source->GetName());
        if (!ProcessCommand(os, *source, parameters, false)) return;
      }
      SendStream(os);
      break;
    case kCommand:
      if (auto source = GetSource()) {
        ProcessCommand(os, *source, parameters, true);
      } else {
        SendHeader(os, 200, "OK", "text/plain");
        os << "Ignored due to no connected source." << "\r\n";
        DEBUG("Ignored due to no connected source.");
      }
      break;
    case kGetSettings:
      DEBUG("request for JSON file");
      if (auto source = GetSource())
        SendJSON(os, *source, true);
      else
        SendError(os, 404, "Resource not found");
      break;
  }

  DEBUG("leaving HTTP client thread");
}

// worker thread for clients that connected to this server
void MJPEGServerImpl::ConnThread::Main() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    while (!m_stream) {
      m_cond.wait(lock);
      if (!m_active) return;
    }
    lock.unlock();
    ProcessRequest();
    lock.lock();
    m_stream = nullptr;
  }
}

// Main server thread
void MJPEGServerImpl::ServerThreadMain() {
  if (m_acceptor->start() != 0) {
    m_active = false;
    return;
  }

  DEBUG("waiting for clients to connect");
  while (m_active) {
    auto stream = m_acceptor->accept();
    if (!stream) {
      m_active = false;
      return;
    }
    if (!m_active) return;

    DEBUG("server: client connection from " << stream->getPeerIP());

    auto source = GetSource();

    std::lock_guard<std::mutex> lock(m_mutex);
    // Find unoccupied worker thread, or create one if necessary
    auto it = std::find_if(m_connThreads.begin(), m_connThreads.end(),
                           [](const wpi::SafeThreadOwner<ConnThread>& owner) {
                             auto thr = owner.GetThread();
                             return !thr || !thr->m_stream;
                           });
    if (it == m_connThreads.end()) {
      m_connThreads.emplace_back();
      it = std::prev(m_connThreads.end());
    }

    // Start it if not already started
    {
      auto thr = it->GetThread();
      if (!thr) it->Start();
    }

    // Hand off connection to it
    auto thr = it->GetThread();
    thr->m_stream = std::move(stream);
    thr->m_source = source;
    thr->m_cond.notify_one();
  }

  DEBUG("leaving server thread");
}

void MJPEGServerImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {
  std::lock_guard<std::mutex> lock(m_mutex);
  for (auto& connThread : m_connThreads) {
    if (auto thr = connThread.GetThread()) {
      if (thr->m_source != source) {
        bool streaming = thr->m_streaming;
        if (streaming) thr->m_source->DisableSink();
        thr->m_source = source;
        if (streaming) thr->m_source->EnableSink();
      }
    }
  }
}

namespace cs {

CS_Sink CreateMJPEGServer(llvm::StringRef name, llvm::StringRef listenAddress,
                          int port, CS_Status* status) {
  llvm::SmallString<128> descBuf;
  llvm::raw_svector_ostream desc{descBuf};
  desc << "HTTP Server on port " << port;
  llvm::SmallString<128> str{listenAddress};
  auto sink = std::make_shared<MJPEGServerImpl>(
      name, desc.str(),
      std::unique_ptr<wpi::NetworkAcceptor>(
          new wpi::TCPAcceptor(port, str.c_str(), Logger::GetInstance())));
  return Sinks::GetInstance().Allocate(SinkData::kHTTP, sink);
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateMJPEGServer(const char* name, const char* listenAddress,
                             int port, CS_Status* status) {
  return cs::CreateMJPEGServer(name, listenAddress, port, status);
}

}  // extern "C"
