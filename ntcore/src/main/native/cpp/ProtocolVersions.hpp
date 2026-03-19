// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <unordered_map>

namespace wpi::nt {

static constexpr unsigned int NT_4_0 = 0x0400;
static constexpr unsigned int NT_4_1 = 0x0401;
static constexpr unsigned int NT_4_2 = 0x0402;

// Map of protocol strings to client versions
static constexpr const std::unordered_map<std::string, unsigned int> PROTOCOL_VERSIONS{
    {"v4.2.networktables.first.wpi.edu", NT_4_2},
    {"v4.1.networktables.first.wpi.edu", NT_4_1},
    {"rtt.networktables.first.wpi.edu", NT_4_1},
    {"networktables.first.wpi.edu", NT_4_0}};

unsigned int StringToProto(std::string_view proto) {
    auto it = PROTOCOL_VERSIONS.find(proto);
    if (proto == PROTOCOL_VERSIONS.end()) {
        return NT_4_0;
    } else {
        return it->second;
    }
}

}  // namespace wpi::nt
