/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MjpegServerImpl.h"

#include <chrono>

#include <llvm/SmallString.h>
#include <support/HttpUtil.h>
#include <support/raw_socket_istream.h>
#include <support/raw_socket_ostream.h>
#include <tcpsockets/TCPAcceptor.h>

#include "Handle.h"
#include "JpegUtil.h"
#include "Log.h"
#include "Notifier.h"
#include "SourceImpl.h"
#include "c_util.h"
#include "cscore_cpp.h"

using namespace cs;

// The boundary used for the M-JPEG stream.
// It separates the multipart stream of pictures
#define BOUNDARY "boundarydonotcross"

// A bare-bones HTML webpage for user friendliness.
static const char* emptyRootPage =
    "</head><body>"
    "<img src=\"/stream.mjpg\" /><p />"
    "<a href=\"/settings.json\">Settings JSON</a>"
    "</body></html>";

// An HTML page to be sent when a source exists
static const char* startRootPage =
    "<script>\n"
    "function httpGetAsync(name, val)\n"
    "{\n"
    "    var host = location.protocol + '//' + location.host + "
    "'/?action=command&' + name + '=' + val;\n"
    "    var xmlHttp = new XMLHttpRequest();\n"
    "    xmlHttp.open(\"GET\", host, true);\n"
    "    xmlHttp.send(null);\n"
    "}\n"
    "function updateInt(prop, name, val) {\n"
    "    document.querySelector(prop).value = val;\n"
    "    httpGetAsync(name, val);\n"
    "}\n"
    "function update(name, val) {\n"
    "    httpGetAsync(name, val);\n"
    "}\n"
    "</script>\n"
    "<style>\n"
    "table, th, td {\n"
    "    border: 1px solid black;\n"
    "    border-collapse: collapse;\n"
    "}\n"
    ".settings { float: left; }\n"
    ".stream { display: inline-block; margin-left: 10px; }\n"
    "</style>\n"
    "</head><body>\n"
    "<div class=\"stream\">\n"
    "<img src=\"/stream.mjpg\" /><p />\n"
    "<a href=\"/settings.json\">Settings JSON</a>\n"
    "</div>\n"
    "<div class=\"settings\">\n";
static const char* endRootPage = "</div></body></html>";

class MjpegServerImpl::ConnThread : public wpi::SafeThread {
 public:
  explicit ConnThread(llvm::StringRef name) : m_name(name) {}

  void Main();

  bool ProcessCommand(llvm::raw_ostream& os, SourceImpl& source,
                      llvm::StringRef parameters, bool respond);
  void SendJSON(llvm::raw_ostream& os, SourceImpl& source, bool header);
  void SendHTMLHeadTitle(llvm::raw_ostream& os) const;
  void SendHTML(llvm::raw_ostream& os, SourceImpl& source, bool header);
  void SendStream(wpi::raw_socket_ostream& os);
  void ProcessRequest();

  std::unique_ptr<wpi::NetworkStream> m_stream;
  std::shared_ptr<SourceImpl> m_source;
  bool m_streaming = false;
  bool m_noStreaming = false;

 private:
  std::string m_name;

  llvm::StringRef GetName() { return m_name; }

  std::shared_ptr<SourceImpl> GetSource() {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    return m_source;
  }

  void StartStream() {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_source->EnableSink();
    m_streaming = true;
  }

  void StopStream() {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_source->DisableSink();
    m_streaming = false;
  }

  int m_width{0};
  int m_height{0};
  int m_compression{80};
  int m_fps{0};
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
  os << "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n";
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
    case 503:
      codeText = "Service Unavailable";
      baseMessage = "503: Service Unavailable";
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
    SDEBUG4("HTTP parameter \"" << rawParam << "\" value \"" << rawValue
                                << "\"");

    // unescape param
    bool error = false;
    llvm::SmallString<64> paramBuf;
    llvm::StringRef param = wpi::UnescapeURI(rawParam, paramBuf, &error);
    if (error) {
      llvm::SmallString<128> error;
      llvm::raw_svector_ostream oss{error};
      oss << "could not unescape parameter \"" << rawParam << "\"";
      SendError(os, 500, error.str());
      SDEBUG(error.str());
      return false;
    }

    // unescape value
    llvm::SmallString<64> valueBuf;
    llvm::StringRef value = wpi::UnescapeURI(rawValue, valueBuf, &error);
    if (error) {
      llvm::SmallString<128> error;
      llvm::raw_svector_ostream oss{error};
      oss << "could not unescape value \"" << rawValue << "\"";
      SendError(os, 500, error.str());
      SDEBUG(error.str());
      return false;
    }

