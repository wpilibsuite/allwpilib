// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include "cameraserver/CameraServer.h"

namespace frc {

WPI_IGNORE_DEPRECATED
template <typename T>
inline cs::AxisCamera CameraServer::AddAxisCamera(
    std::initializer_list<T> hosts) {
  return AddAxisCamera("Axis Camera", hosts);
}

template <typename T>
inline cs::AxisCamera CameraServer::AddAxisCamera(
    std::string_view name, std::initializer_list<T> hosts) {
  std::vector<std::string> vec;
  vec.reserve(hosts.size());
  for (const auto& host : hosts) {
    vec.emplace_back(host);
  }
  return AddAxisCamera(name, vec);
}
WPI_UNIGNORE_DEPRECATED

}  // namespace frc
