/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/HttpUtil.h"

#include <cctype>

#include "wpi/Base64.h"
#include "wpi/STLExtras.h"
#include "wpi/StringExtras.h"
#include "wpi/TCPConnector.h"
#include "wpi/raw_ostream.h"

namespace wpi {

StringRef UnescapeURI(const Twine& str, SmallVectorImpl<char>& buf,
                      bool* error) {
  SmallString<128> strBuf;
  StringRef strStr = str.toStringRef(strBuf);
  buf.clear();
  for (auto i = strStr.begin(), end = strStr.end(); i != end; ++i) {
    // pass non-escaped characters to output
    if (*i != '%') {
      // decode + to space
      if (*i == '+')
        buf.push_back(' ');
      else
        buf.push_back(*i);
      continue;
    }

    // are there enough characters left?
    if (i + 2 >= end) {
      *error = true;
      return StringRef{};
    }

    // replace %xx with the corresponding character
    unsigned val1 = hexDigitValue(*++i);
    if (val1 == -1U) {
      *error = true;
      return StringRef{};
    }
    unsigned val2 = hexDigitValue(*++i);
    if (val2 == -1U) {
      *error = true;
      return StringRef{};
    }
    buf.push_back((val1 << 4) | val2);
  }

  *error = false;
  return StringRef{buf.data(), buf.size()};
}

StringRef EscapeURI(const Twine& str, SmallVectorImpl<char>& buf,
                    bool spacePlus) {
  static const char* const hexLut = "0123456789ABCDEF";

  SmallString<128> strBuf;
  StringRef strStr = str.toStringRef(strBuf);
  buf.clear();
  for (auto i = strStr.begin(), end = strStr.end(); i != end; ++i) {
    // pass unreserved characters to output
    if (std::isalnum(*i) || *i == '-' || *i == '_' || *i == '.' || *i == '~') {
      buf.push_back(*i);
      continue;
    }

    // encode space to +
    if (spacePlus && *i == ' ') {
      buf.push_back('+');
      continue;
    }

    // convert others to %xx
    buf.push_back('%');
    buf.push_back(hexLut[((*i) >> 4) & 0x0f]);
    buf.push_back(hexLut[(*i) & 0x0f]);
  }

  return StringRef{buf.data(), buf.size()};
}

bool ParseHttpHeaders(raw_istream& is, SmallVectorImpl<char>* contentType,
                      SmallVectorImpl<char>* contentLength) {
  if (contentType) contentType->clear();
  if (contentLength) contentLength->clear();

  bool inContentType = false;
  bool inContentLength = false;
  SmallString<64> lineBuf;
  for (;;) {
    StringRef line = is.getline(lineBuf, 1024).rtrim();
    if (is.has_error()) return false;
    if (line.empty()) return true;  // empty line signals end of headers

    // header fields start at the beginning of the line
    if (!std::isspace(line[0])) {
      inContentType = false;
      inContentLength = false;
      StringRef field;
      std::tie(field, line) = line.split(':');
      field = field.rtrim();
      if (field.equals_lower("content-type"))
        inContentType = true;
      else if (field.equals_lower("content-length"))
        inContentLength = true;
      else
        continue;  // ignore other fields
    }

    // collapse whitespace
    line = line.ltrim();

    // save field data
    if (inContentType && contentType)
      contentType->append(line.begin(), line.end());
    else if (inContentLength && contentLength)
      contentLength->append(line.begin(), line.end());
  }
}

bool FindMultipartBoundary(raw_istream& is, StringRef boundary,
                           std::string* saveBuf) {
  SmallString<64> searchBuf;
  searchBuf.resize(boundary.size() + 2);
  size_t searchPos = 0;

  // Per the spec, the --boundary should be preceded by \r\n, so do a first
  // pass of 1-byte reads to throw those away (common case) and keep the
  // last non-\r\n character in searchBuf.
  if (!saveBuf) {
    do {
      is.read(searchBuf.data(), 1);
      if (is.has_error()) return false;
    } while (searchBuf[0] == '\r' || searchBuf[0] == '\n');
    searchPos = 1;
  }

  // Look for --boundary.  Read boundarysize+2 bytes at a time
  // during the search to speed up the reads, then fast-scan for -,
  // and only then match the entire boundary.  This will be slow if
  // there's a bunch of continuous -'s in the output, but that's unlikely.
  for (;;) {
    is.read(searchBuf.data() + searchPos, searchBuf.size() - searchPos);
    if (is.has_error()) return false;

    // Did we find the boundary?
    if (searchBuf[0] == '-' && searchBuf[1] == '-' &&
        searchBuf.substr(2) == boundary)
      return true;

    // Fast-scan for '-'
    size_t pos = searchBuf.find('-', searchBuf[0] == '-' ? 1 : 0);
    if (pos == StringRef::npos) {
      if (saveBuf) saveBuf->append(searchBuf.data(), searchBuf.size());
    } else {
      if (saveBuf) saveBuf->append(searchBuf.data(), pos);

      // move '-' and following to start of buffer (next read will fill)
      std::memmove(searchBuf.data(), searchBuf.data() + pos,
                   searchBuf.size() - pos);
      searchPos = searchBuf.size() - pos;
    }
  }
}

HttpLocation::HttpLocation(const Twine& url_, bool* error,
                           std::string* errorMsg)
    : url{url_.str()} {
  // Split apart into components
  StringRef query{url};

  // scheme:
  StringRef scheme;
  std::tie(scheme, query) = query.split(':');
  if (!scheme.equals_lower("http")) {
    *errorMsg = "only supports http URLs";
    *error = true;
    return;
  }

  // "//"
  if (!query.startswith("//")) {
    *errorMsg = "expected http://...";
    *error = true;
    return;
  }
  query = query.drop_front(2);

  // user:password@host:port/
  StringRef authority;
  std::tie(authority, query) = query.split('/');

  StringRef userpass, hostport;
  std::tie(userpass, hostport) = authority.split('@');
  // split leaves the RHS empty if the split char isn't present...
  if (hostport.empty()) {
    hostport = userpass;
    userpass = StringRef{};
  }

  if (!userpass.empty()) {
    StringRef rawUser, rawPassword;
    std::tie(rawUser, rawPassword) = userpass.split(':');
    SmallString<64> userBuf, passBuf;
    user = UnescapeURI(rawUser, userBuf, error);
    if (*error) {
      raw_string_ostream oss(*errorMsg);
      oss << "could not unescape user \"" << rawUser << "\"";
      oss.flush();
      return;
    }
    password = UnescapeURI(rawPassword, passBuf, error);
    if (*error) {
      raw_string_ostream oss(*errorMsg);
      oss << "could not unescape password \"" << rawPassword << "\"";
      oss.flush();
      return;
    }
  }

  StringRef portStr;
  std::tie(host, portStr) = hostport.rsplit(':');
  if (host.empty()) {
    *errorMsg = "host is empty";
    *error = true;
    return;
  }
  if (portStr.empty()) {
    port = 80;
  } else if (portStr.getAsInteger(10, port)) {
    raw_string_ostream oss(*errorMsg);
    oss << "port \"" << portStr << "\" is not an integer";
    oss.flush();
    *error = true;
    return;
  }

  // path?query#fragment
  std::tie(query, fragment) = query.split('#');
  std::tie(path, query) = query.split('?');

  // Split query string into parameters
  while (!query.empty()) {
    // split out next param and value
    StringRef rawParam, rawValue;
    std::tie(rawParam, query) = query.split('&');
    if (rawParam.empty()) continue;  // ignore "&&"
    std::tie(rawParam, rawValue) = rawParam.split('=');

    // unescape param
    *error = false;
    SmallString<64> paramBuf;
    StringRef param = UnescapeURI(rawParam, paramBuf, error);
    if (*error) {
      raw_string_ostream oss(*errorMsg);
      oss << "could not unescape parameter \"" << rawParam << "\"";
      oss.flush();
      return;
    }

    // unescape value
    SmallString<64> valueBuf;
    StringRef value = UnescapeURI(rawValue, valueBuf, error);
    if (*error) {
      raw_string_ostream oss(*errorMsg);
      oss << "could not unescape value \"" << rawValue << "\"";
      oss.flush();
      return;
    }

    params.emplace_back(std::make_pair(param, value));
  }

  *error = false;
}

void HttpRequest::SetAuth(const HttpLocation& loc) {
  if (!loc.user.empty()) {
    SmallString<64> userpass;
    userpass += loc.user;
    userpass += ':';
    userpass += loc.password;
    Base64Encode(userpass, &auth);
  }
}

bool HttpConnection::Handshake(const HttpRequest& request,
                               std::string* warnMsg) {
  // send GET request
  os << "GET /" << request.path << " HTTP/1.1\r\n";
  os << "Host: " << request.host << "\r\n";
  if (!request.auth.empty())
    os << "Authorization: Basic " << request.auth << "\r\n";
  os << "\r\n";
  os.flush();

  // read first line of response
  SmallString<64> lineBuf;
  StringRef line = is.getline(lineBuf, 1024).rtrim();
  if (is.has_error()) {
    *warnMsg = "disconnected before response";
    return false;
  }

  // see if we got a HTTP 200 response
  StringRef httpver, code, codeText;
  std::tie(httpver, line) = line.split(' ');
  std::tie(code, codeText) = line.split(' ');
  if (!httpver.startswith("HTTP")) {
    *warnMsg = "did not receive HTTP response";
    return false;
  }
  if (code != "200") {
    raw_string_ostream oss(*warnMsg);
    oss << "received " << code << " " << codeText << " response";
    oss.flush();
    return false;
  }

  // Parse headers
  if (!ParseHttpHeaders(is, &contentType, &contentLength)) {
    *warnMsg = "disconnected during headers";
    return false;
  }

  return true;
}

void HttpMultipartScanner::SetBoundary(StringRef boundary) {
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

StringRef HttpMultipartScanner::Execute(StringRef in) {
  if (m_state == kDone) Reset(m_saveSkipped);
  if (m_saveSkipped) m_buf += in;

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
    for (char ch : in.drop_front(pos)) {
      ++pos;
      if (ch == '\n') {
        // Found the LF; return remaining input buffer (following it)
        m_state = kDone;
        if (m_saveSkipped) m_buf.resize(m_buf.size() - in.size() + pos);
        return in.drop_front(pos);
      }
    }
  }

  // We consumed the entire input
  return StringRef{};
}

}  // namespace wpi
