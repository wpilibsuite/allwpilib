// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>

#include <llhttp.h>

#include "wpi/util/Signal.h"
#include "wpi/util/SmallString.hpp"

namespace wpi::net {

/**
 * HTTP protocol parser.  Performs incremental parsing with callbacks for each
 * part of the HTTP protocol.  As this is incremental, it's suitable for use
 * with event based frameworks that provide arbitrary chunks of data.
 */
class HttpParser {
 public:
  enum class Type {
    REQUEST = HTTP_REQUEST,
    RESPONSE = HTTP_RESPONSE,
    BOTH = HTTP_BOTH
  };

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
  std::string_view Execute(std::string_view in);

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
  llhttp_method GetMethod() const {
    return static_cast<llhttp_method>(m_parser.method);
  }

  /**
   * Determine if an error occurred.
   * @return False if no error.
   */
  bool HasError() const {
    return (llhttp_get_errno(&m_parser) != HPE_OK &&
            llhttp_get_errno(&m_parser) != HPE_PAUSED_UPGRADE) ||
           m_invalidEofState;
  }

  /**
   * Get error number.
   */
  llhttp_errno GetError() const { return llhttp_get_errno(&m_parser); }

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
  bool ShouldKeepAlive() const { return llhttp_should_keep_alive(&m_parser); }

  /**
   * Pause the parser.
   * @param paused True to pause, false to unpause.
   */
  void Pause(bool paused) {
    if (paused) {
      llhttp_pause(&m_parser);
    } else {
      llhttp_resume(&m_parser);
    }
  }

  /**
   * Get URL.  Valid in and after the url callback has been called.
   */
  std::string_view GetUrl() const { return m_urlBuf.str(); }

  /**
   * Message begin callback.
   */
  wpi::util::sig::Signal<> messageBegin;

  /**
   * URL callback.
   *
   * The parameter to the callback is the complete URL string.
   */
  wpi::util::sig::Signal<std::string_view> url;

  /**
   * Status callback.
   *
   * The parameter to the callback is the complete status string.
   * GetStatusCode() can be used to get the numeric status code.
   */
  wpi::util::sig::Signal<std::string_view> status;

  /**
   * Header field callback.
   *
   * The parameters to the callback are the field name and field value.
   */
  wpi::util::sig::Signal<std::string_view, std::string_view> header;

  /**
   * Headers complete callback.
   *
   * The parameter to the callback is whether the connection should be kept
   * alive.  If this is false, then this should be the last message on the
   * connection.  If you are the server, respond with the "Connection: close"
   * header.  If you are the client, close the connection.
   */
  wpi::util::sig::Signal<bool> headersComplete;

  /**
   * Body data callback.
   *
   * The parameter to the callback is the data chunk. Note this callback will be
   * called multiple times arbitrarily (e.g. it's possible that it may be called
   * with just a few characters at a time).
   */
  wpi::util::sig::Signal<std::string_view> body;

  /**
   * Headers complete callback.
   *
   * The parameter to the callback is whether the connection should be kept
   * alive.  If this is false, then this should be the last message on the
   * connection.  If you are the server, respond with the "Connection: close"
   * header.  If you are the client, close the connection.
   */
  wpi::util::sig::Signal<bool> messageComplete;

  /**
   * Chunk header callback.
   *
   * The parameter to the callback is the chunk size.
   */
  wpi::util::sig::Signal<uint64_t> chunkHeader;

  /**
   * Chunk complete callback.
   */
  wpi::util::sig::Signal<> chunkComplete;

 private:
  llhttp_t m_parser;
  llhttp_settings_t m_settings;

  size_t m_maxLength = 1024;
  enum class State { START, URL, STATUS, FIELD, VALUE };
  State m_state = State::START;
  wpi::util::SmallString<128> m_urlBuf;
  wpi::util::SmallString<32> m_fieldBuf;
  wpi::util::SmallString<128> m_valueBuf;

  bool m_aborted = false;
  bool m_invalidEofState = false;
};

}  // namespace wpi::net
