// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/util/StringExtras.hpp"

namespace wpi::tunables::detail {

inline std::string_view NormalizeName(std::string_view path, std::string& buf) {
  // common case is a well formatted name, so check first
  if (wpi::util::starts_with(path, '/') && !wpi::util::contains(path, "//")) {
    return path;
  }
  buf.clear();
  buf.reserve(path.size() + 2);
  if (!wpi::util::starts_with(path, '/')) {
    buf.push_back('/');
  }
  char prevCh = '\0';
  for (auto ch : path) {
    if (ch != '/' || prevCh != '/') {
      buf.push_back(ch);
    }
    prevCh = ch;
  }
  return buf;
}

inline std::string_view NormalizePrefix(std::string_view prefix,
                                        std::string& buf) {
  return prefix.empty() ? prefix : NormalizeName(prefix, buf);
}

inline std::string_view NormalizeBackendPrefix(std::string_view prefix,
                                               std::string& buf) {
  if (prefix.empty()) {
    return prefix;
  }
  prefix = NormalizeName(prefix, buf);
  if (prefix.size() > 1 && wpi::util::ends_with(prefix, '/')) {
    if (buf.empty()) {
      buf = prefix;
    }
    while (buf.size() > 1 && wpi::util::ends_with(buf, '/')) {
      buf.pop_back();
    }
    return buf;
  }
  return prefix;
}

inline std::string GetChildTablePath(std::string_view path) {
  std::string tablePath{path};
  if (tablePath.empty() || tablePath.back() != '/') {
    tablePath.push_back('/');
  }
  return tablePath;
}

inline std::string NormalizeChildName(std::string_view name) {
  std::string buf;
  std::string_view normalized = NormalizeName(name, buf);
  if (!normalized.empty() && normalized.front() == '/') {
    normalized.remove_prefix(1);
  }
  return std::string{normalized};
}

inline bool IsPathOrDescendant(std::string_view path, std::string_view root) {
  if (root.empty() || root == "/" || path == root) {
    return true;
  }
  if (wpi::util::ends_with(root, '/')) {
    return wpi::util::starts_with(path, root);
  }
  return path.size() > root.size() && wpi::util::starts_with(path, root) &&
         path[root.size()] == '/';
}

}  // namespace wpi::tunables::detail
