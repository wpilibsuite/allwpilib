// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/datalog/DataLogReaderThread.h>

namespace sawmill {
struct Entry {
  explicit Entry(const wpi::log::StartRecordData& srd)
      : name{srd.name}, type{srd.type}, metadata{srd.metadata} {}

  std::string name;
  std::string type;
  std::string metadata;
  // std::set<InputFile*> inputFiles;
  bool typeConflict = false;
  bool metadataConflict = false;
  bool selected = true;

  // used only during export
  int column = -1;
};

struct DataLogRecord {
  const Entry entryData;
  wpi::log::DataLogRecord dataLogRecord;
};

}  // namespace sawmill
