/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace halsimgui {

template <typename Info>
void IniSaver<Info>::Initialize() {
  // hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = m_typeName;
  iniHandler.TypeHash = ImHashStr(m_typeName);
  iniHandler.ReadOpenFn = ReadOpen;
  iniHandler.ReadLineFn = ReadLine;
  iniHandler.WriteAllFn = WriteAll;
  iniHandler.UserData = this;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
}

template <typename Info>
void* IniSaver<Info>::ReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                               const char* name) {
  auto self = static_cast<IniSaver*>(handler->UserData);
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) return nullptr;
  return &self->m_map[num];
}

template <typename Info>
void IniSaver<Info>::ReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                              void* entry, const char* lineStr) {
  auto element = static_cast<Info*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  element->ReadIni(name, value);
}

template <typename Info>
void IniSaver<Info>::WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                              ImGuiTextBuffer* out_buf) {
  auto self = static_cast<IniSaver*>(handler->UserData);
  for (auto&& it : self->m_map) {
    out_buf->appendf("[%s][%d]\n", self->m_typeName, it.first);
    it.second.WriteIni(out_buf);
    out_buf->append("\n");
  }
}

}  // namespace halsimgui
