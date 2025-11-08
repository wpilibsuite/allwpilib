// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <imgui.h>

namespace glass {

class ColorSetting {
 public:
  explicit ColorSetting(std::vector<float>& color);

  ImVec4 GetColor() const {
    return {m_color[0], m_color[1], m_color[2], m_color[3]};
  }

  float* GetColorFloat() { return m_color.data(); }
  const float* GetColorFloat() const { return m_color.data(); }

  void SetColor(const ImVec4& color) {
    m_color[0] = color.x;
    m_color[1] = color.y;
    m_color[2] = color.z;
    m_color[3] = color.w;
  }

  // updates internal value, returns true on change
  bool ColorEdit3(const char* label, ImGuiColorEditFlags flags = 0) {
    return ImGui::ColorEdit3(label, m_color.data(), flags);
  }

  bool ColorEdit4(const char* label, ImGuiColorEditFlags flags = 0) {
    return ImGui::ColorEdit4(label, m_color.data(), flags);
  }

  bool ColorPicker3(const char* label, ImGuiColorEditFlags flags = 0) {
    return ImGui::ColorPicker3(label, m_color.data(), flags);
  }

  bool ColorPicker4(const char* label, ImGuiColorEditFlags flags = 0,
                    const float* ref_col = nullptr) {
    return ImGui::ColorPicker4(label, m_color.data(), flags, ref_col);
  }

 private:
  std::vector<float>& m_color;
};

}  // namespace glass
