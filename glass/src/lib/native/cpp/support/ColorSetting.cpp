// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/ColorSetting.h"

using namespace glass;

ColorSetting::ColorSetting(std::vector<float>& color) : m_color{color} {
  m_color.resize(4);
}
