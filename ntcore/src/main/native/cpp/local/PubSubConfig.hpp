// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "PubSubOptions.h"
#include "ntcore_c.h"

namespace nt::local {

struct PubSubConfig : public PubSubOptionsImpl {
  PubSubConfig() = default;
  PubSubConfig(NT_Type type, std::string_view typeStr,
               const PubSubOptions& options)
      : PubSubOptionsImpl{options}, type{type}, typeStr{typeStr} {
    prefixMatch = false;
  }

  NT_Type type{NT_UNASSIGNED};
  std::string typeStr;
};

}  // namespace nt::local
