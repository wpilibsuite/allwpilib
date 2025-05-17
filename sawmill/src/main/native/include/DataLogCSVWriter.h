// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

namespace sawmill {
class DataLogCSVWriter {
 public:
  void ExportCsv(std::string_view outputFolder, int style);
};
}  // namespace sawmill
