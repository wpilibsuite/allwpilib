// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>

#include "Handle.h"
#include "ntcore_c.h"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace nt::local {

struct LocalTopic;

struct LocalDataLogger {
  static constexpr auto kType = Handle::kDataLogger;

  LocalDataLogger(NT_DataLogger handle, wpi::log::DataLog& log,
                  std::string_view prefix, std::string_view logPrefix)
      : handle{handle}, log{log}, prefix{prefix}, logPrefix{logPrefix} {}

  int Start(std::string_view name, std::string_view typeStr,
            std::string_view metadata, int64_t time);

  NT_DataLogger handle;
  wpi::log::DataLog& log;
  std::string prefix;
  std::string logPrefix;
};

}  // namespace nt::local
