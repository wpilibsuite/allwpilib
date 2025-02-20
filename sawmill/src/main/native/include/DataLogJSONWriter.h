// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <wpi/datalog/DataLogReaderThread.h>
#include <wpi/fs.h>

#include "DataLogExport.h"

namespace sawmill {
/**
 * Helps with loading datalog files.
 */
class DataLogJSONWriter {
 public:
  void ExportJSON(fs::path exportPath,
                  std::vector<sawmill::DataLogRecord> records);
};
}  // namespace sawmill
