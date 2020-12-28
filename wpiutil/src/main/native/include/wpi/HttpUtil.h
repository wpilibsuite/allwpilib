// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_HTTPUTIL_H_
#define WPIUTIL_WPI_HTTPUTIL_H_

#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "wpi/ArrayRef.h"
#include "wpi/NetworkStream.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringMap.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"
#include "wpi/raw_istream.h"
#include "wpi/raw_socket_istream.h"
#include "wpi/raw_socket_ostream.h"

namespace wpi {

// Unescape a %xx-encoded URI.
// @param buf Buffer for output
// @param error Set to true if an error occurred
// @return Escaped string
StringRef UnescapeURI(const Twine& str, SmallVectorImpl<char>& buf,
                      bool* error);

// Escape a string with %xx-encoding.
// @param buf Buffer for output
// @param spacePlus If true, encodes spaces to '+' rather than "%20"
// @return Escaped string
StringRef EscapeURI(const Twine& str, SmallVectorImpl<char>& buf,
                    bool spacePlus = true);

// Parse a set of HTTP headers.  Saves just the Content-Type and Content-Length
// fields.
// @param is Input stream
// @param contentType If not null, Content-Type contents are saved here.
// @param contentLength If not null, Content-Length contents are saved here.
// @return False if error occurred in input stream
bool ParseHttpHeaders(raw_istream& is, SmallVectorImpl<char>* contentType,
                      SmallVectorImpl<char>* contentLength);

// Look for a MIME multi-part boundary.  On return, the input stream will
// be located at the character following the boundary (usually "\r\n").
// @param is Input stream
// @param boundary Boundary string to scan for (not including "--" prefix)
// @param saveBuf If not null, all scanned characters up to but not including
//     the boundary are saved to this string
// @return False if error occurred on input stream, true if boundary found.
bool FindMultipartBoundary(wpi::raw_istream& is, StringRef boundary,
                           std::string* saveBuf);

/**
 * Map for looking up elements of the query portion of a URI.  Does not
 * handle multiple elements with the same name.  This is a reference type;
 * it does not make a copy of the query string, so it is important that the
 * query string has a lifetime at least as long as this object.
 */
class HttpQueryMap {
 public:
  /**
   * Constructs an empty map (with no entries).
   */
  HttpQueryMap() = default;

  /**
   * Constructs from an escaped query string.  Note: does not make a copy of
   * the query string, so it must not be a temporary.
   *
   * @param query query string
   */
  explicit HttpQueryMap(StringRef query);

  /**
   * Gets an element of the query string.  Both the name and the returned
   * value are unescaped strings.
   *
   * @param name name (unescaped)
   * @param buf result buffer for value
   * @return Optional unescaped value.  Returns an empty optional if the
   *         name is not present in the query map.
   */
  std::optional<StringRef> Get(StringRef name,
                               SmallVectorImpl<char>& buf) const;

 private:
  StringMap<StringRef> m_elems;
};

class HttpPathRef;

/**
 * Class for HTTP path matching.  A root path is represented as a single
 * empty element, otherwise the path consists of each non-empty element
 * between the '/' characters:
 *  - "" -> []
 *  - "/" -> [""]
 *  - "/foo" -> ["foo"]
 *  - "/foo/bar" -> ["foo", "bar"]
 *  - "/foo//bar/" -> ["foo", "bar"]
 *
 * All path elements are unescaped.
 */
class HttpPath {
 public:
  /**
   * Constructs an empty HTTP path.
   */
  HttpPath() = default;

  /**
   * Constructs a HTTP path from an escaped path string.  Makes a copy of the
   * path, so it's safe to be a temporary.
   */
  explicit HttpPath(StringRef path);

  /**
   * Evaluates to true if the path is not empty.
   */
  explicit operator bool() const { return !empty(); }

  /**
   * Returns true if the path has no elements.
   */
  bool empty() const { return m_pathEnds.empty(); }

  /**
   * Returns number of elements in the path.
   */
  size_t size() const { return m_pathEnds.size(); }

