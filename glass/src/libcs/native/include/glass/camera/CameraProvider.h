// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>

#include "glass/WindowManager.h"

namespace glass {

class CameraProvider : private WindowManager {
 public:
  explicit CameraProvider(Storage& storage);
  CameraProvider(Storage& storage, nt::NetworkTableInstance inst);
  ~CameraProvider() override;

  using WindowManager::GlobalInit;

  void DisplayMenu() override;

 private:
  struct SourceInfo {
    std::string name;
    std::string description;
    bool connected = false;

    NT_Topic descTopic{0};
    NT_Topic connTopic{0};
    NT_Topic streamsTopic{0};
    std::vector<std::string> streams;
  };

  nt::NetworkTableInstance m_inst;
  nt::NetworkTableListenerPoller m_poller;
  NT_Listener m_listener{0};
};

}  // namespace glass
