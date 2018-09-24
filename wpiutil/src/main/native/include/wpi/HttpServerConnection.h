/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_HTTPSERVERCONNECTION_H_
#define WPIUTIL_WPI_HTTPSERVERCONNECTION_H_

#include <memory>

#include "wpi/ArrayRef.h"
#include "wpi/HttpParser.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"
#include "wpi/uv/Stream.h"

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
   * Each line must be terminated with \r\n.
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
   * @param extra Extra HTTP headers to send, not including final "\r\n"
   */
  void BuildHeader(raw_ostream& os, int code, const Twine& codeText,
                   const Twine& contentType, uint64_t contentLength,
                   const Twine& extra = Twine{});

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
  void SendData(ArrayRef<uv::Buffer> bufs, bool closeAfter = false);

  /**
   * Send HTTP response, along with other header information like mimetype.
   * Calls BuildHeader().
   *
   * @param code HTTP response code (e.g. 200)
   * @param codeText HTTP response code text (e.g. "OK")
   * @param contentType MIME content type (e.g. "text/plain")
   * @param content Response message content
   * @param extraHeader Extra HTTP headers to send, not including final "\r\n"
   */
  void SendResponse(int code, const Twine& codeText, const Twine& contentType,
                    StringRef content, const Twine& extraHeader = Twine{});

  /**
   * Send error header and message.
   * This provides standard code responses for 400, 401, 403, 404, 500, and 503.
   * Other codes will be reported as 501.  For arbitrary code handling, use
   * SendResponse() instead.
   *
   * @param code HTTP error code (e.g. 404)
   * @param message Additional message text
   */
  void SendError(int code, const Twine& message = Twine{});

  /** The HTTP request. */
  HttpParser m_request{HttpParser::kRequest};

  /** Whether the connection should be kept alive. */
  bool m_keepAlive = false;

  /** The underlying stream for the connection. */
  uv::Stream& m_stream;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_HTTPSERVERCONNECTION_H_
