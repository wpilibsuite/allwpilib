// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MjpegServerImpl.h"

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/fmt/raw_ostream.h>
#include <wpi/print.h>
#include <wpinet/HttpUtil.h>
#include <wpinet/TCPAcceptor.h>
#include <wpinet/raw_socket_istream.h>
#include <wpinet/raw_socket_ostream.h>

#include "Instance.h"
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
    "<a href=\"/settings.json\">Settings JSON</a> |\n"
    "<a href=\"/config.json\">Source Config JSON</a>\n"
    "</div>\n"
    "<div class=\"settings\">\n";
static const char* endRootPage = "</div></body></html>";

class MjpegServerImpl::ConnThread : public wpi::SafeThread {
 public:
  explicit ConnThread(std::string_view name, wpi::Logger& logger)
      : m_name(name), m_logger(logger) {}

  void Main() override;

  bool ProcessCommand(wpi::raw_ostream& os, SourceImpl& source,
                      std::string_view parameters, bool respond);
  void SendJSON(wpi::raw_ostream& os, SourceImpl& source, bool header);
  void SendHTMLHeadTitle(wpi::raw_ostream& os) const;
  void SendHTML(wpi::raw_ostream& os, SourceImpl& source, bool header);
  void SendStream(wpi::raw_socket_ostream& os);
  void ProcessRequest();

  std::unique_ptr<wpi::NetworkStream> m_stream;
  std::shared_ptr<SourceImpl> m_source;
  bool m_streaming = false;
  bool m_noStreaming = false;
  int m_width = 0;
  int m_height = 0;
  int m_compression = -1;
  int m_defaultCompression = 80;
  int m_fps = 0;

 private:
  std::string m_name;
  wpi::Logger& m_logger;

  std::string_view GetName() { return m_name; }

  std::shared_ptr<SourceImpl> GetSource() {
    std::scoped_lock lock(m_mutex);
    return m_source;
  }

  void StartStream() {
    std::scoped_lock lock(m_mutex);
    if (m_source) {
      m_source->EnableSink();
    }
    m_streaming = true;
  }

  void StopStream() {
    std::scoped_lock lock(m_mutex);
    if (m_source) {
      m_source->DisableSink();
    }
    m_streaming = false;
  }
};

// Standard header to send along with other header information like mimetype.
//
// The parameters should ensure the browser does not cache our answer.
// A browser should connect for each file and not serve files from its cache.
// Using cached pictures would lead to showing old/outdated pictures.
// Many browsers seem to ignore, or at least not always obey, those headers.
static void SendHeader(wpi::raw_ostream& os, int code,
                       std::string_view codeText, std::string_view contentType,
                       std::string_view extra = {}) {
  wpi::print(os, "HTTP/1.0 {} {}\r\n", code, codeText);
  os << "Connection: close\r\n"
        "Server: CameraServer/1.0\r\n"
        "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, "
        "post-check=0, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n";
  os << "Content-Type: " << contentType << "\r\n";
  os << "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n";
  if (!extra.empty()) {
    os << extra << "\r\n";
  }
  os << "\r\n";  // header ends with a blank line
}

