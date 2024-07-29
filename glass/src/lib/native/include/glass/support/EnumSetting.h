// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/SmallVector.h>

namespace glass {

class EnumSetting {
 public:
  EnumSetting(std::string& str, int defaultValue,
              std::initializer_list<const char*> choices);

  int GetValue() const;
  void SetValue(int value);

  // updates internal value, returns true on change
  bool Combo(const char* label, int numOptions = -1,
             int popup_max_height_in_items = -1);

 private:
  void UpdateValue() const;

  std::string& m_str;
  wpi::SmallVector<const char*, 8> m_choices;
  int m_defaultValue;
  mutable int m_value = -1;
};

}  // namespace glass
