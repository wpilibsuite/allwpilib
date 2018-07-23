/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/HttpParser.h"

using namespace wpi;

uint32_t HttpParser::GetParserVersion() {
  return static_cast<uint32_t>(http_parser_version());
}

HttpParser::HttpParser(Type type) {
  http_parser_init(&m_parser,
                   static_cast<http_parser_type>(static_cast<int>(type)));
  m_parser.data = this;

  http_parser_settings_init(&m_settings);

  // Unlike the underlying http_parser library, we don't perform callbacks
  // (other than body) with partial data; instead we buffer and call the user
  // callback only when the data is complete.

  // on_message_begin: initialize our state, call user callback
  m_settings.on_message_begin = [](http_parser* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.m_urlBuf.clear();
    self.m_state = kStart;
    self.messageBegin();
    return self.m_aborted;
  };

  // on_url: collect into buffer
  m_settings.on_url = [](http_parser* p, const char* at, size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    // append to buffer
    if ((self.m_urlBuf.size() + length) > self.m_maxLength) return 1;
    self.m_urlBuf += StringRef{at, length};
    self.m_state = kUrl;
    return 0;
  };

  // on_status: collect into buffer, call user URL callback
  m_settings.on_status = [](http_parser* p, const char* at,
                            size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    // use valueBuf for the status
    if ((self.m_valueBuf.size() + length) > self.m_maxLength) return 1;
    self.m_valueBuf += StringRef{at, length};
    self.m_state = kStatus;
    return 0;
  };

  // on_header_field: collect into buffer, call user header/status callback
  m_settings.on_header_field = [](http_parser* p, const char* at,
                                  size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);

    // once we're in header, we know the URL is complete
    if (self.m_state == kUrl) {
      self.url(self.m_urlBuf);
      if (self.m_aborted) return 1;
    }

    // once we're in header, we know the status is complete
    if (self.m_state == kStatus) {
      self.status(self.m_valueBuf);
      if (self.m_aborted) return 1;
    }

    // if we previously were in value state, that means we finished a header
    if (self.m_state == kValue) {
      self.header(self.m_fieldBuf, self.m_valueBuf);
      if (self.m_aborted) return 1;
    }

    // clear field and value when we enter this state
    if (self.m_state != kField) {
      self.m_state = kField;
      self.m_fieldBuf.clear();
      self.m_valueBuf.clear();
    }

    // append data to field buffer
    if ((self.m_fieldBuf.size() + length) > self.m_maxLength) return 1;
    self.m_fieldBuf += StringRef{at, length};
    return 0;
  };

  // on_header_field: collect into buffer
  m_settings.on_header_value = [](http_parser* p, const char* at,
                                  size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);

    // if we weren't previously in value state, clear the buffer
    if (self.m_state != kValue) {
      self.m_state = kValue;
      self.m_valueBuf.clear();
    }

    // append data to value buffer
    if ((self.m_valueBuf.size() + length) > self.m_maxLength) return 1;
    self.m_valueBuf += StringRef{at, length};
    return 0;
  };

  // on_headers_complete: call user status/header/complete callback
  m_settings.on_headers_complete = [](http_parser* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);

    // if we previously were in url state, that means we finished the url
    if (self.m_state == kUrl) {
      self.url(self.m_urlBuf);
      if (self.m_aborted) return 1;
    }

    // if we previously were in status state, that means we finished the status
    if (self.m_state == kStatus) {
      self.status(self.m_valueBuf);
      if (self.m_aborted) return 1;
    }

    // if we previously were in value state, that means we finished a header
    if (self.m_state == kValue) {
      self.header(self.m_fieldBuf, self.m_valueBuf);
      if (self.m_aborted) return 1;
    }

    self.headersComplete(self.ShouldKeepAlive());
    return self.m_aborted;
  };

  // on_body: call user callback
  m_settings.on_body = [](http_parser* p, const char* at,
                          size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.body(StringRef{at, length}, self.IsBodyFinal());
    return self.m_aborted;
  };

  // on_message_complete: call user callback
  m_settings.on_message_complete = [](http_parser* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.messageComplete(self.ShouldKeepAlive());
    return self.m_aborted;
  };

  // on_chunk_header: call user callback
  m_settings.on_chunk_header = [](http_parser* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.chunkHeader(p->content_length);
    return self.m_aborted;
  };

  // on_chunk_complete: call user callback
  m_settings.on_chunk_complete = [](http_parser* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.chunkComplete();
    return self.m_aborted;
  };
}

void HttpParser::Reset(Type type) {
  http_parser_init(&m_parser,
                   static_cast<http_parser_type>(static_cast<int>(type)));
  m_parser.data = this;
  m_maxLength = 1024;
  m_state = kStart;
  m_urlBuf.clear();
  m_fieldBuf.clear();
  m_valueBuf.clear();
  m_aborted = false;
}
