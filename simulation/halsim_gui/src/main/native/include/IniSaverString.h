/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>

namespace halsimgui {

template <typename Info>
class IniSaverString {
 public:
  explicit IniSaverString(const char* typeName) : m_typeName(typeName) {}
  void Initialize();

  // pass through useful functions to map
  Info& operator[](wpi::StringRef key) { return m_map[key]; }

  auto begin() { return m_map.begin(); }
  auto end() { return m_map.end(); }
  auto find(wpi::StringRef key) { return m_map.find(key); }

  auto begin() const { return m_map.begin(); }
  auto end() const { return m_map.end(); }
  auto find(wpi::StringRef key) const { return m_map.find(key); }

 private:
  static void* ReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        const char* name);
  static void ReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                       void* entry, const char* lineStr);
  static void WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                       ImGuiTextBuffer* out_buf);

  const char* m_typeName;
  wpi::StringMap<Info> m_map;
};

}  // namespace halsimgui

#include "IniSaverString.inl"
