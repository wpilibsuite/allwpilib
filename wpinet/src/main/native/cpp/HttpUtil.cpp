// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/HttpUtil.h"

#include <cctype>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <wpi/Base64.h>
#include <wpi/StringExtras.h>
#include <wpi/raw_ostream.h>

namespace wpi {

wpi::expected<std::string, size_t> UnescapeURI(std::string_view str) {
  std::string out;
  out.reserve(str.size());
  for (auto i = str.begin(), end = str.end(); i != end; ++i) {
    // pass non-escaped characters to output
    if (*i != '%') {
      // decode + to space
      if (*i == '+') {
        out.push_back(' ');
      } else {
        out.push_back(*i);
      }
      continue;
    }

    // are there enough characters left?
    if (i + 2 >= end) {
      return wpi::unexpected{i - str.begin()};
    }

    // replace %xx with the corresponding character
    unsigned val1 = hexDigitValue(*++i);
    if (val1 == -1U) {
      return wpi::unexpected{i - str.begin()};
    }
    unsigned val2 = hexDigitValue(*++i);
    if (val2 == -1U) {
      return wpi::unexpected{i - str.begin()};
    }
    out.push_back((val1 << 4) | val2);
  }
  return out;
}

std::string EscapeURI(std::string_view str, bool spacePlus) {
  static const char* const hexLut = "0123456789ABCDEF";

  std::string out;
  out.reserve(str.size() + 1);
  for (auto i = str.begin(), end = str.end(); i != end; ++i) {
    // pass unreserved characters to output
    if (std::isalnum(*i) || *i == '-' || *i == '_' || *i == '.' || *i == '~') {
      out.push_back(*i);
      continue;
    }

    // encode space to +
    if (spacePlus && *i == ' ') {
      out.push_back('+');
      continue;
    }

    // convert others to %xx
    out.push_back('%');
    out.push_back(hexLut[((*i) >> 4) & 0x0f]);
    out.push_back(hexLut[(*i) & 0x0f]);
  }

  return out;
}

std::string EscapeHTML(std::string_view str) {
  std::string out;
  out.reserve(str.size() + 1);
  for (auto i = str.begin(), end = str.end(); i != end; ++i) {
    if (*i == '&') {
      out.append({'&', 'a', 'm', 'p', ';'});
    } else if (*i == '<') {
      out.append({'&', 'l', 't', ';'});
    } else if (*i == '>') {
      out.append({'&', 'g', 't', ';'});
    } else {
      out.push_back(*i);
    }
  }
  return out;
}

HttpQueryMap::HttpQueryMap(std::string_view query) {
  split(query, '&', 100, false, [&](auto elem) {
    auto [nameEsc, valueEsc] = split(elem, '=');
    // note: ignores duplicates
    if (auto name = wpi::UnescapeURI(nameEsc)) {
      m_elems.try_emplace(*name, valueEsc);
    }
  });
}

std::optional<std::string> HttpQueryMap::Get(std::string_view name) const {
  auto it = m_elems.find(name);
  if (it == m_elems.end()) {
    return {};
  }
  return wpi::UnescapeURI(it->second).value_or(std::string{});
}

HttpPath::HttpPath(std::string_view path) {
  // special-case root path to be a single empty element
  if (path == "/") {
    m_pathEnds.emplace_back(0);
    return;
  }
  split(path, '/', 100, false, [&](auto elem) {
    auto val = wpi::UnescapeURI(elem);
    if (!val) {
      m_pathEnds.clear();
      return;
    }
    m_pathBuf += *val;
    m_pathEnds.emplace_back(m_pathBuf.size());
  });
}

bool HttpPath::startswith(size_t start,
                          std::span<const std::string_view> match) const {
  if (m_pathEnds.size() < (start + match.size())) {
    return false;
  }
  bool first = start == 0;
  auto p = m_pathEnds.begin() + start;
  for (auto m : match) {
    auto val = slice(m_pathBuf, first ? 0 : *(p - 1), *p);
    if (val != m) {
      return false;
    }
    first = false;
    ++p;
  }
  return true;
}

std::string_view HttpPath::operator[](size_t n) const {
  return slice(m_pathBuf, n == 0 ? 0 : m_pathEnds[n - 1], m_pathEnds[n]);
}

bool ParseHttpHeaders(raw_istream& is, std::string* contentType,
                      std::string* contentLength) {
  if (contentType) {
    contentType->clear();
  }
  if (contentLength) {
    contentLength->clear();
  }

  bool inContentType = false;
  bool inContentLength = false;
  for (;;) {
    auto line = rtrim(is.getline(1024));
    if (is.has_error()) {
      return false;
    }
    if (line.empty()) {
      return true;  // empty line signals end of headers
    }

    // header fields start at the beginning of the line
    if (!std::isspace(line[0])) {
      inContentType = false;
      inContentLength = false;
      std::string_view field;
      std::tie(field, line) = split(line, ':');
      field = rtrim(field);
      if (equals_lower(field, "content-type")) {
        inContentType = true;
      } else if (equals_lower(field, "content-length")) {
        inContentLength = true;
      } else {
        continue;  // ignore other fields
      }
    }

    // collapse whitespace
    line = ltrim(line);

    // save field data
    if (inContentType && contentType) {
      contentType->append(line.begin(), line.end());
    } else if (inContentLength && contentLength) {
      contentLength->append(line.begin(), line.end());
    }
  }
}

bool FindMultipartBoundary(raw_istream& is, std::string_view boundary,
                           std::string* saveBuf) {
  std::string searchBuf;
  searchBuf.resize(boundary.size() + 2);
  size_t searchPos = 0;

  // Per the spec, the --boundary should be preceded by \r\n, so do a first
  // pass of 1-byte reads to throw those away (common case) and keep the
  // last non-\r\n character in searchBuf.
  if (!saveBuf) {
    do {
      is.read(searchBuf.data(), 1);
      if (is.has_error()) {
        return false;
      }
    } while (searchBuf[0] == '\r' || searchBuf[0] == '\n');
    searchPos = 1;
  }

  // Look for --boundary.  Read boundarysize+2 bytes at a time
  // during the search to speed up the reads, then fast-scan for -,
  // and only then match the entire boundary.  This will be slow if
  // there's a bunch of continuous -'s in the output, but that's unlikely.
  for (;;) {
    is.read(searchBuf.data() + searchPos, searchBuf.size() - searchPos);
    if (is.has_error()) {
      return false;
    }

    // Did we find the boundary?
    if (searchBuf[0] == '-' && searchBuf[1] == '-' &&
        wpi::substr(searchBuf, 2) == boundary) {
      return true;
    }

    // Fast-scan for '-'
    size_t pos = searchBuf.find('-', searchBuf[0] == '-' ? 1 : 0);
    if (pos == std::string_view::npos) {
      if (saveBuf) {
        saveBuf->append(searchBuf.data(), searchBuf.size());
      }
    } else {
      if (saveBuf) {
        saveBuf->append(searchBuf.data(), pos);
      }

      // move '-' and following to start of buffer (next read will fill)
      std::memmove(searchBuf.data(), searchBuf.data() + pos,
                   searchBuf.size() - pos);
      searchPos = searchBuf.size() - pos;
    }
  }
}

HttpLocation::HttpLocation(std::string_view url_, bool* error,
                           std::string* errorMsg)
    : url{url_} {
  // Split apart into components
  std::string_view query{url};

  // scheme:
  std::string_view scheme;
  std::tie(scheme, query) = split(query, ':');
  if (!equals_lower(scheme, "http")) {
    *errorMsg = "only supports http URLs";
    *error = true;
    return;
  }

  // "//"
  if (!starts_with(query, "//")) {
    *errorMsg = "expected http://...";
    *error = true;
    return;
  }
  query.remove_prefix(2);

  // user:password@host:port/
  std::string_view authority;
  std::tie(authority, query) = split(query, '/');

  auto [userpass, hostport] = split(authority, '@');
  // split leaves the RHS empty if the split char isn't present...
  if (hostport.empty()) {
    hostport = userpass;
    userpass = {};
  }

  if (!userpass.empty()) {
    auto [rawUser, rawPassword] = split(userpass, ':');
    auto user = UnescapeURI(rawUser);
    if (!user) {
      *errorMsg = fmt::format("could not unescape user \"{}\"", rawUser);
      *error = true;
      return;
    }
    auto password = UnescapeURI(rawPassword);
    if (!password) {
      *errorMsg =
          fmt::format("could not unescape password \"{}\"", rawPassword);
      *error = true;
      return;
    }
  }

  std::string_view portStr;
  std::tie(host, portStr) = rsplit(hostport, ':');
  if (host.empty()) {
    *errorMsg = "host is empty";
    *error = true;
    return;
  }
  if (portStr.empty()) {
    port = 80;
  } else if (auto p = parse_integer<int>(portStr, 10)) {
    port = p.value();
  } else {
    *errorMsg = fmt::format("port \"{}\" is not an integer", portStr);
    *error = true;
    return;
  }

  // path?query#fragment
  std::tie(query, fragment) = split(query, '#');
  std::tie(path, query) = split(query, '?');

  // Split query string into parameters
  while (!query.empty()) {
    // split out next param and value
    std::string_view rawParam, rawValue;
    std::tie(rawParam, query) = split(query, '&');
    if (rawParam.empty()) {
      continue;  // ignore "&&"
    }
    std::tie(rawParam, rawValue) = split(rawParam, '=');

    // unescape param
    auto param = UnescapeURI(rawParam);
    if (!param) {
      *errorMsg = fmt::format("could not unescape parameter \"{}\"", rawParam);
      *error = true;
      return;
    }

    // unescape value
    auto value = UnescapeURI(rawValue);
    if (!value) {
      *errorMsg = fmt::format("could not unescape value \"{}\"", rawValue);
      *error = true;
      return;
    }

    params.emplace_back(std::pair{*param, *value});
  }

  *error = false;
}

void HttpRequest::SetAuth(const HttpLocation& loc) {
  if (!loc.user.empty()) {
    auth = Base64Encode(fmt::format("{}:{}", loc.user, loc.password));
  }
}

bool HttpConnection::Handshake(const HttpRequest& request,
                               std::string* warnMsg) {
  // send GET request
  os << "GET /" << request.path << " HTTP/1.1\r\n";
  os << "Host: " << request.host << "\r\n";
  if (!request.auth.empty()) {
    os << "Authorization: Basic " << request.auth << "\r\n";
  }
  os << "\r\n";
  os.flush();

  // read first line of response
  auto line = rtrim(is.getline(1024));
  if (is.has_error()) {
    *warnMsg = "disconnected before response";
    return false;
  }

  // see if we got a HTTP 200 response
  std::string_view httpver, code, codeText;
  std::tie(httpver, line) = split(line, ' ');
  std::tie(code, codeText) = split(line, ' ');
  if (!starts_with(httpver, "HTTP")) {
    *warnMsg = "did not receive HTTP response";
    return false;
  }
  if (code != "200") {
    *warnMsg = fmt::format("received {} {} response", code, codeText);
    return false;
  }

  // Parse headers
  if (!ParseHttpHeaders(is, &contentType, &contentLength)) {
    *warnMsg = "disconnected during headers";
    return false;
  }

  return true;
}

void HttpMultipartScanner::SetBoundary(std::string_view boundary) {
  m_boundaryWith = "\n--";
  m_boundaryWith += boundary;
  m_boundaryWithout = "\n";
  m_boundaryWithout += boundary;
  m_dashes = kUnknown;
}

void HttpMultipartScanner::Reset(bool saveSkipped) {
  m_saveSkipped = saveSkipped;
  m_state = kBoundary;
  m_posWith = 0;
  m_posWithout = 0;
  m_buf.resize(0);
}

std::string_view HttpMultipartScanner::Execute(std::string_view in) {
  if (m_state == kDone) {
    Reset(m_saveSkipped);
  }
  if (m_saveSkipped) {
    m_buf += in;
  }

  size_t pos = 0;
  if (m_state == kBoundary) {
    for (char ch : in) {
      ++pos;
      if (m_dashes != kWithout) {
        if (ch == m_boundaryWith[m_posWith]) {
          ++m_posWith;
          if (m_posWith == m_boundaryWith.size()) {
            // Found the boundary; transition to padding
            m_state = kPadding;
            m_dashes = kWith;  // no longer accept plain 'boundary'
            break;
          }
        } else if (ch == m_boundaryWith[0]) {
          m_posWith = 1;
        } else {
          m_posWith = 0;
        }
      }

      if (m_dashes != kWith) {
        if (ch == m_boundaryWithout[m_posWithout]) {
          ++m_posWithout;
          if (m_posWithout == m_boundaryWithout.size()) {
            // Found the boundary; transition to padding
            m_state = kPadding;
            m_dashes = kWithout;  // no longer accept '--boundary'
            break;
          }
        } else if (ch == m_boundaryWithout[0]) {
          m_posWithout = 1;
        } else {
          m_posWithout = 0;
        }
      }
    }
  }

  if (m_state == kPadding) {
    for (char ch : drop_front(in, pos)) {
      ++pos;
      if (ch == '\n') {
        // Found the LF; return remaining input buffer (following it)
        m_state = kDone;
        if (m_saveSkipped) {
          m_buf.resize(m_buf.size() - in.size() + pos);
        }
        return drop_front(in, pos);
      }
    }
  }

  // We consumed the entire input
  return {};
}

}  // namespace wpi
