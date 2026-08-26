// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

// based on
// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138

namespace wpi::util {

namespace detail {
template <class T>
constexpr std::string_view GetTypeNameHelper() {
#if defined(_MSC_VER)
  return __FUNCSIG__;
#else
  return __PRETTY_FUNCTION__;
#endif
}

struct RawTypeNameTraits {
  static constexpr auto name = GetTypeNameHelper<double>();
  static constexpr size_t len = std::string_view("double").size();
  static constexpr size_t prefixLen = name.find("double");
  static_assert(prefixLen != std::string_view::npos,
                "cannot extract typename from function signature");
  static constexpr size_t suffixLen = name.size() - prefixLen - len;
};
}  // namespace detail

/**
 * Gets the name of a type as a string view.
 *
 * @tparam T the type
 * @return the name of the type
 */
template <typename T>
constexpr std::string_view GetTypeName() {
  std::string_view name = detail::GetTypeNameHelper<T>();
  return std::string_view(name.data() + detail::RawTypeNameTraits::prefixLen,
                          name.size() - detail::RawTypeNameTraits::prefixLen -
                              detail::RawTypeNameTraits::suffixLen);
}

}  // namespace wpi::util