// Send error header and message
// @param code HTTP error code (e.g. 404)
// @param message Additional message text
static void SendError(wpi::raw_ostream& os, int code,
                      std::string_view message) {
  std::string_view codeText, extra, baseMessage;
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
bool MjpegServerImpl::ConnThread::ProcessCommand(wpi::raw_ostream& os,
                                                 SourceImpl& source,
                                                 std::string_view parameters,
                                                 bool respond) {
  wpi::SmallString<256> responseBuf;
  wpi::raw_svector_ostream response{responseBuf};
  // command format: param1=value1&param2=value2...
  while (!parameters.empty()) {
    // split out next param and value
    std::string_view rawParam, rawValue;
    std::tie(rawParam, parameters) = wpi::split(parameters, '&');
    if (rawParam.empty()) {
      continue;  // ignore "&&"
    }
    std::tie(rawParam, rawValue) = wpi::split(rawParam, '=');
    if (rawParam.empty() || rawValue.empty()) {
      continue;  // ignore "param="
    }
    SDEBUG4("HTTP parameter \"{}\" value \"{}\"", rawParam, rawValue);

    // unescape param
    bool error = false;
    wpi::SmallString<64> paramBuf;
    std::string_view param = wpi::UnescapeURI(rawParam, paramBuf, &error);
    if (error) {
      auto estr = fmt::format("could not unescape parameter \"{}\"", rawParam);
      SendError(os, 500, estr);
      SDEBUG("{}", estr);
      return false;
    }

    // unescape value
    wpi::SmallString<64> valueBuf;
    std::string_view value = wpi::UnescapeURI(rawValue, valueBuf, &error);
    if (error) {
      auto estr = fmt::format("could not unescape value \"{}\"", rawValue);
      SendError(os, 500, estr);
      SDEBUG("{}", estr);
      return false;
    }

    // Handle resolution, compression, and FPS.  These are handled locally
    // rather than passed to the source.
    if (param == "resolution") {
      auto [widthStr, heightStr] = wpi::split(value, 'x');
      int width = wpi::parse_integer<int>(widthStr, 10).value_or(-1);
      int height = wpi::parse_integer<int>(heightStr, 10).value_or(-1);
      if (width < 0) {
        response << param << ": \"width is not an integer\"\r\n";
        SWARNING("HTTP parameter \"{}\" width \"{}\" is not an integer", param,
                 widthStr);
        continue;
      }
      if (height < 0) {
        response << param << ": \"height is not an integer\"\r\n";
        SWARNING("HTTP parameter \"{}\" height \"{}\" is not an integer", param,
                 heightStr);
        continue;
      }
      m_width = width;
      m_height = height;
      response << param << ": \"ok\"\r\n";
      continue;
    }

    if (param == "fps") {
      if (auto v = wpi::parse_integer<int>(value, 10)) {
        m_fps = v.value();
        response << param << ": \"ok\"\r\n";
      } else {
        response << param << ": \"invalid integer\"\r\n";
        SWARNING("HTTP parameter \"{}\" value \"{}\" is not an integer", param,
                 value);
      }
      continue;
    }

    if (param == "compression") {
      if (auto v = wpi::parse_integer<int>(value, 10)) {
        m_compression = v.value();
        response << param << ": \"ok\"\r\n";
      } else {
        response << param << ": \"invalid integer\"\r\n";
        SWARNING("HTTP parameter \"{}\" value \"{}\" is not an integer", param,
                 value);
      }
      continue;
    }

    // ignore name parameter
    if (param == "name") {
      continue;
    }

    // try to assign parameter
    auto prop = source.GetPropertyIndex(param);
    if (!prop) {
      response << param << ": \"ignored\"\r\n";
      SWARNING("ignoring HTTP parameter \"{}\"", param);
      continue;
    }

    CS_Status status = 0;
    auto kind = source.GetPropertyKind(prop);
    switch (kind) {
      case CS_PROP_BOOLEAN:
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM: {
        if (auto v = wpi::parse_integer<int>(value, 10)) {
          wpi::print(response, "{}: {}\r\n", param, v.value());
          SDEBUG4("HTTP parameter \"{}\" value {}", param, value);
          source.SetProperty(prop, v.value(), &status);
        } else {
          response << param << ": \"invalid integer\"\r\n";
          SWARNING("HTTP parameter \"{}\" value \"{}\" is not an integer",
                   param, value);
        }
        break;
      }
      case CS_PROP_STRING: {
        response << param << ": \"ok\"\r\n";
        SDEBUG4("HTTP parameter \"{}\" value \"{}\"", param, value);
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
    wpi::raw_ostream& os) const {
  os << "<html><head><title>" << m_name << " CameraServer</title>"
     << "<meta charset=\"UTF-8\">";
}

// Send the root html file with controls for all the settable properties.
void MjpegServerImpl::ConnThread::SendHTML(wpi::raw_ostream& os,
                                           SourceImpl& source, bool header) {
  if (header) {
    SendHeader(os, 200, "OK", "text/html");
  }

  SendHTMLHeadTitle(os);
  os << startRootPage;
  wpi::SmallVector<int, 32> properties_vec;
  CS_Status status = 0;
  for (auto prop : source.EnumerateProperties(properties_vec, &status)) {
    wpi::SmallString<128> name_buf;
    auto name = source.GetPropertyName(prop, name_buf, &status);
    if (wpi::starts_with(name, "raw_")) {
      continue;
    }
    auto kind = source.GetPropertyKind(prop);
    wpi::print(os, "<p /><label for=\"{0}\">{0}</label>\n", name);
    switch (kind) {
      case CS_PROP_BOOLEAN:
        wpi::print(os,
                   "<input id=\"{0}\" type=\"checkbox\" "
                   "onclick=\"update('{0}', this.checked ? 1 : 0)\" ",
                   name);
        if (source.GetProperty(prop, &status) != 0) {
          os << "checked />\n";
        } else {
          os << " />\n";
        }
        break;
      case CS_PROP_INTEGER: {
        auto valI = source.GetProperty(prop, &status);
        auto min = source.GetPropertyMin(prop, &status);
        auto max = source.GetPropertyMax(prop, &status);
        auto step = source.GetPropertyStep(prop, &status);
        wpi::print(os,
                   "<input type=\"range\" min=\"{1}\" max=\"{2}\" "
                   "value=\"{3}\" id=\"{0}\" step=\"{4}\" "
                   "oninput=\"updateInt('#{0}op', '{0}', value)\" />\n",
                   name, min, max, valI, step);
        wpi::print(os, "<output for=\"{0}\" id=\"{0}op\">{1}</output>\n", name,
                   valI);
        break;
      }
      case CS_PROP_ENUM: {
        auto valE = source.GetProperty(prop, &status);
        auto choices = source.GetEnumPropertyChoices(prop, &status);
        int j = 0;
        for (auto choice = choices.begin(), end = choices.end(); choice != end;
             ++j, ++choice) {
          if (choice->empty()) {
            continue;  // skip empty choices
          }
          // replace any non-printable characters in name with spaces
          wpi::SmallString<128> ch_name;
          for (char ch : *choice) {
            ch_name.push_back(wpi::isPrint(ch) ? ch : ' ');
          }
          wpi::print(os,
                     "<input id=\"{0}{1}\" type=\"radio\" name=\"{0}\" "
                     "value=\"{2}\" onclick=\"update('{0}', {1})\"",
                     name, j, ch_name.str());
          if (j == valE) {
            os << " checked";
          }
          wpi::print(os, " /><label for=\"{}{}\">{}</label>\n", name, j,
                     ch_name.str());
        }
        break;
      }
      case CS_PROP_STRING: {
        wpi::SmallString<128> strval_buf;
        wpi::print(os,
                   "<input type=\"text\" id=\"{0}box\" name=\"{0}\" "
                   "value=\"{1}\" />\n",
                   name, source.GetStringProperty(prop, strval_buf, &status));
        wpi::print(os,
                   "<input type=\"button\" value =\"Submit\" "
                   "onclick=\"update('{0}', {0}box.value)\" />\n",
                   name);
        break;
      }
      default:
        break;
    }
  }

  status = 0;
  auto info = GetUsbCameraInfo(Instance::GetInstance().FindSource(source).first,
                               &status);
  if (status == CS_OK) {
    os << "<p>USB device path: " << info.path << '\n';
    for (auto&& path : info.otherPaths) {
      os << "<p>Alternate device path: " << path << '\n';
    }
  }

  os << "<p>Supported Video Modes:</p>\n";
  os << "<table cols=\"4\" style=\"border: 1px solid black\">\n";
  os << "<tr><th>Pixel Format</th>" << "<th>Width</th>" << "<th>Height</th>"
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
      case VideoMode::kBGRA:
        os << "BGRA";
        break;
      case VideoMode::kGray:
        os << "gray";
        break;
      case VideoMode::kY16:
        os << "Y16";
        break;
      case VideoMode::kUYVY:
        os << "UYVY";
        break;
      default:
        os << "unknown";
        break;
    }
    wpi::print(os, "</td><td>{}</td><td>{}</td><td>{}</td></tr>", mode.width,
               mode.height, mode.fps);
  }
  os << "</table>\n";
  os << endRootPage << "\r\n";
  os.flush();
}

// Send a JSON file which is contains information about the source parameters.
void MjpegServerImpl::ConnThread::SendJSON(wpi::raw_ostream& os,
                                           SourceImpl& source, bool header) {
  if (header) {
    SendHeader(os, 200, "OK", "application/json");
  }

  os << "{\n\"controls\": [\n";
  wpi::SmallVector<int, 32> properties_vec;
  bool first = true;
  CS_Status status = 0;
  for (auto prop : source.EnumerateProperties(properties_vec, &status)) {
    if (first) {
      first = false;
    } else {
      os << ",\n";
    }
    os << '{';
    wpi::SmallString<128> name_buf;
    auto name = source.GetPropertyName(prop, name_buf, &status);
    auto kind = source.GetPropertyKind(prop);
    wpi::print(os, "\n\"name\": \"{}\"", name);
    wpi::print(os, ",\n\"id\": \"{}\"", prop);
    wpi::print(os, ",\n\"type\": \"{}\"", static_cast<int>(kind));
    wpi::print(os, ",\n\"min\": \"{}\"", source.GetPropertyMin(prop, &status));
    wpi::print(os, ",\n\"max\": \"{}\"", source.GetPropertyMax(prop, &status));
    wpi::print(os, ",\n\"step\": \"{}\"",
               source.GetPropertyStep(prop, &status));
    wpi::print(os, ",\n\"default\": \"{}\"",
               source.GetPropertyDefault(prop, &status));
    os << ",\n\"value\": \"";
    switch (kind) {
      case CS_PROP_BOOLEAN:
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM:
        wpi::print(os, "{}", source.GetProperty(prop, &status));
        break;
      case CS_PROP_STRING: {
        wpi::SmallString<128> strval_buf;
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
        if (j != 0) {
          os << ", ";
        }
        // replace any non-printable characters in name with spaces
        wpi::SmallString<128> ch_name;
        for (char ch : *choice) {
          ch_name.push_back(std::isprint(ch) ? ch : ' ');
        }
        wpi::print(os, "\"{}\": \"{}\"", j, ch_name.str());
      }
      os << "}\n";
    }
    os << '}';
  }
  os << "\n],\n\"modes\": [\n";
  first = true;
  for (auto mode : source.EnumerateVideoModes(&status)) {
    if (first) {
      first = false;
    } else {
      os << ",\n";
    }
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
      case VideoMode::kY16:
        os << "Y16";
        break;
      case VideoMode::kUYVY:
        os << "UYVY";
        break;
      default:
        os << "unknown";
        break;
    }
    wpi::print(os, "\",\n\"width\": \"{}\"", mode.width);
    wpi::print(os, ",\n\"height\": \"{}\"", mode.height);
    wpi::print(os, ",\n\"fps\": \"{}\"", mode.fps);
    os << '}';
  }
  os << "\n]\n}\n";
  os.flush();
}

MjpegServerImpl::MjpegServerImpl(std::string_view name, wpi::Logger& logger,
                                 Notifier& notifier, Telemetry& telemetry,
                                 std::string_view listenAddress, int port,
                                 std::unique_ptr<wpi::NetworkAcceptor> acceptor)
    : SinkImpl{name, logger, notifier, telemetry},
      m_listenAddress(listenAddress),
      m_port(port),
      m_acceptor{std::move(acceptor)} {
  m_active = true;

  SetDescription(fmt::format("HTTP Server on port {}", port));

  // Create properties
  m_widthProp = CreateProperty("width", [] {
    return std::make_unique<PropertyImpl>("width", CS_PROP_INTEGER, 1, 0, 0);
  });
  m_heightProp = CreateProperty("height", [] {
    return std::make_unique<PropertyImpl>("height", CS_PROP_INTEGER, 1, 0, 0);
  });
  m_compressionProp = CreateProperty("compression", [] {
    return std::make_unique<PropertyImpl>("compression", CS_PROP_INTEGER, -1,
                                          100, 1, -1, -1);
  });
  m_defaultCompressionProp = CreateProperty("default_compression", [] {
    return std::make_unique<PropertyImpl>("default_compression",
                                          CS_PROP_INTEGER, 0, 100, 1, 80, 80);
  });
  m_fpsProp = CreateProperty("fps", [] {
    return std::make_unique<PropertyImpl>("fps", CS_PROP_INTEGER, 1, 0, 0);
  });

  m_serverThread = std::thread(&MjpegServerImpl::ServerThreadMain, this);
}

MjpegServerImpl::~MjpegServerImpl() {
  Stop();
}

void MjpegServerImpl::Stop() {
  m_active = false;

  // wake up server thread by shutting down the socket
  m_acceptor->shutdown();

  // join server thread
  if (m_serverThread.joinable()) {
    m_serverThread.join();
  }

  // close streams
  for (auto& connThread : m_connThreads) {
    if (auto thr = connThread.GetThread()) {
      if (thr->m_stream) {
        thr->m_stream->close();
      }
    }
    connThread.Stop();
  }

  // wake up connection threads by forcing an empty frame to be sent
  if (auto source = GetSource()) {
    source->Wakeup();
  }
}

// Send HTTP response and a stream of JPG-frames
void MjpegServerImpl::ConnThread::SendStream(wpi::raw_socket_ostream& os) {
  if (m_noStreaming) {
    SERROR("Too many simultaneous client streams");
    SendError(os, 503, "Too many simultaneous streams");
    return;
  }

  os.SetUnbuffered();

  wpi::SmallString<256> header;
  wpi::raw_svector_ostream oss{header};

  SendHeader(oss, 200, "OK", "multipart/x-mixed-replace;boundary=" BOUNDARY);
  os << oss.str();

  SDEBUG("Headers send, sending stream now");

  Frame::Time lastFrameTime = 0;
  Frame::Time timePerFrame = 0;
  if (m_fps != 0) {
    timePerFrame = 1000000.0 / m_fps;
  }
  Frame::Time averageFrameTime = 0;
  Frame::Time averagePeriod = 1000000;  // 1 second window
  if (averagePeriod < timePerFrame) {
    averagePeriod = timePerFrame * 10;
  }

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
    if (!m_active) {
      break;
    }
    if (!frame) {
      // Bad frame; sleep for 20 ms so we don't consume all processor time.
      os << "\r\n";  // Keep connection alive
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }

    auto thisFrameTime = frame.GetTime();
    if (thisFrameTime != 0 && timePerFrame != 0 && lastFrameTime != 0) {
      Frame::Time deltaTime = thisFrameTime - lastFrameTime;

      // drop frame if it is early compared to the desired frame rate AND
      // the current average is higher than the desired average
      if (deltaTime < timePerFrame && averageFrameTime < timePerFrame) {
        // sleep for 1 ms so we don't consume all processor time
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }

      // update average
      if (averageFrameTime != 0) {
        averageFrameTime =
            averageFrameTime * (averagePeriod - timePerFrame) / averagePeriod +
            deltaTime * timePerFrame / averagePeriod;
      } else {
        averageFrameTime = deltaTime;
      }
    }

    int width = m_width != 0 ? m_width : frame.GetOriginalWidth();
    int height = m_height != 0 ? m_height : frame.GetOriginalHeight();
    Image* image = frame.GetImageMJPEG(
        width, height, m_compression,
        m_compression == -1 ? m_defaultCompression : m_compression);
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
      case VideoMode::kUYVY:
      case VideoMode::kRGB565:
      case VideoMode::kYUYV:
      case VideoMode::kY16:
      default:
        // Bad frame; sleep for 10 ms so we don't consume all processor time.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
    }

    SDEBUG4("sending frame size={} addDHT={}", size, addDHT);

    // print the individual mimetype and the length
    // sending the content-length fixes random stream disruption observed
    // with firefox
    lastFrameTime = thisFrameTime;
    double timestamp = lastFrameTime / 1000000.0;
    header.clear();
    oss << "\r\n--" BOUNDARY "\r\n"
        << "Content-Type: image/jpeg\r\n";
    wpi::print(oss, "Content-Length: {}\r\n", size);
    wpi::print(oss, "X-Timestamp: {}\r\n", timestamp);
    oss << "\r\n";
    os << oss.str();
    if (addDHT) {
      // Insert DHT data immediately before SOF
      os << std::string_view(data, locSOF);
      os << JpegGetDHT();
      os << std::string_view(data + locSOF, image->size() - locSOF);
    } else {
      os << std::string_view(data, size);
    }
    // os.flush();
  }
  StopStream();
}

