/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/StringRef.h>

namespace glass {

template <typename Info>
void* IniSaverString<Info>::IniReadOpen(const char* name) {
  return &m_map[name];
}

template <typename Info>
void IniSaverString<Info>::IniReadLine(void* entry, const char* lineStr) {
  auto element = static_cast<Info*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  element->ReadIni(name, value);
}

template <typename Info>
void IniSaverString<Info>::IniWriteAll(ImGuiTextBuffer* out_buf) {
  for (auto&& it : m_map) {
    out_buf->appendf("[%s][%s]\n", GetTypeName(), it.getKey().data());
    it.second.WriteIni(out_buf);
    out_buf->append("\n");
  }
}

}  // namespace glass