    // Handle resolution, compression, and FPS.  These are handled locally
    // rather than passed to the source.
    if (param == "resolution") {
      llvm::StringRef widthStr, heightStr;
      std::tie(widthStr, heightStr) = value.split('x');
      int width, height;
      if (widthStr.getAsInteger(10, width)) {
        response << param << ": \"width is not an integer\"\r\n";
        SWARNING("HTTP parameter \"" << param << "\" width \"" << widthStr
                                     << "\" is not an integer");
        continue;
      }
      if (heightStr.getAsInteger(10, height)) {
        response << param << ": \"height is not an integer\"\r\n";
        SWARNING("HTTP parameter \"" << param << "\" height \"" << heightStr
                                     << "\" is not an integer");
        continue;
      }
      m_width = width;
      m_height = height;
      response << param << ": \"ok\"\r\n";
      continue;
    }

    if (param == "fps") {
      int fps;
      if (value.getAsInteger(10, fps)) {
        response << param << ": \"invalid integer\"\r\n";
        SWARNING("HTTP parameter \"" << param << "\" value \"" << value
                                     << "\" is not an integer");
        continue;
      } else {
        m_fps = fps;
        response << param << ": \"ok\"\r\n";
      }
      continue;
    }

    if (param == "compression") {
      int compression;
      if (value.getAsInteger(10, compression)) {
        response << param << ": \"invalid integer\"\r\n";
        SWARNING("HTTP parameter \"" << param << "\" value \"" << value
                                     << "\" is not an integer");
        continue;
      } else {
        m_compression = compression;
        response << param << ": \"ok\"\r\n";
      }
      continue;
    }

    // ignore name parameter
    if (param == "name") continue;

