// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_HTTPUTIL_INC_
#define WPINET_HTTPUTIL_INC_

#include <utility>

#include "wpinet/HttpUtil.h"

namespace wpi {

inline HttpPathRef HttpPath::drop_front(size_t n) const {
  return HttpPathRef(*this, n);
}

template <typename T>
HttpRequest::HttpRequest(const HttpLocation& loc, const T& extraParams)
    : host{loc.host}, port{loc.port} {
  StringMap<std::string_view> params;
  for (const auto& p : loc.params) {
    params.insert(std::make_pair(GetFirst(p), GetSecond(p)));
  }
  for (const auto& p : extraParams) {
    params.insert(std::make_pair(GetFirst(p), GetSecond(p)));
  }
  SetPath(loc.path, params);
  SetAuth(loc);
}

template <typename T>
void HttpRequest::SetPath(std::string_view path_, const T& params) {
  // Build location including query string
  raw_svector_ostream pathOs{path};
  pathOs << path_;
  bool first = true;
  for (const auto& param : params) {
    if (first) {
      pathOs << '?';
      first = false;
    } else {
      pathOs << '&';
    }
    SmallString<64> escapeBuf;
    pathOs << EscapeURI(GetFirst(param), escapeBuf, false);
    if (!GetSecond(param).empty()) {
      pathOs << '=' << EscapeURI(GetSecond(param), escapeBuf, false);
    }
  }
}

}  // namespace wpi

#endif  // WPINET_HTTPUTIL_INC_
