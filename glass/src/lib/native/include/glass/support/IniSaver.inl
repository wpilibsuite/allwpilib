// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/StringRef.h>

namespace glass {

template <typename Info>
void* IniSaver<Info>::IniReadOpen(const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) return nullptr;
  return &m_map[num];
}

template <typename Info>
void IniSaver<Info>::IniReadLine(void* entry, const char* lineStr) {
  auto element = static_cast<Info*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  element->ReadIni(name, value);
}

template <typename Info>
void IniSaver<Info>::IniWriteAll(ImGuiTextBuffer* out_buf) {
  for (auto&& it : m_map) {
    out_buf->appendf("[%s][%d]\n", GetTypeName(), it.first);
    it.second.WriteIni(out_buf);
    out_buf->append("\n");
  }
}

}  // namespace glass