    // try to assign parameter
    auto prop = source.GetPropertyIndex(param);
    if (!prop) {
      response << param << ": \"ignored\"\r\n";
      SWARNING("ignoring HTTP parameter \"" << param << "\"");
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
          SWARNING("HTTP parameter \"" << param << "\" value \"" << value
                                       << "\" is not an integer");
        } else {
          response << param << ": " << val << "\r\n";
          SDEBUG4("HTTP parameter \"" << param << "\" value " << value);
          source.SetProperty(prop, val, &status);
        }
        break;
      }
      case CS_PROP_STRING: {
        response << param << ": \"ok\"\r\n";
        SDEBUG4("HTTP parameter \"" << param << "\" value \"" << value << "\"");
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

void MjpegServerImpl::ConnThread::SendHTMLHeadTitle(
    llvm::raw_ostream& os) const {
  os << "<html><head><title>" << m_name << " CameraServer</title>";
}

// Send the root html file with controls for all the settable properties.
void MjpegServerImpl::ConnThread::SendHTML(llvm::raw_ostream& os,
                                           SourceImpl& source, bool header) {
  if (header) SendHeader(os, 200, "OK", "application/x-javascript");

  SendHTMLHeadTitle(os);
  os << startRootPage;
  llvm::SmallVector<int, 32> properties_vec;
  CS_Status status = 0;
  for (auto prop : source.EnumerateProperties(properties_vec, &status)) {
    llvm::SmallString<128> name_buf;
    auto name = source.GetPropertyName(prop, name_buf, &status);
    if (name.startswith("raw_")) continue;
    auto kind = source.GetPropertyKind(prop);
    os << "<p />"
       << "<label for=\"" << name << "\">" << name << "</label>\n";
    switch (kind) {
      case CS_PROP_BOOLEAN:
        os << "<input id=\"" << name
           << "\" type=\"checkbox\" onclick=\"update('" << name
           << "', this.checked ? 1 : 0)\" ";
        if (source.GetProperty(prop, &status) != 0)
          os << "checked />\n";
        else
          os << " />\n";
        break;
      case CS_PROP_INTEGER: {
        auto valI = source.GetProperty(prop, &status);
        auto min = source.GetPropertyMin(prop, &status);
        auto max = source.GetPropertyMax(prop, &status);
        auto step = source.GetPropertyStep(prop, &status);
        os << "<input type=\"range\" min=\"" << min << "\" max=\"" << max
           << "\" value=\"" << valI << "\" id=\"" << name << "\" step=\""
           << step << "\" oninput=\"updateInt('#" << name << "op', '" << name
           << "', value)\" />\n";
        os << "<output for=\"" << name << "\" id=\"" << name << "op\">" << valI
           << "</output>\n";
        break;
      }
      case CS_PROP_ENUM: {
        auto valE = source.GetProperty(prop, &status);
        auto choices = source.GetEnumPropertyChoices(prop, &status);
        int j = 0;
        for (auto choice = choices.begin(), end = choices.end(); choice != end;
             ++j, ++choice) {
          if (choice->empty()) continue;  // skip empty choices
          // replace any non-printable characters in name with spaces
          llvm::SmallString<128> ch_name;
          for (char ch : *choice)
            ch_name.push_back(std::isprint(ch) ? ch : ' ');
          os << "<input id=\"" << name << j << "\" type=\"radio\" name=\""
             << name << "\" value=\"" << ch_name << "\" onclick=\"update('"
             << name << "', " << j << ")\"";
          if (j == valE) {
            os << " checked";
          }
          os << " /><label for=\"" << name << j << "\">" << ch_name
             << "</label>\n";
        }
        break;
      }
      case CS_PROP_STRING: {
        llvm::SmallString<128> strval_buf;
        os << "<input type=\"text\" id=\"" << name << "box\" name=\"" << name
           << "\" value=\""
           << source.GetStringProperty(prop, strval_buf, &status) << "\" />\n";
        os << "<input type=\"button\" value =\"Submit\" onclick=\"update('"
           << name << "', " << name << "box.value)\" />\n";
        break;
      }
      default:
        break;
    }
  }

  os << "<p>Supported Video Modes:</p>\n";
  os << "<table cols=\"4\" style=\"border: 1px solid black\">\n";
  os << "<tr><th>Pixel Format</th>"
     << "<th>Width</th>"
     << "<th>Height</th>"
     << "<th>FPS</th></tr>";
  for (auto mode : source.EnumerateVideoModes(&status)) {
    os << "<tr><td>";
    switch (mode.pixelFormat) {
      case VideoMode::kMJPEG:
        os << "MJPEG";
        break;
      case VideoMode::kYUYV:
        os << "YUYV";
        break;
      case VideoMode::kRGB565:
        os << "RGB565";
        break;
      case VideoMode::kBGR:
        os << "BGR";
        break;
      case VideoMode::kGray:
        os << "gray";
        break;
      default:
        os << "unknown";
        break;
    }
    os << "</td><td>" << mode.width;
    os << "</td><td>" << mode.height;
    os << "</td><td>" << mode.fps;
    os << "</td></tr>";
  }
  os << "</table>\n";
  os << endRootPage << "\r\n";
  os.flush();
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
    os << '{';
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
        for (char ch : *choice) ch_name.push_back(std::isprint(ch) ? ch : ' ');
        os << '"' << j << "\": \"" << ch_name << '"';
      }
      os << "}\n";
    }
    os << '}';
  }
  os << "\n],\n\"modes\": [\n";
  first = true;
  for (auto mode : source.EnumerateVideoModes(&status)) {
    if (first)
      first = false;
    else
      os << ",\n";
    os << '{';
    os << "\n\"pixelFormat\": \"";
    switch (mode.pixelFormat) {
      case VideoMode::kMJPEG:
        os << "MJPEG";
        break;
      case VideoMode::kYUYV:
        os << "YUYV";
        break;
      case VideoMode::kRGB565:
        os << "RGB565";
        break;
      case VideoMode::kBGR:
        os << "BGR";
        break;
      case VideoMode::kGray:
        os << "gray";
        break;
      default:
        os << "unknown";
        break;
    }
    os << "\",\n\"width\": \"" << mode.width << '"';
    os << ",\n\"height\": \"" << mode.height << '"';
    os << ",\n\"fps\": \"" << mode.fps << '"';
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
  if (auto source = GetSource()) source->Wakeup();
}

