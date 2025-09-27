// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <array>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace wpi {

// derived from:
// https://codereview.stackexchange.com/questions/282514/string-literals-concatenation-with-support-for-dynamic-strings

/**
 * Fixed length string (array of character) for compile time use.
 *
 * @tparam N number of characters
 * @tparam Char character type
 * @tparam Traits character traits
 */
template <typename Char, typename Traits, size_t N>
  requires(N < (std::numeric_limits<size_t>::max)())
struct ct_string {
  std::array<Char, N + 1> chars;

  template <size_t M>
    requires(M <= (N + 1))
  constexpr ct_string(Char const (&s)[M]) {  // NOLINT
    if constexpr (M == (N + 1)) {
      if (s[N] != Char{}) {
        throw std::logic_error{"char array not null terminated"};
      }
    }

    // avoid dependency on <algorithm>
    // auto p = std::ranges::copy(s, chars.begin()).out;
    auto p = chars.begin();
    for (auto c : s) {
      *p++ = c;
    }
    // std::ranges::fill() isn't constexpr on GCC 11
    while (p != chars.end()) {
      *p++ = Char{};
    }
  }

  explicit constexpr ct_string(std::basic_string_view<Char, Traits> s) {
    // avoid dependency on <algorithm>
    // auto p = std::ranges::copy(s, chars.begin()).out;
    auto p = chars.begin();
    for (auto c : s) {
      *p++ = c;
    }
    // std::ranges::fill() isn't constexpr on GCC 11
    while (p != chars.end()) {
      *p++ = Char{};
    }
  }

  constexpr bool operator==(const ct_string<Char, Traits, N>&) const = default;

  constexpr bool operator==(const std::basic_string<Char, Traits>& rhs) const {
    if (size() != rhs.size()) {
      return false;
    }

    for (size_t i = 0; i < size(); ++i) {
      if (chars[i] != rhs[i]) {
        return false;
      }
    }

    return true;
  }

  constexpr bool operator==(std::basic_string_view<Char, Traits> rhs) const {
    if (size() != rhs.size()) {
      return false;
    }

    for (size_t i = 0; i < size(); ++i) {
      if (chars[i] != rhs[i]) {
        return false;
      }
    }

    return true;
  }

  template <size_t M>
    requires(N + 1 == M)
  constexpr bool operator==(Char const (&rhs)[M]) const {
    for (size_t i = 0; i < M; ++i) {
      if (chars[i] != rhs[i]) {
        return false;
      }
    }

    return true;
  }

  constexpr bool operator==(const Char* rhs) const {
    for (size_t i = 0; i < N + 1; ++i) {
      if (chars[i] != rhs[i]) {
        return false;
      }

      // If index of rhs's null terminator is less than lhs's size - 1, rhs is
      // shorter than lhs
      if (rhs[i] == '\0' && i < N) {
        return false;
      }
    }

    return true;
  }

  constexpr auto size() const noexcept { return N; }

  constexpr auto begin() const noexcept { return chars.begin(); }
  constexpr auto end() const noexcept { return chars.begin() + N; }

  constexpr auto data() const noexcept { return chars.data(); }
  constexpr auto c_str() const noexcept { return chars.data(); }

  constexpr operator std::basic_string<Char, Traits>()  // NOLINT
      const noexcept {
    return std::basic_string<Char, Traits>{chars.data(), N};
  }

  constexpr operator std::basic_string_view<Char, Traits>()  // NOLINT
      const noexcept {
    return std::basic_string_view<Char, Traits>{chars.data(), N};
  }
};

template <typename Char, size_t M>
ct_string(Char const (&s)[M]) -> ct_string<Char, std::char_traits<Char>, M - 1>;

inline namespace literals {
template <ct_string S>
constexpr auto operator""_ct_string() {
  return S;
}
}  // namespace literals

template <typename Char, typename Traits, size_t N1, size_t N2>
constexpr auto operator+(ct_string<Char, Traits, N1> const& s1,
                         ct_string<Char, Traits, N2> const& s2) noexcept {
  return Concat(s1, s2);
}

/**
 * Concatenates multiple fixed_strings into a larger fixed_string at compile
 * time.
 *
 * @param s1 first string
 * @param s second and later strings
 * @return concatenated string
 */
template <typename Char, typename Traits, size_t N1, size_t... N>
constexpr auto Concat(ct_string<Char, Traits, N1> const& s1,
                      ct_string<Char, Traits, N> const&... s) {
  // Need a dummy array to instantiate a ct_string.
  constexpr Char dummy[1] = {};
  auto res = ct_string<Char, Traits, (N1 + ... + N)>{dummy};

  auto p = res.chars.begin();
  auto append = [&p](auto&& s) {
    // avoid dependency on <algorithm>
    // p = std::ranges::copy(s, p).out;
    for (auto c : s) {
      *p++ = c;
    }
  };

  (append(s1), ..., append(s));

  return res;
}

// derived from:
// https://github.com/tcsullivan/constexpr-to-string/blob/master/to_string.hpp

/**
 * Converts any integral to a ct_string at compile-time.
 *
 * @tparam N number to convert
 * @tparam Base desired base, can be from 2 to 36
 * @tparam Char character type
 * @tparam Traits character traits
 */
template <intmax_t N, int Base = 10, typename Char = char,
          typename Traits = std::char_traits<Char>>
  requires(Base >= 2 && Base <= 36)
constexpr auto NumToCtString() {
  constexpr char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  auto buflen = [] {
    size_t len = N > 0 ? 0 : 1;
    for (auto n = N; n; len++, n /= Base) {
    }
    return len;
  };
  constexpr size_t size = buflen();

  constexpr Char dummy[1] = {};
  auto res = ct_string<Char, Traits, size>{dummy};

  auto ptr = res.chars.data() + size;
  if (N != 0) {
    for (auto n = N; n; n /= Base) {
      *--ptr = digits[(N < 0 ? -1 : 1) * (n % Base)];
    }
    if (N < 0) {
      *--ptr = '-';
    }
  } else {
    res.chars[0] = '0';
  }

  return res;
}

}  // namespace wpi
