/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_LEB128_H_
#define WPIUTIL_SUPPORT_LEB128_H_

#include <cstddef>

#include "llvm/SmallVector.h"

namespace wpi {

class raw_istream;

uint64_t SizeUleb128(uint64_t val);
uint64_t WriteUleb128(llvm::SmallVectorImpl<char>& dest, uint64_t val);
uint64_t ReadUleb128(const char* addr, uint64_t* ret);
bool ReadUleb128(raw_istream& is, uint64_t* ret);

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_LEB128_H_
