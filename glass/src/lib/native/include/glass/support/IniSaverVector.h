/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <imgui.h>
#include <wpi/Twine.h>

#include "glass/support/IniSaverBase.h"

namespace glass {

template <typename Info>
class IniSaverVector : public std::vector<Info>, public IniSaverBase {
 public:
  explicit IniSaverVector(const wpi::Twine& typeName,
                          IniSaverBackend* backend = nullptr)
      : IniSaverBase(typeName, backend) {}

 private:
  void* IniReadOpen(const char* name) override;
  void IniReadLine(void* entry, const char* lineStr) override;
  void IniWriteAll(ImGuiTextBuffer* out_buf) override;
};

}  // namespace glass

#include "IniSaverVector.inl"
