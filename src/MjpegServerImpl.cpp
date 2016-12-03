/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MjpegServerImpl.h"

#include <chrono>

#include "llvm/SmallString.h"
#include "support/raw_socket_istream.h"
#include "support/raw_socket_ostream.h"
#include "tcpsockets/TCPAcceptor.h"

#include "c_util.h"
#include "cscore_cpp.h"
#include "Handle.h"
#include "HttpUtil.h"
#include "JpegUtil.h"
#include "Log.h"
#include "Notifier.h"
#include "SourceImpl.h"

using namespace cs;


// The boundary used for the M-JPEG stream.
// It separates the multipart stream of pictures
#define BOUNDARY "boundarydonotcross"

// A bare-bones HTML webpage for user friendliness.
static const char* rootPage =
    "<html><head><title>CameraServer</title><body>"
    "<img src=\"/stream.mjpg\" /><p />"
    "<a href=\"/settings.json\">Settings JSON</a>"
    "</body></html>";

class MjpegServerImpl::ConnThread : public wpi::SafeThread {
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

// Perform a command specified by HTTP GET parameters.
bool MjpegServerImpl::ConnThread::ProcessCommand(llvm::raw_ostream& os,
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
    bool error = false;
    llvm::SmallString<64> paramBuf;
    llvm::StringRef param = UnescapeURI(rawParam, paramBuf, &error);
    if (error) {
      llvm::SmallString<128> error;
      llvm::raw_svector_ostream oss{error};
      oss << "could not unescape parameter \"" << rawParam << "\"";
      SendError(os, 500, error.str());
      DEBUG(error.str());
      return false;
    }

    // unescape value
    llvm::SmallString<64> valueBuf;
    llvm::StringRef value = UnescapeURI(rawValue, valueBuf, &error);
    if (error) {
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
      std::tie(widthStr, heightStr) = value.split('x');
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
      if (value.getAsInteger(10, fps)) {
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

    // ignore name parameter
    if (param == "name") continue;

    // try to assign parameter
    auto prop = source.GetPropertyIndex(param);
    if (!prop) {
      if (param == "compression") continue;  // silently ignore
      response << param << ": \"ignored\"\r\n";
      WARNING("ignoring HTTP parameter \"" << param << "\"");
      continue;
    }

    CS_Status status = 0;
    auto kind = source.GetPropertyKind(prop);
    switch (kind) {
      case CS_PROP_BOOLEAN:
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM: {
        int val;
        if (value.getAsInteger(10, val)) {
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
void MjpegServerImpl::ConnThread::SendJSON(llvm::raw_ostream& os,
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
    auto kind = source.GetPropertyKind(prop);
    os << "\n\"name\": \"" << name << '"';
    os << ",\n\"id\": \"" << prop << '"';
    os << ",\n\"type\": \"" << kind << '"';
    os << ",\n\"min\": \"" << source.GetPropertyMin(prop, &status) << '"';
    os << ",\n\"max\": \"" << source.GetPropertyMax(prop, &status) << '"';
    os << ",\n\"step\": \"" << source.GetPropertyStep(prop, &status) << '"';
    os << ",\n\"default\": \"" << source.GetPropertyDefault(prop, &status)
       << '"';
    os << ",\n\"value\": \"";
    switch (kind) {
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
    if (source.GetPropertyKind(prop) == CS_PROP_ENUM) {
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

MjpegServerImpl::MjpegServerImpl(llvm::StringRef name,
                                 llvm::StringRef listenAddress, int port,
                                 std::unique_ptr<wpi::NetworkAcceptor> acceptor)
    : SinkImpl{name},
      m_listenAddress(listenAddress),
      m_port(port),
      m_acceptor{std::move(acceptor)} {
  m_active = true;

  llvm::SmallString<128> descBuf;
  llvm::raw_svector_ostream desc{descBuf};
  desc << "HTTP Server on port " << port;
  SetDescription(desc.str());

  m_serverThread = std::thread(&MjpegServerImpl::ServerThreadMain, this);
}

MjpegServerImpl::~MjpegServerImpl() { Stop(); }

void MjpegServerImpl::Stop() {
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
void MjpegServerImpl::ConnThread::SendStream(wpi::raw_socket_ostream& os) {
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
    bool addDHT = false;
    std::size_t locSOF = size;
    switch (frame.GetPixelFormat()) {
      case VideoMode::kMJPEG:
        // Determine if we need to add DHT to it, and allocate enough space
        // for adding it if required.
        addDHT = JpegNeedsDHT(data, &size, &locSOF);
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
    if (addDHT) {
      // Insert DHT data immediately before SOF
      os << llvm::StringRef(data, locSOF);
      os << JpegGetDHT();
      os << llvm::StringRef(data + locSOF, frame.size() - locSOF);
    } else {
      os << llvm::StringRef(data, size);
    }
    // os.flush();
  }
  StopStream();
}

void MjpegServerImpl::ConnThread::ProcessRequest() {
  wpi::raw_socket_istream is{*m_stream};
  wpi::raw_socket_ostream os{*m_stream, true};

  // Read the request string from the stream
  bool error = false;
  llvm::SmallString<128> reqBuf;
  llvm::StringRef req = ReadLine(is, reqBuf, 4096, &error);
  if (error) {
    DEBUG("HTTP error getting request string");
    return;
  }

  enum { kCommand, kStream, kGetSettings, kRootPage } kind;
  llvm::StringRef parameters;
  size_t pos;

  DEBUG("HTTP request: '" << req << "'\n");

  // Determine request kind.  Most of these are for mjpgstreamer
  // compatibility, others are for Axis camera compatibility.
  if ((pos = req.find("POST /stream")) != llvm::StringRef::npos) {
    kind = kStream;
    parameters = req.substr(req.find('?', pos + 12)).substr(1);
  } else if ((pos = req.find("GET /?action=stream")) != llvm::StringRef::npos) {
    kind = kStream;
    parameters = req.substr(req.find('&', pos + 19)).substr(1);
  } else if ((pos = req.find("GET /stream.mjpg")) != llvm::StringRef::npos) {
    kind = kStream;
    parameters = req.substr(req.find('?', pos + 16)).substr(1);
  } else if (req.find("GET /settings") != llvm::StringRef::npos &&
             req.find(".json") != llvm::StringRef::npos) {
    kind = kGetSettings;
  } else if (req.find("GET /input") != llvm::StringRef::npos &&
             req.find(".json") != llvm::StringRef::npos) {
    kind = kGetSettings;
  } else if (req.find("GET /output") != llvm::StringRef::npos &&
             req.find(".json") != llvm::StringRef::npos) {
    kind = kGetSettings;
  } else if ((pos = req.find("GET /?action=command")) !=
             llvm::StringRef::npos) {
    kind = kCommand;
    parameters = req.substr(req.find('&', pos + 20)).substr(1);
  } else if (req.find("GET / ") != llvm::StringRef::npos || req == "GET /\n") {
    kind = kRootPage;
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
  // The end of the request is marked by a single, empty line
  llvm::SmallString<128> lineBuf;
  for (;;) {
    if (ReadLine(is, lineBuf, 4096, &error).startswith("\n")) break;
    if (error) return;
  }

  // Send response
  switch (kind) {
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
    case kRootPage:
      DEBUG("request for root page");
      SendHeader(os, 200, "OK", "text/html");
      os << rootPage << "\r\n";
      break;
  }

  DEBUG("leaving HTTP client thread");
}

// worker thread for clients that connected to this server
void MjpegServerImpl::ConnThread::Main() {
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
void MjpegServerImpl::ServerThreadMain() {
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

void MjpegServerImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {
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

CS_Sink CreateMjpegServer(llvm::StringRef name, llvm::StringRef listenAddress,
                          int port, CS_Status* status) {
  llvm::SmallString<128> str{listenAddress};
  auto sink = std::make_shared<MjpegServerImpl>(
      name, listenAddress, port,
      std::unique_ptr<wpi::NetworkAcceptor>(
          new wpi::TCPAcceptor(port, str.c_str(), Logger::GetInstance())));
  auto handle = Sinks::GetInstance().Allocate(CS_SINK_MJPEG, sink);
  Notifier::GetInstance().NotifySink(name, handle, CS_SINK_CREATED);
  return handle;
}

std::string GetMjpegServerListenAddress(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_MJPEG) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<MjpegServerImpl&>(*data->sink).GetListenAddress();
}

int GetMjpegServerPort(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_MJPEG) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<MjpegServerImpl&>(*data->sink).GetPort();
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateMjpegServer(const char* name, const char* listenAddress,
                             int port, CS_Status* status) {
  return cs::CreateMjpegServer(name, listenAddress, port, status);
}

char* CS_GetMjpegServerListenAddress(CS_Sink sink, CS_Status* status) {
  return ConvertToC(cs::GetMjpegServerListenAddress(sink, status));
}

int CS_GetMjpegServerPort(CS_Sink sink, CS_Status* status) {
  return cs::GetMjpegServerPort(sink, status);
}

}  // extern "C"
