// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

namespace wpi::util {
template <typename T>
class SmallVectorImpl;
}

namespace wpi::net {
std::string GetHostname();
std::string_view GetHostname(wpi::util::SmallVectorImpl<char>& name);
}  // namespace wpi::net
