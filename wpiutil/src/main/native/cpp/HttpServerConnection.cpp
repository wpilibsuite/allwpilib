/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/HttpServerConnection.h"

#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_uv_ostream.h"

using namespace wpi;

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
  m_request.header.connect([this](StringRef name, StringRef value) {
    if (name.equals_lower("accept-encoding") && value.contains("gzip")) {
      m_acceptGzip = true;
    }
  });

  // pass incoming data to HTTP parser
  m_dataConn =
      stream->data.connect_connection([this](uv::Buffer& buf, size_t size) {
        m_request.Execute(StringRef{buf.base, size});
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

void HttpServerConnection::BuildCommonHeaders(raw_ostream& os) {
  os << "Server: WebServer/1.0\r\n"
        "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, "
        "post-check=0, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n";
}

void HttpServerConnection::BuildHeader(raw_ostream& os, int code,
                                       const Twine& codeText,
                                       const Twine& contentType,
                                       uint64_t contentLength,
                                       const Twine& extra) {
  os << "HTTP/" << m_request.GetMajor() << '.' << m_request.GetMinor() << ' '
     << code << ' ' << codeText << "\r\n";
  if (contentLength == 0) m_keepAlive = false;
  if (!m_keepAlive) os << "Connection: close\r\n";
  BuildCommonHeaders(os);
  os << "Content-Type: " << contentType << "\r\n";
  if (contentLength != 0) os << "Content-Length: " << contentLength << "\r\n";
  os << "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: *\r\n";
  SmallString<128> extraBuf;
  StringRef extraStr = extra.toStringRef(extraBuf);
  if (!extraStr.empty()) os << extraStr;
  os << "\r\n";  // header ends with a blank line
}

void HttpServerConnection::SendData(ArrayRef<uv::Buffer> bufs,
                                    bool closeAfter) {
  m_stream.Write(bufs, [ closeAfter, stream = &m_stream ](
                           MutableArrayRef<uv::Buffer> bufs, uv::Error) {
    for (auto&& buf : bufs) buf.Deallocate();
    if (closeAfter) stream->Close();
  });
}

void HttpServerConnection::SendResponse(int code, const Twine& codeText,
                                        const Twine& contentType,
                                        StringRef content,
                                        const Twine& extraHeader) {
  SmallVector<uv::Buffer, 4> toSend;
  raw_uv_ostream os{toSend, 4096};
  BuildHeader(os, code, codeText, contentType, content.size(), extraHeader);
  os << content;
  // close after write completes if we aren't keeping alive
  SendData(os.bufs(), !m_keepAlive);
}

void HttpServerConnection::SendStaticResponse(int code, const Twine& codeText,
                                              const Twine& contentType,
                                              StringRef content, bool gzipped,
                                              const Twine& extraHeader) {
  // TODO: handle remote side not accepting gzip (very rare)

  StringRef contentEncodingHeader;
  if (gzipped /* && m_acceptGzip*/)
    contentEncodingHeader = "Content-Encoding: gzip\r\n";

  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os{bufs, 4096};
  BuildHeader(os, code, codeText, contentType, content.size(),
              extraHeader + contentEncodingHeader);
  // can send content without copying
  bufs.emplace_back(content);

  m_stream.Write(bufs, [ closeAfter = !m_keepAlive, stream = &m_stream ](
                           MutableArrayRef<uv::Buffer> bufs, uv::Error) {
    // don't deallocate the static content
    for (auto&& buf : bufs.drop_back()) buf.Deallocate();
    if (closeAfter) stream->Close();
  });
}

void HttpServerConnection::SendError(int code, const Twine& message) {
  StringRef codeText, extra, baseMessage;
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
  SmallString<256> content = baseMessage;
  content += "\r\n";
  message.toVector(content);
  SendResponse(code, codeText, "text/plain", content, extra);
}
