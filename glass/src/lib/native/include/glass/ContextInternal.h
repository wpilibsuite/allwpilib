/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  bool ReadIni(wpi::StringRef name_, wpi::StringRef value) {
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
