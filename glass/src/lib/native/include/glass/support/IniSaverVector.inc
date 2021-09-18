// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/StringExtras.h>

#include "glass/support/IniSaverVector.h"

namespace glass {

template <typename Info>
void* IniSaverVector<Info>::IniReadOpen(const char* name) {
  if (auto num = wpi::parse_integer<unsigned int>(name, 10)) {
    if (num.value() >= this->size()) {
      this->resize(num.value() + 1);
    }
    return &(*this)[num.value()];
  } else {
    return nullptr;
  }
}

template <typename Info>
void IniSaverVector<Info>::IniReadLine(void* entry, const char* line) {
  auto element = static_cast<Info*>(entry);
  auto [name, value] = wpi::split(line, '=');
  element->ReadIni(wpi::trim(name), wpi::trim(value));
}

template <typename Info>
void IniSaverVector<Info>::IniWriteAll(ImGuiTextBuffer* out_buf) {
  for (size_t i = 0; i < this->size(); ++i) {
    out_buf->appendf("[%s][%d]\n", GetTypeName(), static_cast<int>(i));
    (*this)[i].WriteIni(out_buf);
    out_buf->append("\n");
  }
}

}  // namespace glass
