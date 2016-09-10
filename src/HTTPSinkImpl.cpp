/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HTTPSinkImpl.h"

#include <chrono>

#include "llvm/SmallString.h"
#include "llvm/StringExtras.h"
#include "support/raw_socket_istream.h"
#include "support/raw_socket_ostream.h"
#include "tcpsockets/TCPAcceptor.h"

#include "cameraserver_cpp.h"
#include "Handle.h"
#include "Log.h"
#include "SourceImpl.h"

using namespace cs;


// The boundary used for the M-JPEG stream.
// It separates the multipart stream of pictures
#define BOUNDARY "boundarydonotcross"

// Standard header to send along with other header information like mimetype.
//
// The parameters should ensure the browser does not cache our answer.
// A browser should connect for each file and not serve files from its cache.
// Using cached pictures would lead to showing old/outdated pictures.
// Many browsers seem to ignore, or at least not always obey, those headers.
void HTTPSinkImpl::SendHeader(llvm::raw_ostream& os, int code,
                              llvm::StringRef codeText,
                              llvm::StringRef contentType,
                              llvm::StringRef extra) {
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
void HTTPSinkImpl::SendError(llvm::raw_ostream& os, int code,
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
bool HTTPSinkImpl::ReadLine(wpi::raw_istream& istream,
                            llvm::SmallVectorImpl<char>& buffer, int maxLen) {
  buffer.clear();
  for (int i = 0; i < maxLen; ++i) {
    char c;
    istream.read(c);
    if (istream.has_error()) return false;
    buffer.push_back(c);
    if (c == '\n') break;
  }
  return true;
}

// Unescape a %xx-encoded URI.  Returns false on error.
bool HTTPSinkImpl::UnescapeURI(llvm::StringRef str,
                               llvm::SmallVectorImpl<char>& out) {
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
bool HTTPSinkImpl::ProcessCommand(llvm::raw_ostream& os, SourceImpl& source,
                                  llvm::StringRef parameters, bool respond) {
  // command format: param1=value1&param2=value2...
  while (!parameters.empty()) {
    // split out next param and value
    llvm::StringRef rawParam, rawValue;
    std::tie(rawParam, parameters) = parameters.split('&');
    if (rawParam.empty()) continue;  // ignore "&&"
    std::tie(rawParam, rawValue) = rawParam.split('=');
    if (rawParam.empty() || rawValue.empty()) continue;  // ignore "param="

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

    // try to assign parameter
  }

  if (respond) {
    // Send HTTP response
    SendHeader(os, 200, "OK", "text/plain");
    //os << command << ": " << res;
  }

  return true;
}

// Send a JSON file which is contains information about the source parameters.
void HTTPSinkImpl::SendJSON(llvm::raw_ostream& os, SourceImpl& source,
                            bool header) {
  if (header) SendHeader(os, 200, "OK", "application/x-javascript");

  os << "{\n\"controls\": [\n";
  llvm::SmallVector<int, 32> properties;
  source.EnumerateProperties(properties);
  bool first = true;
  for (auto prop : properties) {
    if (first)
      first = false;
    else
      os << ",\n";
    os << "{";
    llvm::SmallString<128> name;
    source.GetPropertyName(prop, name);
    auto type = source.GetPropertyType(prop);
    os << "\n\"name\": \"" << name << '"';
    os << ",\n\"id\": \"" << prop << '"';
    os << ",\n\"type\": \"" << type << '"';
    os << ",\n\"min\": \"" << source.GetPropertyMin(prop) << '"';
    os << ",\n\"max\": \"" << source.GetPropertyMax(prop) << '"';
    // os << ",\n\"step\": \"" << param->step << '"';
    // os << ",\n\"default\": \"" << param->default_value << '"';
    os << ",\n\"value\": \"";
    switch (type) {
      case CS_PROP_BOOLEAN:
        os << (source.GetBooleanProperty(prop) ? "1" : "0");
        break;
      case CS_PROP_DOUBLE:
        os << source.GetDoubleProperty(prop);
        break;
      case CS_PROP_STRING: {
        llvm::SmallString<128> strval;
        source.GetStringProperty(prop, strval);
        os << strval.str();
        break;
      }
      case CS_PROP_ENUM:
        os << source.GetEnumProperty(prop);
        break;
      default:
        break;
    }
    os << '"';
    os << ",\n\"dest\": \"0\",";
    // os << ",\n\"flags\": \"" << param->flags << '"';
    // os << ",\n\"group\": \"" << param->group << '"';

    // append the menu object to the menu typecontrols
    if (source.GetPropertyType(prop) == CS_PROP_ENUM) {
      os << ",\n\"menu\": {";
      auto choices = source.GetEnumPropertyChoices(prop);
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

HTTPSinkImpl::HTTPSinkImpl(llvm::StringRef name, llvm::StringRef description,
                           std::unique_ptr<wpi::NetworkAcceptor> acceptor)
    : SinkImpl{name},
      m_description(description),
      m_acceptor{std::move(acceptor)} {
  m_serverThread = std::thread(&HTTPSinkImpl::ServerThreadMain, this);
}

HTTPSinkImpl::~HTTPSinkImpl() { Stop(); }

void HTTPSinkImpl::GetDescription(llvm::SmallVectorImpl<char>& desc) const {
  llvm::raw_svector_ostream oss{desc};
  oss << m_description;
}

void HTTPSinkImpl::Stop() {
  m_active = false;

  // wake up server thread by shutting down the socket
  m_acceptor->shutdown();

  // join server thread
  if (m_serverThread.joinable()) m_serverThread.join();

  // close streams
  for (auto& stream : m_connStreams) stream->close();

  // wake up connection threads by forcing an empty frame to be sent
  if (auto source = GetSource())
    source->Wakeup();

  // join connection threads
  for (auto& connThread : m_connThreads) connThread.join();
}

// Send HTTP response and a stream of JPG-frames
void HTTPSinkImpl::SendStream(wpi::raw_socket_ostream& os) {
  os.SetUnbuffered();

  llvm::SmallString<256> header;
  llvm::raw_svector_ostream oss{header};

  SendHeader(oss, 200, "OK", "multipart/x-mixed-replace;boundary=" BOUNDARY,
             "Access-Control-Allow-Origin: *");
  oss << "--" BOUNDARY "\r\n";
  os << oss.str();

  DEBUG("Headers send, sending stream now");

  Enable();
  while (m_active && !os.has_error()) {
    auto source = GetSource();
    if (!source) {
      // Source disconnected; sleep for one second
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    Frame frame = source->GetNextFrame();  // blocks
    if (!m_active) break;
    if (!frame) {
      // Bad frame; sleep for 10 ms so we don't consume all processor time.
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    // print the individual mimetype and the length
    // sending the content-length fixes random stream disruption observed
    // with firefox
    double timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                           frame.time().time_since_epoch())
                           .count();
    header.clear();
    oss << "Content-Type: image/jpeg\r\n"
        << "Content-Length: " << frame.size(m_sourceChannel) << "\r\n"
        << "X-Timestamp: " << timestamp << "\r\n"
        << "\r\n";
    os << oss.str();
    os << frame.data(m_sourceChannel);
    os << "\r\n--" BOUNDARY "\r\n";
    // os.flush();
  }
  Disable();
}

// thread for clients that connected to this server
void HTTPSinkImpl::ConnThreadMain(wpi::NetworkStream* stream) {
  wpi::raw_socket_istream is{*stream};

  // Read the request string from the stream
  llvm::SmallString<128> buf;
  if (!ReadLine(is, buf, 4096)) return;

  wpi::raw_socket_ostream os{*stream};
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
  do {
    if (!ReadLine(is, buf, 4096)) return;
  } while (!buf.startswith("\r\n"));

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
        if (!ProcessCommand(os, *source, parameters, true)) return;
        SendHeader(os, 200, "OK", "text/plain");
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

// Main server thread
void HTTPSinkImpl::ServerThreadMain() {
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

    m_connThreads.emplace_back(&HTTPSinkImpl::ConnThreadMain, this,
                               stream.get());
    m_connStreams.emplace_back(std::move(stream));
  }

  DEBUG("leaving server thread");
}

namespace cs {

CS_Sink CreateHTTPSink(llvm::StringRef name, llvm::StringRef listenAddress,
                       int port, CS_Status* status) {
  llvm::SmallString<128> descBuf;
  llvm::raw_svector_ostream desc{descBuf};
  desc << "HTTP Server on port " << port;
  llvm::SmallString<128> str{listenAddress};
  auto sink = std::make_shared<HTTPSinkImpl>(
      name, desc.str(),
      std::unique_ptr<wpi::NetworkAcceptor>(
          new wpi::TCPAcceptor(port, str.c_str(), Logger::GetInstance())));
  return Sinks::GetInstance().Allocate(SinkData::kHTTP, sink);
}

void SetSinkSourceChannel(CS_Sink sink, int channel, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->type != SinkData::kHTTP) {
    *status = CS_WRONG_HANDLE_SUBTYPE;
    return;
  }
  static_cast<HTTPSinkImpl*>(data->sink.get())->SetSourceChannel(channel);
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateHTTPSink(const char* name, const char* listenAddress, int port,
                          CS_Status* status) {
  return cs::CreateHTTPSink(name, listenAddress, port, status);
}

void CS_SetSinkSourceChannel(CS_Sink sink, int channel, CS_Status* status) {
  return cs::SetSinkSourceChannel(sink, channel, status);
}

}  // extern "C"
