//===- SwapByteOrder.h - Generic and optimized byte swaps -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares generic and optimized functions to swap the byte order of
// an integral type.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_SWAPBYTEORDER_H
#define WPIUTIL_WPI_SWAPBYTEORDER_H

#include "wpi/STLForwardCompat.h"
#include "wpi/bit.h"
#include <cstdint>
#include <type_traits>

namespace wpi {

namespace sys {

constexpr bool IsBigEndianHost =
    wpi::endianness::native == wpi::endianness::big;

static const bool IsLittleEndianHost = !IsBigEndianHost;

inline unsigned char      getSwappedBytes(unsigned char      C) { return wpi::byteswap(C); }
inline   signed char      getSwappedBytes( signed  char      C) { return wpi::byteswap(C); }
inline          char      getSwappedBytes(         char      C) { return wpi::byteswap(C); }

inline unsigned short     getSwappedBytes(unsigned short     C) { return wpi::byteswap(C); }
inline   signed short     getSwappedBytes(  signed short     C) { return wpi::byteswap(C); }

inline unsigned int       getSwappedBytes(unsigned int       C) { return wpi::byteswap(C); }
inline   signed int       getSwappedBytes(  signed int       C) { return wpi::byteswap(C); }

inline unsigned long      getSwappedBytes(unsigned long      C) { return wpi::byteswap(C); }
inline   signed long      getSwappedBytes(  signed long      C) { return wpi::byteswap(C); }

inline unsigned long long getSwappedBytes(unsigned long long C) { return wpi::byteswap(C); }
inline   signed long long getSwappedBytes(  signed long long C) { return wpi::byteswap(C); }

inline float getSwappedBytes(float C) {
  return wpi::bit_cast<float>(wpi::byteswap(wpi::bit_cast<uint32_t>(C)));
}

inline double getSwappedBytes(double C) {
  return wpi::bit_cast<double>(wpi::byteswap(wpi::bit_cast<uint64_t>(C)));
}

template <typename T>
inline std::enable_if_t<std::is_enum_v<T>, T> getSwappedBytes(T C) {
  return static_cast<T>(wpi::byteswap(wpi::to_underlying(C)));
}

template<typename T>
inline void swapByteOrder(T &Value) {
  Value = getSwappedBytes(Value);
}

} // end namespace sys
} // end namespace wpi

#endif
