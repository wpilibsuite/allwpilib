// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "ntcore_c.h"

namespace nt {

std::string_view TypeToString(NT_Type type);
NT_Type StringToType(std::string_view typeStr);
NT_Type StringToType3(std::string_view typeStr);

}  // namespace nt
