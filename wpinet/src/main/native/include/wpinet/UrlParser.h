// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_URLPARSER_H_
#define WPINET_URLPARSER_H_

#include <string_view>

#include <wpi/StringExtras.h>

#include "wpinet/http_parser.h"

namespace wpi {

/**
 * Parses a URL into its constiuent components.
 * `schema://userinfo@host:port/the/path?query#fragment`
 */
class UrlParser {
 public:
  /**
   * Parse a URL.
   * @param in input
   * @param isConnect
   */
  UrlParser(std::string_view in, bool isConnect) {
    m_data = in;
    http_parser_url_init(&m_url);
    m_error = http_parser_parse_url(in.data(), in.size(), isConnect, &m_url);
  }

  /**
   * Determine if the URL is valid (e.g. the parse was successful).
   */
  bool IsValid() const { return !m_error; }

  bool HasSchema() const { return (m_url.field_set & (1 << UF_SCHEMA)) != 0; }

  bool HasHost() const { return (m_url.field_set & (1 << UF_HOST)) != 0; }

  bool HasPort() const { return (m_url.field_set & (1 << UF_PORT)) != 0; }

  bool HasPath() const { return (m_url.field_set & (1 << UF_PATH)) != 0; }

  bool HasQuery() const { return (m_url.field_set & (1 << UF_QUERY)) != 0; }

  bool HasFragment() const {
    return (m_url.field_set & (1 << UF_FRAGMENT)) != 0;
  }

  bool HasUserInfo() const {
    return (m_url.field_set & (1 << UF_USERINFO)) != 0;
  }

  std::string_view GetSchema() const {
    return wpi::substr(m_data, m_url.field_data[UF_SCHEMA].off,
                       m_url.field_data[UF_SCHEMA].len);
  }

  std::string_view GetHost() const {
    return wpi::substr(m_data, m_url.field_data[UF_HOST].off,
                       m_url.field_data[UF_HOST].len);
  }

  unsigned int GetPort() const { return m_url.port; }

  std::string_view GetPath() const {
    return wpi::substr(m_data, m_url.field_data[UF_PATH].off,
                       m_url.field_data[UF_PATH].len);
  }

  std::string_view GetQuery() const {
    return wpi::substr(m_data, m_url.field_data[UF_QUERY].off,
                       m_url.field_data[UF_QUERY].len);
  }

  std::string_view GetFragment() const {
    return wpi::substr(m_data, m_url.field_data[UF_FRAGMENT].off,
                       m_url.field_data[UF_FRAGMENT].len);
  }

  std::string_view GetUserInfo() const {
    return wpi::substr(m_data, m_url.field_data[UF_USERINFO].off,
                       m_url.field_data[UF_USERINFO].len);
  }

 private:
  bool m_error;
  std::string_view m_data;
  http_parser_url m_url;
};

}  // namespace wpi

#endif  // WPINET_URLPARSER_H_
