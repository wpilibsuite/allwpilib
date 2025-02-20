// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DataLogJSONWriter.h"

#include <vector>

#include <wpi/datalog/DataLogReaderThread.h>
#include <wpi/fs.h>
#include <wpi/json.h>

void ExportJSON(fs::path outputPath,
                std::vector<sawmill::DataLogRecord> records) {
  // JSON structure
  // List of blocks
  // Each block is a direct transcription of a record
  // this includes the entry id, timestamp, and data. If the record contains raw
  // bytes, they will be represented as a base64 string.
}