  /**
   * Returns true if the path exactly matches the provided match list.
   *
   * @param match match list
   * @return True if path equals match list
   */
  bool equals(std::initializer_list<StringRef> match) const {
    return equals(0, makeArrayRef(match.begin(), match.end()));
  }
  bool equals(ArrayRef<StringRef> match) const { return equals(0, match); }
  bool equals(StringRef match) const { return equals(0, makeArrayRef(match)); }

  /**
   * Returns true if the elements of the path starting at the "start" element
   * match the provided match list, and there are no additional elements.
   *
   * @param start element to start matching at
   * @param match match list
   * @return True if match
   */
  bool equals(size_t start, std::initializer_list<StringRef> match) const {
    return equals(start, makeArrayRef(match.begin(), match.end()));
  }
  bool equals(size_t start, ArrayRef<StringRef> match) const {
    if (m_pathEnds.size() != (start + match.size())) {
      return false;
    }
    return startswith(start, match);
  }
  bool equals(size_t start, StringRef match) const {
    return equals(start, makeArrayRef(match));
  }

  /**
   * Returns true if the first elements of the path match the provided match
   * list.  The path may have additional elements.
   *
   * @param match match list
   * @return True if path starts with match list
   */
  bool startswith(std::initializer_list<StringRef> match) const {
    return startswith(0, makeArrayRef(match.begin(), match.end()));
  }
  bool startswith(ArrayRef<StringRef> match) const {
    return startswith(0, match);
  }
  bool startswith(StringRef match) const {
    return startswith(0, makeArrayRef(match));
  }

  /**
   * Returns true if the elements of the path starting at the "start" element
   * match the provided match list.  The path may have additional elements.
   *
   * @param start element to start matching at
   * @param match match list
   * @return True if path starting at the start element matches the match list
   */
  bool startswith(size_t start, std::initializer_list<StringRef> match) const {
    return startswith(start, makeArrayRef(match.begin(), match.end()));
  }

  bool startswith(size_t start, ArrayRef<StringRef> match) const;

  bool startswith(size_t start, StringRef match) const {
    return startswith(start, makeArrayRef(match));
  }

  /**
   * Gets a single element of the path.
   */
  StringRef operator[](size_t n) const {
    return m_pathBuf.slice(n == 0 ? 0 : m_pathEnds[n - 1], m_pathEnds[n]);
  }

  /**
   * Returns a path reference with the first N elements of the path removed.
   */
  HttpPathRef drop_front(size_t n) const;

 private:
  SmallString<128> m_pathBuf;
  SmallVector<size_t, 16> m_pathEnds;
};

/**
 * Proxy reference object for a portion of a HttpPath.
 */
class HttpPathRef {
 public:
  HttpPathRef() = default;
  /*implicit*/ HttpPathRef(const HttpPath& path,  // NOLINT
                           size_t start = 0)
      : m_path(&path), m_start(start) {}

  explicit operator bool() const { return !empty(); }
  bool empty() const { return m_path && m_path->size() == m_start; }
  size_t size() const { return m_path ? m_path->size() - m_start : 0; }

  bool equals(std::initializer_list<StringRef> match) const {
    return equals(0, makeArrayRef(match.begin(), match.end()));
  }
  bool equals(ArrayRef<StringRef> match) const { return equals(0, match); }
  bool equals(StringRef match) const { return equals(0, makeArrayRef(match)); }

  bool equals(size_t start, std::initializer_list<StringRef> match) const {
    return equals(start, makeArrayRef(match.begin(), match.end()));
  }
  bool equals(size_t start, ArrayRef<StringRef> match) const {
    return m_path ? m_path->equals(m_start + start, match) : false;
  }
  bool equals(size_t start, StringRef match) const {
    return equals(start, makeArrayRef(match));
  }

  bool startswith(std::initializer_list<StringRef> match) const {
    return startswith(0, makeArrayRef(match.begin(), match.end()));
  }
  bool startswith(ArrayRef<StringRef> match) const {
    return startswith(0, match);
  }
  bool startswith(StringRef match) const {
    return startswith(0, makeArrayRef(match));
  }

  bool startswith(size_t start, std::initializer_list<StringRef> match) const {
    return startswith(start, makeArrayRef(match.begin(), match.end()));
  }
  bool startswith(size_t start, ArrayRef<StringRef> match) const {
    return m_path ? m_path->startswith(m_start + start, match) : false;
  }
  bool startswith(size_t start, StringRef match) const {
    return startswith(start, makeArrayRef(match));
  }

