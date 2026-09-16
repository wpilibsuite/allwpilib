// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTTunableTopic.hpp"

#include <string_view>

#include "wpi/nt/ntcore_c.h"
#include "wpi/util/json.hpp"

using namespace wpi::glass;

namespace {

constexpr std::string_view MUTABLE_PROPERTY = "mutable";

}  // namespace

bool wpi::glass::IsTunableTopicMutable(const wpi::nt::Topic& topic) {
  if (!topic.Exists()) {
    return false;
  }

  auto info = topic.GetInfo();
  if (info.type == NT_UNASSIGNED) {
    return false;
  }

  auto properties = info.GetProperties();
  if (auto property = properties.lookup(MUTABLE_PROPERTY)) {
    if (property->is_bool()) {
      return property->get_bool();
    }
  }

  return false;
}
