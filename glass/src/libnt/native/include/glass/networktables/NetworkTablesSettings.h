// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <ntcore_cpp.h>
#include <wpi/SafeThread.h>

#include "glass/support/EnumSetting.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace glass {

class Storage;

class NetworkTablesSettings {
 public:
  explicit NetworkTablesSettings(std::string_view clientName, Storage& storage,
                                 NT_Inst inst = nt::GetDefaultInstance());

  /**
   * Enables or disables the server option.  Default is enabled.
   */
  void EnableServerOption(bool enable) { m_serverOption = enable; }

  void Update();
  bool Display();

 private:
  bool m_restart = true;
  bool m_serverOption = true;
  EnumSetting m_mode;
  std::string& m_persistentFilename;
  std::string& m_serverTeam;
  std::string& m_listenAddress;
  std::string& m_clientName;
  bool& m_dsClient;

  class Thread : public wpi::SafeThread {
   public:
    explicit Thread(NT_Inst inst) : m_inst{inst} {}

    void Main() override;

    NT_Inst m_inst;
    bool m_restart = false;
    int m_mode;
    std::string m_iniName;
    std::string m_serverTeam;
    std::string m_listenAddress;
    std::string m_clientName;
    bool m_dsClient;
  };
  wpi::SafeThreadOwner<Thread> m_thread;
};

}  // namespace glass
