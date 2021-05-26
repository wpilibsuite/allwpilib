// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/StringExtras.h>

#include "glass/support/IniSaverString.h"

namespace glass {

template <typename Info>
void* IniSaverString<Info>::IniReadOpen(const char* name) {
  return &m_map[name];
}

template <typename Info>
void IniSaverString<Info>::IniReadLine(void* entry, const char* line) {
  auto element = static_cast<Info*>(entry);
  auto [name, value] = wpi::split(line, '=');
  element->ReadIni(wpi::trim(name), wpi::trim(value));
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
