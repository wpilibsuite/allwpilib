// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "ntcore_c.h"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace nt {
class Value;
}  // namespace nt

namespace nt::local {

struct LocalTopic;

struct LocalDataLoggerEntry {
  LocalDataLoggerEntry(wpi::log::DataLog& log, int entry, NT_DataLogger logger)
      : log{&log}, entry{entry}, logger{logger} {}

  static std::string MakeMetadata(std::string_view properties);

  void Append(const Value& v);

  wpi::log::DataLog* log;
  int entry;
  NT_DataLogger logger;
};

}  // namespace nt::local
