//===--- AlignOf.h - Portable calculation of type alignment -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the AlignedCharArrayUnion class.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_ALIGNOF_H
#define WPIUTIL_WPI_ALIGNOF_H

#include <algorithm>
#include <cstddef>

namespace wpi {

/// A suitably aligned and sized character array member which can hold elements
/// of any type.
template <typename... Ts> struct AlignedCharArrayUnion {
  alignas((std::max)({alignof(Ts)...}))
      std::byte buffer[(std::max)({static_cast<size_t>(1), sizeof(Ts)...})];
};

} // end namespace wpi

#endif // WPIUTIL_WPI_ALIGNOF_H
