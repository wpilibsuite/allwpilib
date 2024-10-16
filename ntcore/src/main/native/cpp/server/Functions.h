// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string_view>

namespace nt::server {

using SetPeriodicFunc = std::function<void(uint32_t repeatMs)>;
using Connected3Func =
    std::function<void(std::string_view name, uint16_t proto)>;

}  // namespace nt::server
