/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "MockData/HALValue.h"
#include "MockData/llvm/StringRef.h"

namespace hal {

class Value;

void ConvertToC(const Value& in, HAL_Value* out);
std::shared_ptr<Value> ConvertFromC(const HAL_Value& value);
void ConvertToC(llvm::StringRef in, HALString* out);
inline llvm::StringRef ConvertFromC(const HALString& str) {
  return llvm::StringRef(str.str, str.len);
}

}  // namespace hal
