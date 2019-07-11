/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_HTTPPARSER_H_
#define WPIUTIL_WPI_HTTPPARSER_H_

#include <stdint.h>

#include "wpi/Signal.h"
#include "wpi/SmallString.h"
#include "wpi/StringRef.h"
#include "wpi/http_parser.h"

namespace wpi {

/**
 * HTTP protocol parser.  Performs incremental parsing with callbacks for each
 * part of the HTTP protocol.  As this is incremental, it's suitable for use
 * with event based frameworks that provide arbitrary chunks of data.
 */
class HttpParser {
 public:
  enum Type {
    kRequest = HTTP_REQUEST,
    kResponse = HTTP_RESPONSE,
    kBoth = HTTP_BOTH
  };

  /**
   * Returns the library version. Bits 16-23 contain the major version number,
   * bits 8-15 the minor version number and bits 0-7 the patch level.
   */
  static uint32_t GetParserVersion();

  /**
   * Constructor.
   * @param type Type of parser (request or response or both)
   */
  explicit HttpParser(Type type);

  /**
   * Reset the parser to initial state.
   * This allows reusing the same parser object from request to request.
   * @param type Type of parser (request or response or both)
   */
  void Reset(Type type);

  /**
   * Set the maximum accepted length for URLs, field names, and field values.
   * The default is 1024.
   * @param len maximum length
   */
  void SetMaxLength(size_t len) { m_maxLength = len; }

  /**
   * Executes the parser.  An empty input is treated as EOF.
   * @param in input data
   * @return Trailing input data after the parse.
   */
  StringRef Execute(StringRef in) {
    return in.drop_front(
        http_parser_execute(&m_parser, &m_settings, in.data(), in.size()));
  }

  /**
   * Get HTTP major version.
   */
  unsigned int GetMajor() const { return m_parser.http_major; }

  /**
   * Get HTTP minor version.
   */
  unsigned int GetMinor() const { return m_parser.http_minor; }

  /**
   * Get HTTP status code.  Valid only on responses.  Valid in and after
   * the OnStatus() callback has been called.
   */
  unsigned int GetStatusCode() const { return m_parser.status_code; }

  /**
   * Get HTTP method.  Valid only on requests.
   */
  http_method GetMethod() const {
    return static_cast<http_method>(m_parser.method);
  }

  /**
   * Determine if an error occurred.
   * @return False if no error.
   */
  bool HasError() const { return m_parser.http_errno != HPE_OK; }

  /**
   * Get error number.
   */
  http_errno GetError() const {
    return static_cast<http_errno>(m_parser.http_errno);
  }

  /**
   * Abort the parse.  Call this from a callback handler to indicate an error.
   * This will result in GetError() returning one of the callback-related
   * errors (e.g. HPE_CB_message_begin).
   */
  void Abort() { m_aborted = true; }

  /**
   * Determine if an upgrade header was present and the parser has exited
   * because of that.  Should be checked when Execute() returns in addition to
   * checking GetError().
   * @return True if upgrade header, false otherwise.
   */
  bool IsUpgrade() const { return m_parser.upgrade; }

  /**
   * If this returns false in the headersComplete or messageComplete
   * callback, then this should be the last message on the connection.
   * If you are the server, respond with the "Connection: close" header.
   * If you are the client, close the connection.
   */
  bool ShouldKeepAlive() const { return http_should_keep_alive(&m_parser); }

  /**
   * Pause the parser.
   * @param paused True to pause, false to unpause.
   */
  void Pause(bool paused) { http_parser_pause(&m_parser, paused); }

  /**
   * Checks if this is the final chunk of the body.
   */
  bool IsBodyFinal() const { return http_body_is_final(&m_parser); }

  /**
   * Get URL.  Valid in and after the url callback has been called.
   */
  StringRef GetUrl() const { return m_urlBuf; }

  /**
   * Message begin callback.
   */
  sig::Signal<> messageBegin;

  /**
   * URL callback.
   *
   * The parameter to the callback is the complete URL string.
   */
  sig::Signal<StringRef> url;

  /**
   * Status callback.
   *
   * The parameter to the callback is the complete status string.
   * GetStatusCode() can be used to get the numeric status code.
   */
  sig::Signal<StringRef> status;

  /**
   * Header field callback.
   *
   * The parameters to the callback are the field name and field value.
   */
  sig::Signal<StringRef, StringRef> header;

  /**
   * Headers complete callback.
   *
   * The parameter to the callback is whether the connection should be kept
   * alive.  If this is false, then this should be the last message on the
   * connection.  If you are the server, respond with the "Connection: close"
   * header.  If you are the client, close the connection.
   */
  sig::Signal<bool> headersComplete;

  /**
   * Body data callback.
   *
   * The parameters to the callback is the data chunk and whether this is the
   * final chunk of data in the message.  Note this callback will be called
   * multiple times arbitrarily (e.g. it's possible that it may be called with
   * just a few characters at a time).
   */
  sig::Signal<StringRef, bool> body;

  /**
   * Headers complete callback.
   *
   * The parameter to the callback is whether the connection should be kept
   * alive.  If this is false, then this should be the last message on the
   * connection.  If you are the server, respond with the "Connection: close"
   * header.  If you are the client, close the connection.
   */
  sig::Signal<bool> messageComplete;

  /**
   * Chunk header callback.
   *
   * The parameter to the callback is the chunk size.
   */
  sig::Signal<uint64_t> chunkHeader;

  /**
   * Chunk complete callback.
   */
  sig::Signal<> chunkComplete;

 private:
  http_parser m_parser;
  http_parser_settings m_settings;

  size_t m_maxLength = 1024;
  enum { kStart, kUrl, kStatus, kField, kValue } m_state = kStart;
  SmallString<128> m_urlBuf;
  SmallString<32> m_fieldBuf;
  SmallString<128> m_valueBuf;

  bool m_aborted = false;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_HTTPPARSER_H_
