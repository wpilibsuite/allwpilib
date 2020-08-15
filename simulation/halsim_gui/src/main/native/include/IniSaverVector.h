/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

namespace halsimgui {

template <typename Info>
class IniSaverVector : public std::vector<Info> {
 public:
  explicit IniSaverVector(const char* typeName) : m_typeName(typeName) {}
  void Initialize();

 private:
  static void* ReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        const char* name);
  static void ReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                       void* entry, const char* lineStr);
  static void WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                       ImGuiTextBuffer* out_buf);

  const char* m_typeName;
};

}  // namespace halsimgui

#include "IniSaverVector.inl"
