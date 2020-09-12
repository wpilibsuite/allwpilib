/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
