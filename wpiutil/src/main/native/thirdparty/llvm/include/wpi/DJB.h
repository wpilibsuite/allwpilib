//===-- llvm/Support/DJB.h ---DJB Hash --------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains support for the DJ Bernstein hash function.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_DJB_H
#define WPIUTIL_WPI_DJB_H

#include <string_view>

namespace wpi {

/// The Bernstein hash function used by the DWARF accelerator tables.
inline uint32_t djbHash(std::string_view Buffer, uint32_t H = 5381) {
  for (unsigned char C : Buffer)
    H = (H << 5) + H + C;
  return H;
}

} // namespace wpi

#endif // WPIUTIL_WPI_DJB_H
