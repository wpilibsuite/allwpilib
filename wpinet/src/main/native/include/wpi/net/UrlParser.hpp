// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_URLPARSER_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_URLPARSER_HPP_

#include <string_view>

#include <wpi/util/StringExtras.hpp>

#include "wpi/net/http_parser.hpp"

namespace wpi::net {

/**
 * Parses a URL into its constituent components.
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
    return wpi::util::substr(m_data, m_url.field_data[UF_SCHEMA].off,
                             m_url.field_data[UF_SCHEMA].len);
  }

  std::string_view GetHost() const {
    return wpi::util::substr(m_data, m_url.field_data[UF_HOST].off,
                             m_url.field_data[UF_HOST].len);
  }

  unsigned int GetPort() const { return m_url.port; }

  std::string_view GetPath() const {
    return wpi::util::substr(m_data, m_url.field_data[UF_PATH].off,
                             m_url.field_data[UF_PATH].len);
  }

  std::string_view GetQuery() const {
    return wpi::util::substr(m_data, m_url.field_data[UF_QUERY].off,
                             m_url.field_data[UF_QUERY].len);
  }

  std::string_view GetFragment() const {
    return wpi::util::substr(m_data, m_url.field_data[UF_FRAGMENT].off,
                             m_url.field_data[UF_FRAGMENT].len);
  }

  std::string_view GetUserInfo() const {
    return wpi::util::substr(m_data, m_url.field_data[UF_USERINFO].off,
                             m_url.field_data[UF_USERINFO].len);
  }

 private:
  bool m_error;
  std::string_view m_data;
  http_parser_url m_url;
};

}  // namespace wpi::net

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_URLPARSER_HPP_
