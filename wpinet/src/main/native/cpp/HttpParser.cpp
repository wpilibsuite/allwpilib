// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/HttpParser.hpp"

#include <string>

using namespace wpi::net;

HttpParser::HttpParser(Type type) {
  llhttp_init(&m_parser, static_cast<llhttp_type>(static_cast<int>(type)),
              &m_settings);
  m_parser.data = this;

  llhttp_settings_init(&m_settings);

  // Unlike the underlying llhttp library, we don't perform callbacks
  // (other than body) with partial data; instead we buffer and call the user
  // callback only when the data is complete.

  // on_message_begin: initialize our state, call user callback
  m_settings.on_message_begin = [](llhttp_t* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.m_urlBuf.clear();
    self.m_state = State::START;
    self.messageBegin();
    return self.m_aborted;
  };

  // on_url: collect into buffer
  m_settings.on_url = [](llhttp_t* p, const char* at, size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    // append to buffer
    if ((self.m_urlBuf.size() + length) > self.m_maxLength) {
      return 1;
    }
    self.m_urlBuf += std::string_view{at, length};
    self.m_state = State::URL;
    return 0;
  };

  // on_status: collect into buffer, call user URL callback
  m_settings.on_status = [](llhttp_t* p, const char* at, size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    // use valueBuf for the status
    if ((self.m_valueBuf.size() + length) > self.m_maxLength) {
      return 1;
    }
    self.m_valueBuf += std::string_view{at, length};
    self.m_state = State::STATUS;
    return 0;
  };

  // on_header_field: collect into buffer, call user header/status callback
  m_settings.on_header_field = [](llhttp_t* p, const char* at,
                                  size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);

    // once we're in header, we know the URL is complete
    if (self.m_state == State::URL) {
      self.url(self.m_urlBuf);
      if (self.m_aborted) {
        return 1;
      }
    }

    // once we're in header, we know the status is complete
    if (self.m_state == State::STATUS) {
      self.status(self.m_valueBuf);
      if (self.m_aborted) {
        return 1;
      }
    }

    // if we previously were in value state, that means we finished a header
    if (self.m_state == State::VALUE) {
      self.header(self.m_fieldBuf, self.m_valueBuf);
      if (self.m_aborted) {
        return 1;
      }
    }

    // clear field and value when we enter this state
    if (self.m_state != State::FIELD) {
      self.m_state = State::FIELD;
      self.m_fieldBuf.clear();
      self.m_valueBuf.clear();
    }

    // append data to field buffer
    if ((self.m_fieldBuf.size() + length) > self.m_maxLength) {
      return 1;
    }
    self.m_fieldBuf += std::string_view{at, length};
    return 0;
  };

  // on_header_field: collect into buffer
  m_settings.on_header_value = [](llhttp_t* p, const char* at,
                                  size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);

    // if we weren't previously in value state, clear the buffer
    if (self.m_state != State::VALUE) {
      self.m_state = State::VALUE;
      self.m_valueBuf.clear();
    }

    // append data to value buffer
    if ((self.m_valueBuf.size() + length) > self.m_maxLength) {
      return 1;
    }
    self.m_valueBuf += std::string_view{at, length};
    return 0;
  };

  // on_headers_complete: call user status/header/complete callback
  m_settings.on_headers_complete = [](llhttp_t* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);

    // if we previously were in url state, that means we finished the url
    if (self.m_state == State::URL) {
      self.url(self.m_urlBuf);
      if (self.m_aborted) {
        return 1;
      }
    }

    // if we previously were in status state, that means we finished the status
    if (self.m_state == State::STATUS) {
      self.status(self.m_valueBuf);
      if (self.m_aborted) {
        return 1;
      }
    }

    // if we previously were in value state, that means we finished a header
    if (self.m_state == State::VALUE) {
      self.header(self.m_fieldBuf, self.m_valueBuf);
      if (self.m_aborted) {
        return 1;
      }
    }

    self.headersComplete(self.ShouldKeepAlive());
    return self.m_aborted;
  };

  // on_body: call user callback
  m_settings.on_body = [](llhttp_t* p, const char* at, size_t length) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.body(std::string_view{at, length});
    return self.m_aborted;
  };

  // on_message_complete: call user callback
  m_settings.on_message_complete = [](llhttp_t* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.messageComplete(self.ShouldKeepAlive());
    return self.m_aborted;
  };

  // on_chunk_header: call user callback
  m_settings.on_chunk_header = [](llhttp_t* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.chunkHeader(p->content_length);
    return self.m_aborted;
  };

  // on_chunk_complete: call user callback
  m_settings.on_chunk_complete = [](llhttp_t* p) -> int {
    auto& self = *static_cast<HttpParser*>(p->data);
    self.chunkComplete();
    return self.m_aborted;
  };
}

std::string_view HttpParser::Execute(std::string_view in) {
  if (in.empty()) {
    llhttp_finish(&m_parser);
    return in;
  }
  if (llhttp_execute(&m_parser, in.data(), in.size()) == HPE_OK) {
    // Parse successful, consume all input
    in.remove_prefix(in.size());
  } else {
    // Otherwise, only consume what was parsed
    in.remove_prefix(llhttp_get_error_pos(&m_parser) - in.data());
  }
  return in;
}

void HttpParser::Reset(Type type) {
  llhttp_init(&m_parser, static_cast<llhttp_type>(static_cast<int>(type)),
              &m_settings);
  m_parser.data = this;
  m_maxLength = 1024;
  m_state = State::START;
  m_urlBuf.clear();
  m_fieldBuf.clear();
  m_valueBuf.clear();
  m_aborted = false;
}
