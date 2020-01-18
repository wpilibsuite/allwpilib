/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <wpi/DenseMap.h>

namespace halsimgui {

template <typename Info>
class IniSaver {
 public:
  explicit IniSaver(const char* typeName) : m_typeName(typeName) {}
  void Initialize();

  // pass through useful functions to map
  Info& operator[](int index) { return m_map[index]; }

  auto begin() { return m_map.begin(); }
  auto end() { return m_map.end(); }
  auto find(int index) { return m_map.find(index); }

  auto begin() const { return m_map.begin(); }
  auto end() const { return m_map.end(); }
  auto find(int index) const { return m_map.find(index); }

 private:
  static void* ReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        const char* name);
  static void ReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                       void* entry, const char* lineStr);
  static void WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                       ImGuiTextBuffer* out_buf);

  const char* m_typeName;
  wpi::DenseMap<int, Info> m_map;
};

}  // namespace halsimgui

#include "IniSaver.inl"