// Send HTTP response and a stream of JPG-frames
void MjpegServerImpl::ConnThread::SendStream(wpi::raw_socket_ostream& os) {
  if (m_noStreaming) {
    SERROR("Too many simultaneous client streams");
    SendError(os, 503, "Too many simultaneous streams");
    return;
  }

  os.SetUnbuffered();

  llvm::SmallString<256> header;
  llvm::raw_svector_ostream oss{header};

  SendHeader(oss, 200, "OK", "multipart/x-mixed-replace;boundary=" BOUNDARY);
  os << oss.str();

  SDEBUG("Headers send, sending stream now");

  StartStream();
  while (m_active && !os.has_error()) {
    auto source = GetSource();
    if (!source) {
      // Source disconnected; sleep so we don't consume all processor time.
      os << "\r\n";  // Keep connection alive
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }
    SDEBUG4("waiting for frame");
    Frame frame = source->GetNextFrame(0.225);  // blocks
    if (!m_active) break;
    if (!frame) {
      // Bad frame; sleep for 20 ms so we don't consume all processor time.
      os << "\r\n";  // Keep connection alive
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }

    int width = m_width != 0 ? m_width : frame.GetOriginalWidth();
    int height = m_height != 0 ? m_height : frame.GetOriginalHeight();
    Image* image =
        frame.GetImage(width, height, VideoMode::kMJPEG, m_compression);
    if (!image) {
      // Shouldn't happen, but just in case...
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }

    const char* data = image->data();
    size_t size = image->size();
    bool addDHT = false;
    size_t locSOF = size;
    switch (image->pixelFormat) {
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

    SDEBUG4("sending frame size=" << size << " addDHT=" << addDHT);

    // print the individual mimetype and the length
    // sending the content-length fixes random stream disruption observed
    // with firefox
    double timestamp = frame.GetTime() / 1000000.0;
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
      os << llvm::StringRef(data + locSOF, image->size() - locSOF);
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

  // Reset per-request settings
  m_width = 0;
  m_height = 0;
  m_compression = 80;
  m_fps = 0;

  // Read the request string from the stream
  llvm::SmallString<128> reqBuf;
  llvm::StringRef req = is.getline(reqBuf, 4096);
  if (is.has_error()) {
    SDEBUG("error getting request string");
    return;
  }

  enum { kCommand, kStream, kGetSettings, kRootPage } kind;
  llvm::StringRef parameters;
  size_t pos;

  SDEBUG("HTTP request: '" << req << "'\n");

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
    SDEBUG("HTTP request resource not found");
    SendError(os, 404, "Resource not found");
    return;
  }

  // Parameter can only be certain characters.  This also strips the EOL.
  pos = parameters.find_first_not_of(
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
      "-=&1234567890%./");
  parameters = parameters.substr(0, pos);
  SDEBUG("command parameters: \"" << parameters << "\"");

  // Read the rest of the HTTP request.
  // The end of the request is marked by a single, empty line
  llvm::SmallString<128> lineBuf;
  for (;;) {
    if (is.getline(lineBuf, 4096).startswith("\n")) break;
    if (is.has_error()) return;
  }

  // Send response
  switch (kind) {
    case kStream:
      if (auto source = GetSource()) {
        SDEBUG("request for stream " << source->GetName());
        if (!ProcessCommand(os, *source, parameters, false)) return;
      }
      SendStream(os);
      break;
    case kCommand:
      if (auto source = GetSource()) {
        ProcessCommand(os, *source, parameters, true);
      } else {
        SendHeader(os, 200, "OK", "text/plain");
        os << "Ignored due to no connected source."
           << "\r\n";
        SDEBUG("Ignored due to no connected source.");
      }
      break;
    case kGetSettings:
      SDEBUG("request for JSON file");
      if (auto source = GetSource())
        SendJSON(os, *source, true);
      else
        SendError(os, 404, "Resource not found");
      break;
    case kRootPage:
      SDEBUG("request for root page");
      SendHeader(os, 200, "OK", "text/html");
      if (auto source = GetSource()) {
        SendHTML(os, *source, false);
      } else {
        SendHTMLHeadTitle(os);
        os << emptyRootPage << "\r\n";
      }
      break;
  }

  SDEBUG("leaving HTTP client thread");
}

// worker thread for clients that connected to this server
void MjpegServerImpl::ConnThread::Main() {
  std::unique_lock<wpi::mutex> lock(m_mutex);
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

  SDEBUG("waiting for clients to connect");
  while (m_active) {
    auto stream = m_acceptor->accept();
    if (!stream) {
      m_active = false;
      return;
    }
    if (!m_active) return;

    SDEBUG("client connection from " << stream->getPeerIP());

    auto source = GetSource();

    std::lock_guard<wpi::mutex> lock(m_mutex);
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
      if (!thr) it->Start(new ConnThread{GetName()});
    }

    auto nstreams =
        std::count_if(m_connThreads.begin(), m_connThreads.end(),
                      [](const wpi::SafeThreadOwner<ConnThread>& owner) {
                        auto thr = owner.GetThread();
                        return thr && thr->m_streaming;
                      });

    // Hand off connection to it
    auto thr = it->GetThread();
    thr->m_stream = std::move(stream);
    thr->m_source = source;
    thr->m_noStreaming = nstreams >= 10;
    thr->m_cond.notify_one();
  }

  SDEBUG("leaving server thread");
}

void MjpegServerImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  for (auto& connThread : m_connThreads) {
    if (auto thr = connThread.GetThread()) {
      if (thr->m_source != source) {
        bool streaming = thr->m_streaming;
        if (thr->m_source && streaming) thr->m_source->DisableSink();
        thr->m_source = source;
        if (source && streaming) thr->m_source->EnableSink();
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
