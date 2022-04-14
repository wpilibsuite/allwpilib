// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/EnumSetting.h"

#include <imgui.h>

using namespace glass;

EnumSetting::EnumSetting(std::string& str, int defaultValue,
                         std::initializer_list<const char*> choices)
    : m_str{str}, m_choices{choices}, m_value{defaultValue} {
  // override default value if str is one of the choices
  int i = 0;
  for (auto choice : choices) {
    if (str == choice) {
      m_value = i;
      break;
    }
    ++i;
  }
}

void EnumSetting::SetValue(int value) {
  m_value = value;
  m_str = m_choices[m_value];
}

bool EnumSetting::Combo(const char* label, int numOptions,
                        int popup_max_height_in_items) {
  if (ImGui::Combo(
          label, &m_value, m_choices.data(),
          numOptions < 0 ? m_choices.size() : static_cast<size_t>(numOptions),
          popup_max_height_in_items)) {
    m_str = m_choices[m_value];  // update stored string
    return true;
  }
  return false;
}
