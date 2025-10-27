// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_HTTPSERVERCONNECTION_H_
#define WPINET_HTTPSERVERCONNECTION_H_

#include <memory>
#include <span>
#include <string_view>

#include "wpinet/HttpParser.h"
#include "wpinet/uv/Stream.h"

namespace wpi {

class raw_ostream;

class HttpServerConnection {
 public:
  explicit HttpServerConnection(std::shared_ptr<uv::Stream> stream);
  virtual ~HttpServerConnection() = default;

 protected:
  /**
   * Process an incoming HTTP request.  This is called after the incoming
   * message completes (e.g. from the HttpParser::messageComplete callback).
   *
   * The implementation should read request details from m_request and call the
   * appropriate Send() functions to send a response back to the client.
   */
  virtual void ProcessRequest() = 0;

  /**
   * Build common response headers.
   *
   * Called by SendHeader() to send headers common to every response.
   * Each line must be terminated with "\r\n".
   *
   * The default implementation sends the following:
   * "Server: WebServer/1.0\r\n"
   * "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, "
   * "post-check=0, max-age=0\r\n"
   * "Pragma: no-cache\r\n"
   * "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"
   *
   * These parameters should ensure the browser does not cache the response.
   * A browser should connect for each file and not serve files from its cache.
   *
   * @param os response stream
   */
  virtual void BuildCommonHeaders(raw_ostream& os);

  /**
   * Build HTTP response header, along with other header information like
   * mimetype.  Calls BuildCommonHeaders().
   *
   * @param os response stream
   * @param code HTTP response code (e.g. 200)
   * @param codeText HTTP response code text (e.g. "OK")
   * @param contentType MIME content type (e.g. "text/plain")
   * @param contentLength Length of content.  If 0 is provided, m_keepAlive will
   *                      be set to false.
   * @param extra Extra HTTP headers to send, including final "\r\n"
   */
  virtual void BuildHeader(raw_ostream& os, int code, std::string_view codeText,
                           std::string_view contentType, uint64_t contentLength,
                           std::string_view extra = {});

  /**
   * Send data to client.
   *
   * This is a convenience wrapper around m_stream.Write() to provide
   * auto-close functionality.
   *
   * @param bufs Buffers to write.  Deallocate() will be called on each
   *             buffer after the write completes.  If different behavior
   *             is desired, call m_stream.Write() directly instead.
   * @param closeAfter close the connection after the write completes
   */
  void SendData(std::span<const uv::Buffer> bufs, bool closeAfter = false);

  /**
   * Send HTTP response, along with other header information like mimetype.
   * Calls BuildHeader().
   *
   * @param code HTTP response code (e.g. 200)
   * @param codeText HTTP response code text (e.g. "OK")
   * @param contentType MIME content type (e.g. "text/plain")
   * @param content Response message content
   * @param extraHeader Extra HTTP headers to send, including final "\r\n"
   */
  virtual void SendResponse(int code, std::string_view codeText,
                            std::string_view contentType,
                            std::string_view content,
                            std::string_view extraHeader = {});

  /**
   * Send HTTP response from static data, along with other header information
   * like mimetype.  Calls BuildHeader().  Supports gzip pre-compressed data
   * (it will decompress if the client does not accept gzip encoded data).
   * Unlike SendResponse(), content is not copied and its contents must remain
   * valid for an unspecified lifetime.
   *
   * @param code HTTP response code (e.g. 200)
   * @param codeText HTTP response code text (e.g. "OK")
   * @param contentType MIME content type (e.g. "text/plain")
   * @param content Response message content
   * @param gzipped True if content is gzip compressed
   * @param extraHeader Extra HTTP headers to send, including final "\r\n"
   */
  virtual void SendStaticResponse(int code, std::string_view codeText,
                                  std::string_view contentType,
                                  std::string_view content, bool gzipped,
                                  std::string_view extraHeader = {});

  /**
   * Send error header and message.
   * This provides standard code responses for 400, 401, 403, 404, 500, and 503.
   * Other codes will be reported as 501.  For arbitrary code handling, use
   * SendResponse() instead.
   *
   * @param code HTTP error code (e.g. 404)
   * @param message Additional message text
   */
  virtual void SendError(int code, std::string_view message = {});

  /** The HTTP request. */
  HttpParser m_request{HttpParser::kRequest};

  /** Whether the connection should be kept alive. */
  bool m_keepAlive = false;

  /** If gzip is an acceptable encoding for responses. */
  bool m_acceptGzip = false;

  /** The underlying stream for the connection. */
  uv::Stream& m_stream;

  /** The header reader connection. */
  sig::ScopedConnection m_dataConn;

  /** The end stream connection. */
  sig::ScopedConnection m_endConn;

  /** The message complete connection. */
  sig::Connection m_messageCompleteConn;
};

}  // namespace wpi

#endif  // WPINET_HTTPSERVERCONNECTION_H_