void MjpegServerImpl::ConnThread::ProcessRequest() {
  wpi::raw_socket_istream is{*m_stream};
  wpi::raw_socket_ostream os{*m_stream, true};

  // Read the request string from the stream
  wpi::SmallString<128> reqBuf;
  std::string_view req = is.getline(reqBuf, 4096);
  if (is.has_error()) {
    SDEBUG("error getting request string");
    return;
  }

  enum { kCommand, kStream, kGetSettings, kGetSourceConfig, kRootPage } kind;
  std::string_view parameters;
  size_t pos;

  SDEBUG("HTTP request: '{}'\n", req);

  // Determine request kind.  Most of these are for mjpgstreamer
  // compatibility, others are for Axis camera compatibility.
  if ((pos = req.find("POST /stream")) != std::string_view::npos) {
    kind = kStream;
    parameters = wpi::substr(wpi::substr(req, req.find('?', pos + 12)), 1);
  } else if ((pos = req.find("GET /?action=stream")) !=
             std::string_view::npos) {
    kind = kStream;
    parameters = wpi::substr(wpi::substr(req, req.find('&', pos + 19)), 1);
  } else if ((pos = req.find("GET /stream.mjpg")) != std::string_view::npos) {
    kind = kStream;
    parameters = wpi::substr(wpi::substr(req, req.find('?', pos + 16)), 1);
  } else if (req.find("GET /settings") != std::string_view::npos &&
             req.find(".json") != std::string_view::npos) {
    kind = kGetSettings;
  } else if (req.find("GET /config") != std::string_view::npos &&
             req.find(".json") != std::string_view::npos) {
    kind = kGetSourceConfig;
  } else if (req.find("GET /input") != std::string_view::npos &&
             req.find(".json") != std::string_view::npos) {
    kind = kGetSettings;
  } else if (req.find("GET /output") != std::string_view::npos &&
             req.find(".json") != std::string_view::npos) {
    kind = kGetSettings;
  } else if ((pos = req.find("GET /?action=command")) !=
             std::string_view::npos) {
    kind = kCommand;
    parameters = wpi::substr(wpi::substr(req, req.find('&', pos + 20)), 1);
  } else if (req.find("GET / ") != std::string_view::npos || req == "GET /\n") {
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
  parameters = wpi::substr(parameters, 0, pos);
  SDEBUG("command parameters: \"{}\"", parameters);

  // Read the rest of the HTTP request.
  // The end of the request is marked by a single, empty line
  wpi::SmallString<128> lineBuf;
  for (;;) {
    if (wpi::starts_with(is.getline(lineBuf, 4096), "\n")) {
      break;
    }
    if (is.has_error()) {
      return;
    }
  }

  // Send response
  switch (kind) {
    case kStream:
      if (auto source = GetSource()) {
        SDEBUG("request for stream {}", source->GetName());
        if (!ProcessCommand(os, *source, parameters, false)) {
          return;
        }
      }
      SendStream(os);
      break;
    case kCommand:
      if (auto source = GetSource()) {
        ProcessCommand(os, *source, parameters, true);
      } else {
        SendHeader(os, 200, "OK", "text/plain");
        os << "Ignored due to no connected source." << "\r\n";
        SDEBUG("Ignored due to no connected source.");
      }
      break;
    case kGetSettings:
      SDEBUG("request for JSON file");
      if (auto source = GetSource()) {
        SendJSON(os, *source, true);
      } else {
        SendError(os, 404, "Resource not found");
      }
      break;
    case kGetSourceConfig:
      SDEBUG("request for JSON file");
      if (auto source = GetSource()) {
        SendHeader(os, 200, "OK", "application/json");
        CS_Status status = CS_OK;
        os << source->GetConfigJson(&status);
        os.flush();
      } else {
        SendError(os, 404, "Resource not found");
      }
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
  std::unique_lock lock(m_mutex);
  while (m_active) {
    while (!m_stream) {
      m_cond.wait(lock);
      if (!m_active) {
        return;
      }
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
    if (!m_active) {
      return;
    }

    SDEBUG("client connection from {}", stream->getPeerIP());

    auto source = GetSource();

    std::scoped_lock lock(m_mutex);
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
    it->Start(GetName(), m_logger);

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
    thr->m_width = GetProperty(m_widthProp)->value;
    thr->m_height = GetProperty(m_heightProp)->value;
    thr->m_compression = GetProperty(m_compressionProp)->value;
    thr->m_defaultCompression = GetProperty(m_defaultCompressionProp)->value;
    thr->m_fps = GetProperty(m_fpsProp)->value;
    thr->m_cond.notify_one();
  }

  SDEBUG("leaving server thread");
}

void MjpegServerImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {
  std::scoped_lock lock(m_mutex);
  for (auto& connThread : m_connThreads) {
    if (auto thr = connThread.GetThread()) {
      if (thr->m_source != source) {
        bool streaming = thr->m_streaming;
        if (thr->m_source && streaming) {
          thr->m_source->DisableSink();
        }
        thr->m_source = source;
        if (source && streaming) {
          thr->m_source->EnableSink();
        }
      }
    }
  }
}

namespace cs {

CS_Sink CreateMjpegServer(std::string_view name, std::string_view listenAddress,
                          int port, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSink(
      CS_SINK_MJPEG,
      std::make_shared<MjpegServerImpl>(
          name, inst.logger, inst.notifier, inst.telemetry, listenAddress, port,
          std::unique_ptr<wpi::NetworkAcceptor>(
              new wpi::TCPAcceptor(port, listenAddress, inst.logger))));
}

std::string GetMjpegServerListenAddress(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_MJPEG) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<MjpegServerImpl&>(*data->sink).GetListenAddress();
}

int GetMjpegServerPort(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_MJPEG) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<MjpegServerImpl&>(*data->sink).GetPort();
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateMjpegServer(const struct WPI_String* name,
                             const struct WPI_String* listenAddress, int port,
                             CS_Status* status) {
  return cs::CreateMjpegServer(wpi::to_string_view(name),
                               wpi::to_string_view(listenAddress), port,
                               status);
}

void CS_GetMjpegServerListenAddress(CS_Sink sink, WPI_String* listenAddress,
                                    CS_Status* status) {
  cs::ConvertToC(listenAddress, cs::GetMjpegServerListenAddress(sink, status));
}

int CS_GetMjpegServerPort(CS_Sink sink, CS_Status* status) {
  return cs::GetMjpegServerPort(sink, status);
}

}  // extern "C"
