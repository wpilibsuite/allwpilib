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

#include "c_util.h"
#include "cscore_cpp.h"
#include "Handle.h"
#include "Log.h"
#include "Notifier.h"
#include "SourceImpl.h"

using namespace cs;


// The boundary used for the M-JPEG stream.
// It separates the multipart stream of pictures
#define BOUNDARY "boundarydonotcross"

// DHT data for MJPEG images that don't have it.
static const unsigned char dhtData[] = {
    0xff, 0xc4, 0x01, 0xa2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x01, 0x00, 0x03,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05,
    0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04,
    0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22,
    0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15,
    0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
    0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
    0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5,
    0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05,
    0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
    0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33,
    0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25,
    0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94,
    0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
    0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};

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

MJPEGServerImpl::MJPEGServerImpl(llvm::StringRef name,
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

static bool NeedsDHT(const char* data, std::size_t* size, std::size_t* locSOF) {
  // Search first 2048 bytes (or until SOS) for DHT tag
  for (llvm::StringRef::size_type i = 0; i < 2048 && i < *size; ++i) {
    if (static_cast<unsigned char>(data[i]) != 0xff) continue;  // not a tag
    unsigned char tag = static_cast<unsigned char>(data[i + 1]);
    if (tag == 0xda) break;         // SOS
    if (tag == 0xc4) return false;  // DHT
    if (tag == 0xc0) *locSOF = i;   // SOF
  }
  // Only add DHT if we also found SOF (insertion point)
  if (*locSOF != *size) {
    *size += sizeof(dhtData);
    return true;
  }
  return false;
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
    bool addDHT = false;
    std::size_t locSOF = size;
    switch (frame.GetPixelFormat()) {
      case VideoMode::kMJPEG:
        // Determine if we need to add DHT to it, and allocate enough space
        // for adding it if required.
        addDHT = NeedsDHT(data, &size, &locSOF);
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
      os << llvm::StringRef(reinterpret_cast<const char*>(dhtData),
                            sizeof(dhtData));
      os << llvm::StringRef(data + locSOF, frame.size() - locSOF);
    } else {
      os << llvm::StringRef(data, size);
    }
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

  enum { kCommand, kStream, kGetSettings } kind;
  llvm::StringRef parameters;
  size_t pos;

  // Determine request kind.  Most of these are for mjpgstreamer
  // compatibility, others are for Axis camera compatibility.
  if ((pos = buf.find("POST /stream")) != llvm::StringRef::npos) {
    kind = kStream;
    parameters = buf.substr(buf.find('?', pos + 12)).substr(1);
  } else if ((pos = buf.find("GET /?action=stream")) != llvm::StringRef::npos) {
    kind = kStream;
    parameters = buf.substr(buf.find('&', pos + 19)).substr(1);
  } else if ((pos = buf.find("GET /stream.mjpg")) != llvm::StringRef::npos) {
    kind = kStream;
    parameters = buf.substr(buf.find('?', pos + 16)).substr(1);
  } else if (buf.find("GET /input") != llvm::StringRef::npos &&
             buf.find(".json") != llvm::StringRef::npos) {
    kind = kGetSettings;
  } else if (buf.find("GET /output") != llvm::StringRef::npos &&
             buf.find(".json") != llvm::StringRef::npos) {
    kind = kGetSettings;
  } else if ((pos = buf.find("GET /?action=command")) !=
             llvm::StringRef::npos) {
    kind = kCommand;
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
  llvm::SmallString<128> str{listenAddress};
  auto sink = std::make_shared<MJPEGServerImpl>(
      name, listenAddress, port,
      std::unique_ptr<wpi::NetworkAcceptor>(
          new wpi::TCPAcceptor(port, str.c_str(), Logger::GetInstance())));
  auto handle = Sinks::GetInstance().Allocate(CS_SINK_MJPEG, sink);
  Notifier::GetInstance().NotifySink(name, handle, CS_SINK_CREATED);
  return handle;
}

std::string GetMJPEGServerListenAddress(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_MJPEG) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<MJPEGServerImpl&>(*data->sink).GetListenAddress();
}

int GetMJPEGServerPort(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_MJPEG) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<MJPEGServerImpl&>(*data->sink).GetPort();
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateMJPEGServer(const char* name, const char* listenAddress,
                             int port, CS_Status* status) {
  return cs::CreateMJPEGServer(name, listenAddress, port, status);
}

char* CS_GetMJPEGServerListenAddress(CS_Sink sink, CS_Status* status) {
  return ConvertToC(cs::GetMJPEGServerListenAddress(sink, status));
}

int CS_GetMJPEGServerPort(CS_Sink sink, CS_Status* status) {
  return cs::GetMJPEGServerPort(sink, status);
}

}  // extern "C"
