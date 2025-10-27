// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/HttpServerConnection.hpp"

#include <memory>

#include <wpi/util/SmallString.hpp>
#include <wpi/util/SmallVector.hpp>
#include <wpi/util/SpanExtras.hpp>
#include <wpi/util/StringExtras.hpp>
#include <wpi/util/fmt/raw_ostream.hpp>
#include <wpi/util/print.hpp>

#include "wpi/net/raw_uv_ostream.hpp"

using namespace wpi::net;

HttpServerConnection::HttpServerConnection(std::shared_ptr<uv::Stream> stream)
    : m_stream(*stream) {
  // process HTTP messages
  m_messageCompleteConn =
      m_request.messageComplete.connect_connection([this](bool keepAlive) {
        m_keepAlive = keepAlive;
        ProcessRequest();
      });

  // look for Accept-Encoding headers to determine if gzip is acceptable
  m_request.messageBegin.connect([this] { m_acceptGzip = false; });
  m_request.header.connect(
      [this](std::string_view name, std::string_view value) {
        if (wpi::util::equals_lower(name, "accept-encoding") &&
            wpi::util::contains(value, "gzip")) {
          m_acceptGzip = true;
        }
      });

  // pass incoming data to HTTP parser
  m_dataConn =
      stream->data.connect_connection([this](uv::Buffer& buf, size_t size) {
        m_request.Execute({buf.base, size});
        if (m_request.HasError()) {
          // could not parse; just close the connection
          m_stream.Close();
        }
      });

  // close when remote side closes
  m_endConn =
      stream->end.connect_connection([h = stream.get()] { h->Close(); });

  // start reading
  stream->StartRead();
}

void HttpServerConnection::BuildCommonHeaders(wpi::util::raw_ostream& os) {
  os << "Server: WebServer/1.0\r\n"
        "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, "
        "post-check=0, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n";
}

void HttpServerConnection::BuildHeader(wpi::util::raw_ostream& os, int code,
                                       std::string_view codeText,
                                       std::string_view contentType,
                                       uint64_t contentLength,
                                       std::string_view extra) {
  wpi::util::print(os, "HTTP/{}.{} {} {}\r\n", m_request.GetMajor(),
                   m_request.GetMinor(), code, codeText);
  if (contentLength == 0) {
    m_keepAlive = false;
  }
  if (!m_keepAlive) {
    os << "Connection: close\r\n";
  }
  BuildCommonHeaders(os);
  os << "Content-Type: " << contentType << "\r\n";
  if (contentLength != 0) {
    wpi::util::print(os, "Content-Length: {}\r\n", contentLength);
  }
  os << "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n";
  if (!extra.empty()) {
    os << extra;
  }
  os << "\r\n";  // header ends with a blank line
}

void HttpServerConnection::SendData(std::span<const uv::Buffer> bufs,
                                    bool closeAfter) {
  m_stream.Write(bufs, [closeAfter, stream = &m_stream](auto bufs, uv::Error) {
    for (auto&& buf : bufs) {
      buf.Deallocate();
    }
    if (closeAfter) {
      stream->Close();
    }
  });
}

void HttpServerConnection::SendResponse(int code, std::string_view codeText,
                                        std::string_view contentType,
                                        std::string_view content,
                                        std::string_view extraHeader) {
  wpi::util::SmallVector<uv::Buffer, 4> toSend;
  raw_uv_ostream os{toSend, 4096};
  BuildHeader(os, code, codeText, contentType, content.size(), extraHeader);
  os << content;
  // close after write completes if we aren't keeping alive
  SendData(os.bufs(), !m_keepAlive);
}

void HttpServerConnection::SendStaticResponse(
    int code, std::string_view codeText, std::string_view contentType,
    std::string_view content, bool gzipped, std::string_view extraHeader) {
  // TODO: handle remote side not accepting gzip (very rare)

  std::string_view contentEncodingHeader;
  if (gzipped /* && m_acceptGzip*/) {
    contentEncodingHeader = "Content-Encoding: gzip\r\n";
  }

  wpi::util::SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os{bufs, 4096};
  BuildHeader(os, code, codeText, contentType, content.size(),
              fmt::format("{}{}", extraHeader, contentEncodingHeader));
  // can send content without copying
  bufs.emplace_back(content);

  m_stream.Write(bufs, [closeAfter = !m_keepAlive, stream = &m_stream](
                           auto bufs, uv::Error) {
    // don't deallocate the static content
    for (auto&& buf : wpi::util::drop_back(bufs)) {
      buf.Deallocate();
    }
    if (closeAfter) {
      stream->Close();
    }
  });
}

void HttpServerConnection::SendError(int code, std::string_view message) {
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
  wpi::util::SmallString<256> content{baseMessage};
  content += "\r\n";
  content += message;
  SendResponse(code, codeText, "text/plain", content.str(), extra);
}
