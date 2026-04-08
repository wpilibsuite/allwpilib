// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>

#include "wpi/datalog/DataLog.hpp"
#include "wpi/nt/ntcore_c.h"

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace wpi::nt {
class Value;
}  // namespace wpi::nt

namespace wpi::nt::local {

struct LocalTopic;

struct LocalDataLoggerEntry {
  LocalDataLoggerEntry(wpi::log::DataLog& log, int entry, NT_DataLogger logger)
      : log{&log}, entry{entry}, logger{logger} {}

  static std::string MakeMetadata(std::string_view properties);

  void Append(const Value& v);
  void Finish(int64_t timestamp) { log->Finish(entry, timestamp); }
  void SetMetadata(std::string_view metadata, int64_t timestamp) {
    log->SetMetadata(entry, metadata, timestamp);
  }

  wpi::log::DataLog* log;
  int entry;
  NT_DataLogger logger;
};

}  // namespace wpi::nt::local
