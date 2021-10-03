// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>

#include <imgui.h>
#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>
#include <wpi/StringMap.h>

#include "glass/Context.h"
#include "glass/support/IniSaverInfo.h"
#include "glass/support/IniSaverString.h"

namespace glass {

class DataSource;

class DataSourceName {
 public:
  DataSourceName() = default;
  explicit DataSourceName(DataSource* source) : source{source} {}

  bool ReadIni(std::string_view name_, std::string_view value) {
    return name->ReadIni(name_, value);
  }
  void WriteIni(ImGuiTextBuffer* out) { name->WriteIni(out); }

  std::unique_ptr<NameInfo> name{new NameInfo};
  DataSource* source = nullptr;
};

struct Context {
  wpi::SmallString<128> curId;
  wpi::SmallVector<size_t, 32> idStack;
  wpi::StringMap<std::unique_ptr<Storage>> storage;
  wpi::StringMap<bool> deviceHidden;
  IniSaverString<DataSourceName> sources{"Data Sources"};
  uint64_t zeroTime = 0;
};

extern Context* gContext;

}  // namespace glass
