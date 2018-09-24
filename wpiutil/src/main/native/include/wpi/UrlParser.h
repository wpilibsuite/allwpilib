/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_URLPARSER_H_
#define WPIUTIL_WPI_URLPARSER_H_

#include "wpi/StringRef.h"
#include "wpi/http_parser.h"

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
  UrlParser(StringRef in, bool isConnect) {
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

  StringRef GetSchema() const {
    return m_data.substr(m_url.field_data[UF_SCHEMA].off,
                         m_url.field_data[UF_SCHEMA].len);
  }

  StringRef GetHost() const {
    return m_data.substr(m_url.field_data[UF_HOST].off,
                         m_url.field_data[UF_HOST].len);
  }

  unsigned int GetPort() const { return m_url.port; }

  StringRef GetPath() const {
    return m_data.substr(m_url.field_data[UF_PATH].off,
                         m_url.field_data[UF_PATH].len);
  }

  StringRef GetQuery() const {
    return m_data.substr(m_url.field_data[UF_QUERY].off,
                         m_url.field_data[UF_QUERY].len);
  }

  StringRef GetFragment() const {
    return m_data.substr(m_url.field_data[UF_FRAGMENT].off,
                         m_url.field_data[UF_FRAGMENT].len);
  }

  StringRef GetUserInfo() const {
    return m_data.substr(m_url.field_data[UF_USERINFO].off,
                         m_url.field_data[UF_USERINFO].len);
  }

 private:
  bool m_error;
  StringRef m_data;
  http_parser_url m_url;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_URLPARSER_H_
