// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/StringExtras.h>

#include "glass/support/IniSaver.h"

namespace glass {

template <typename Info>
void* IniSaver<Info>::IniReadOpen(const char* name) {
  if (auto num = wpi::parse_integer<int>(name, 10)) {
    return &m_map[num.value()];
  } else {
    return nullptr;
  }
}

template <typename Info>
void IniSaver<Info>::IniReadLine(void* entry, const char* line) {
  auto element = static_cast<Info*>(entry);
  auto [name, value] = wpi::split(line, '=');
  element->ReadIni(wpi::trim(name), wpi::trim(value));
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
