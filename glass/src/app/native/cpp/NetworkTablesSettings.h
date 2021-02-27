// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <ntcore_cpp.h>

namespace wpi {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

class NetworkTablesSettings {
 public:
  explicit NetworkTablesSettings(
      NT_Inst inst = nt::GetDefaultInstance(),
      const char* storageName = "NetworkTables Settings");

  void Update();
  void Display();

 private:
  NT_Inst m_inst;
  bool m_restart = true;
  int* m_pMode;
  std::string* m_pIniName;
  std::string* m_pServerTeam;
  std::string* m_pListenAddress;
};