  StringRef operator[](size_t n) const {
    return m_path ? m_path->operator[](m_start + n) : StringRef{};
  }
  HttpPathRef drop_front(size_t n) const {
    return m_path ? m_path->drop_front(m_start + n) : HttpPathRef{};
  }

 private:
  const HttpPath* m_path = nullptr;
  size_t m_start = 0;
};

class HttpLocation {
 public:
  HttpLocation() = default;
  HttpLocation(const Twine& url_, bool* error, std::string* errorMsg);

  std::string url;       // retain copy
  std::string user;      // unescaped
  std::string password;  // unescaped
  std::string host;
  int port;
  std::string path;  // escaped, not including leading '/'
  std::vector<std::pair<std::string, std::string>> params;  // unescaped
  std::string fragment;
};

class HttpRequest {
 public:
  HttpRequest() = default;

  explicit HttpRequest(const HttpLocation& loc)
      : host{loc.host}, port{loc.port} {
    SetPath(loc.path, loc.params);
    SetAuth(loc);
  }

  template <typename T>
  HttpRequest(const HttpLocation& loc, const T& extraParams);

  HttpRequest(const HttpLocation& loc, StringRef path_)
      : host{loc.host}, port{loc.port}, path{path_} {
    SetAuth(loc);
  }

  template <typename T>
  HttpRequest(const HttpLocation& loc, StringRef path_, const T& params)
      : host{loc.host}, port{loc.port} {
    SetPath(path_, params);
    SetAuth(loc);
  }

  SmallString<128> host;
  int port;
  std::string auth;
  SmallString<128> path;

 private:
  void SetAuth(const HttpLocation& loc);
  template <typename T>
  void SetPath(StringRef path_, const T& params);

  template <typename T>
  static StringRef GetFirst(const T& elem) {
    return elem.first;
  }
  template <typename T>
  static StringRef GetFirst(const StringMapEntry<T>& elem) {
    return elem.getKey();
  }
  template <typename T>
  static StringRef GetSecond(const T& elem) {
    return elem.second;
  }
};

class HttpConnection {
 public:
  HttpConnection(std::unique_ptr<wpi::NetworkStream> stream_, int timeout)
      : stream{std::move(stream_)}, is{*stream, timeout}, os{*stream, true} {}

  bool Handshake(const HttpRequest& request, std::string* warnMsg);

  std::unique_ptr<wpi::NetworkStream> stream;
  wpi::raw_socket_istream is;
  wpi::raw_socket_ostream os;

  // Valid after Handshake() is successful
  SmallString<64> contentType;
  SmallString<64> contentLength;

  explicit operator bool() const { return stream && !is.has_error(); }
};

class HttpMultipartScanner {
 public:
  explicit HttpMultipartScanner(StringRef boundary, bool saveSkipped = false) {
    Reset(saveSkipped);
    SetBoundary(boundary);
  }

  // Change the boundary.  This is only safe to do when IsDone() is true (or
  // immediately after construction).
  void SetBoundary(StringRef boundary);

  // Reset the scanner.  This allows reuse of internal buffers.
  void Reset(bool saveSkipped = false);

  // Execute the scanner.  Will automatically call Reset() on entry if IsDone()
  // is true.
  // @param in input data
  // @return the input not consumed; empty if all input consumed
  StringRef Execute(StringRef in);

  // Returns true when the boundary has been found.
  bool IsDone() const { return m_state == kDone; }

  // Get the skipped data.  Will be empty if saveSkipped was false.
  StringRef GetSkipped() const {
    return m_saveSkipped ? StringRef{m_buf} : StringRef{};
  }

 private:
  SmallString<64> m_boundaryWith, m_boundaryWithout;

  // Internal state
  enum State { kBoundary, kPadding, kDone };
  State m_state;
  size_t m_posWith, m_posWithout;
  enum Dashes { kUnknown, kWith, kWithout };
  Dashes m_dashes;

  // Buffer
  bool m_saveSkipped;
  std::string m_buf;
};

}  // namespace wpi

#include "HttpUtil.inl"

#endif  // WPIUTIL_WPI_HTTPUTIL_H_
