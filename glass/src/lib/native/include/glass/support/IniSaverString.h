/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <utility>

#include <imgui.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "glass/support/IniSaverBase.h"

namespace glass {

template <typename Info>
class IniSaverString : public IniSaverBase {
 public:
  explicit IniSaverString(const wpi::Twine& typeName,
                          IniSaverBackend* backend = nullptr)
      : IniSaverBase(typeName, backend) {}

  // pass through useful functions to map
  Info& operator[](wpi::StringRef key) { return m_map[key]; }

  template <typename... ArgsTy>
  auto try_emplace(wpi::StringRef key, ArgsTy&&... args) {
    return m_map.try_emplace(key, std::forward<ArgsTy>(args)...);
  }

  void erase(typename wpi::StringMap<Info>::iterator it) { m_map.erase(it); }
  auto erase(wpi::StringRef key) { return m_map.erase(key); }

  auto begin() { return m_map.begin(); }
  auto end() { return m_map.end(); }
  auto find(wpi::StringRef key) { return m_map.find(key); }

  auto begin() const { return m_map.begin(); }
  auto end() const { return m_map.end(); }
  auto find(wpi::StringRef key) const { return m_map.find(key); }

  bool empty() const { return m_map.empty(); }

 private:
  void* IniReadOpen(const char* name) override;
  void IniReadLine(void* entry, const char* lineStr) override;
  void IniWriteAll(ImGuiTextBuffer* out_buf) override;

  wpi::StringMap<Info> m_map;
};

}  // namespace glass

#include "IniSaverString.inl"
