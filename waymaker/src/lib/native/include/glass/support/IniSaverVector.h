// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <vector>

#include <imgui.h>

#include "glass/support/IniSaverBase.h"

namespace glass {

template <typename Info>
class IniSaverVector : public std::vector<Info>, public IniSaverBase {
 public:
  explicit IniSaverVector(std::string_view typeName,
                          IniSaverBackend* backend = nullptr)
      : IniSaverBase(typeName, backend) {}

 private:
  void* IniReadOpen(const char* name) override;
  void IniReadLine(void* entry, const char* lineStr) override;
  void IniWriteAll(ImGuiTextBuffer* out_buf) override;
};

}  // namespace glass

#include "IniSaverVector.inc"
