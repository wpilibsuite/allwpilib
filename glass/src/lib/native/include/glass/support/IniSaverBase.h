// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
