// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ProtocolVersions.hpp"

#include <unordered_map>

using namespace wpi::nt;

// Map of protocol strings to client versions
static const std::unordered_map<std::string_view, unsigned int>
    PROTOCOL_VERSIONS{{"v4.2.networktables.first.wpi.edu", NT_4_2},
                      {"v4.1.networktables.first.wpi.edu", NT_4_1},
                      {"rtt.networktables.first.wpi.edu", NT_4_1},
                      {"networktables.first.wpi.edu", NT_4_0}};

unsigned int wpi::nt::ProtocolStringToVersion(const std::string_view proto) {
  auto it = PROTOCOL_VERSIONS.find(proto);
  if (it == PROTOCOL_VERSIONS.end()) {
    return NT_4_0;
  } else {
    return it->second;
  }
}
