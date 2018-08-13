/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "mockdata/HALValue.h"
#include "mockdata/wpi/StringRef.h"

namespace hal {

class Value;

void ConvertToC(const Value& in, HAL_Value* out);
std::shared_ptr<Value> ConvertFromC(const HAL_Value& value);
void ConvertToC(wpi::StringRef in, HALString* out);
inline wpi::StringRef ConvertFromC(const HALString& str) {
  return wpi::StringRef(str.str, str.len);
}

}  // namespace hal
