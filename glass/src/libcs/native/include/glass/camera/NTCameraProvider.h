// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>
#include <wpi/DenseMap.h>

#include "glass/camera/CameraProviderBase.h"

namespace glass {

class Storage;

/**
 * Provider for CameraServer models and views for NetworkTables-published
 * cameras.
 */
class NTCameraProvider : public CameraProviderBase {
 public:
  explicit NTCameraProvider(Storage& storage);
  NTCameraProvider(Storage& storage, nt::NetworkTableInstance inst);

  /**
   * Displays menu contents.
   */
  void DisplayMenu() override;

 private:
  struct NTSourceInfo : public SourceInfo {
    NT_Topic descTopic{0};
    NT_Topic connTopic{0};
    NT_Topic streamsTopic{0};
    std::vector<std::string> streams;
  };

  void Update() override;
  void InitCamera(SourceInfo* info) override;

  std::vector<std::unique_ptr<NTSourceInfo>> m_sourceInfo;
  wpi::DenseMap<NT_Topic, NTSourceInfo*> m_topicMap;

  nt::NetworkTableInstance m_inst;
  nt::NetworkTableListenerPoller m_poller;
  NT_Listener m_listener{0};
};

}  // namespace glass
