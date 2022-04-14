// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <imgui.h>

namespace glass {

class NameSetting {
 public:
  explicit NameSetting(std::string& str) : m_name{str} {}

  bool HasName() const { return !m_name.empty(); }
  void SetName(std::string_view name) { m_name = name; }
  std::string& GetName() { return m_name; }
  const std::string& GetName() const { return m_name; }
  void GetName(char* buf, size_t size, const char* defaultName) const;
  void GetName(char* buf, size_t size, const char* defaultName,
               int index) const;
  void GetName(char* buf, size_t size, const char* defaultName, int index,
               int index2) const;
  void GetLabel(char* buf, size_t size, const char* defaultName) const;
  void GetLabel(char* buf, size_t size, const char* defaultName,
                int index) const;
  void GetLabel(char* buf, size_t size, const char* defaultName, int index,
                int index2) const;

  void PushEditNameId(int index);
  void PushEditNameId(const char* name);
  bool PopupEditName(int index);
  bool PopupEditName(const char* name);
  bool InputTextName(const char* label_id, ImGuiInputTextFlags flags = 0);

 private:
  std::string& m_name;
};

}  // namespace glass
