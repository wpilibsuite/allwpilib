// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <cscore_cpp.h>

#include "glass/camera/CameraProviderBase.h"

namespace glass {

class Storage;

/**
 * Provider for CameraServer models and views that pulls directly from
 * cscore sources.
 */
class CSCameraProvider : public CameraProviderBase {
 public:
  explicit CSCameraProvider(Storage& storage);
  ~CSCameraProvider() override;

  /**
   * Displays menu contents.
   */
  void DisplayMenu() override;

 private:
  void Update() override;

  struct CSSourceInfo : public SourceInfo {
    CS_Source handle;
    CS_SourceKind kind;
  };

  std::vector<CSSourceInfo> m_sourceInfo;

  CS_ListenerPoller m_poller;
};

}  // namespace glass
