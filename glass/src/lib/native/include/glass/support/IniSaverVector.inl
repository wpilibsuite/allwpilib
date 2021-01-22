// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/StringRef.h>

#include "glass/support/IniSaverVector.h"

namespace glass {

template <typename Info>
void* IniSaverVector<Info>::IniReadOpen(const char* name) {
  unsigned int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) {
    return nullptr;
  }
  if (num >= this->size()) {
    this->resize(num + 1);
  }
  return &(*this)[num];
}

template <typename Info>
void IniSaverVector<Info>::IniReadLine(void* entry, const char* lineStr) {
  auto element = static_cast<Info*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  element->ReadIni(name, value);
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
