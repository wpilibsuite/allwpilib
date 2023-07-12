//===-- llvm/ADT/bit.h - C++20 <bit> ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the C++20 <bit> header.
///
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_BIT_H
#define WPIUTIL_WPI_BIT_H

#include "wpi/Compiler.h"
#include <cstdint>
#include <limits>
#include <type_traits>

#if !__has_builtin(__builtin_bit_cast)
#include <cstring>
#endif

#if defined(_MSC_VER) && !defined(_DEBUG)
#include <cstdlib>  // for _byteswap_{ushort,ulong,uint64}
#endif

namespace wpi {

// This implementation of bit_cast is different from the C++20 one in two ways:
//  - It isn't constexpr because that requires compiler support.
//  - It requires trivially-constructible To, to avoid UB in the implementation.
template <
    typename To, typename From,
    typename = std::enable_if_t<sizeof(To) == sizeof(From)>,
    typename = std::enable_if_t<std::is_trivially_constructible<To>::value>,
    typename = std::enable_if_t<std::is_trivially_copyable<To>::value>,
    typename = std::enable_if_t<std::is_trivially_copyable<From>::value>>
[[nodiscard]] inline To bit_cast(const From &from) noexcept {
#if __has_builtin(__builtin_bit_cast)
  return __builtin_bit_cast(To, from);
#else
  To to;
  std::memcpy(&to, &from, sizeof(To));
  return to;
#endif
}

/// Reverses the bytes in the given integer value V.
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
[[nodiscard]] constexpr T byteswap(T V) noexcept {
  if constexpr (sizeof(T) == 1) {
    return V;
  } else if constexpr (sizeof(T) == 2) {
    uint16_t UV = V;
#if defined(_MSC_VER) && !defined(_DEBUG)
    // The DLL version of the runtime lacks these functions (bug!?), but in a
    // release build they're replaced with BSWAP instructions anyway.
    return _byteswap_ushort(UV);
#else
    uint16_t Hi = UV << 8;
    uint16_t Lo = UV >> 8;
    return Hi | Lo;
#endif
  } else if constexpr (sizeof(T) == 4) {
    uint32_t UV = V;
#if __has_builtin(__builtin_bswap32)
    return __builtin_bswap32(UV);
#elif defined(_MSC_VER) && !defined(_DEBUG)
    return _byteswap_ulong(UV);
#else
    uint32_t Byte0 = UV & 0x000000FF;
    uint32_t Byte1 = UV & 0x0000FF00;
    uint32_t Byte2 = UV & 0x00FF0000;
    uint32_t Byte3 = UV & 0xFF000000;
    return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
#endif
  } else if constexpr (sizeof(T) == 8) {
    uint64_t UV = V;
#if __has_builtin(__builtin_bswap64)
    return __builtin_bswap64(UV);
#elif defined(_MSC_VER) && !defined(_DEBUG)
    return _byteswap_uint64(UV);
#else
    uint64_t Hi = wpi::byteswap<uint32_t>(UV);
    uint32_t Lo = wpi::byteswap<uint32_t>(UV >> 32);
    return (Hi << 32) | Lo;
#endif
  } else {
    static_assert(!sizeof(T *), "Don't know how to handle the given type.");
    return 0;
  }
}

} // namespace wpi

#endif
