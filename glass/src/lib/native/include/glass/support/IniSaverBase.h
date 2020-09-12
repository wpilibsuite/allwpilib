/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <imgui.h>
#include <wpi/Twine.h>

namespace glass {

class IniSaverBase;

class IniSaverBackend {
 public:
  virtual ~IniSaverBackend() = default;
  virtual void Register(IniSaverBase* iniSaver) = 0;
  virtual void Unregister(IniSaverBase* iniSaver) = 0;
};

class IniSaverBase {
 public:
  explicit IniSaverBase(const wpi::Twine& typeName,
                        IniSaverBackend* backend = nullptr);
  virtual ~IniSaverBase();

  void Initialize() { m_backend->Register(this); }

  const char* GetTypeName() const { return m_typeName.c_str(); }
  IniSaverBackend* GetBackend() const { return m_backend; }

  IniSaverBase(const IniSaverBase&) = delete;
  IniSaverBase& operator=(const IniSaverBase&) = delete;

  virtual void* IniReadOpen(const char* name) = 0;
  virtual void IniReadLine(void* entry, const char* lineStr) = 0;
  virtual void IniWriteAll(ImGuiTextBuffer* out_buf) = 0;

 private:
  std::string m_typeName;
  IniSaverBackend* m_backend;
};

}  // namespace glass
