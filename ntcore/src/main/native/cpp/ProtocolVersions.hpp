// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

namespace wpi::nt {

static constexpr unsigned int NT_4_0 = 0x0400;
static constexpr unsigned int NT_4_1 = 0x0401;
static constexpr unsigned int NT_4_2 = 0x0402;

unsigned int ProtocolStringToVersion(const std::string_view proto);

}  // namespace wpi::nt
