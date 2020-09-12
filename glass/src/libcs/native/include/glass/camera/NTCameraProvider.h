// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/Twine.h>

#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/camera/CameraProviderBase.h"

namespace glass {

/**
 * Provider for CameraServer models and views for NetworkTables-published
 * cameras.
 */
class NTCameraProvider : public CameraProviderBase {
 public:
  explicit NTCameraProvider(const wpi::Twine& iniName);
  NTCameraProvider(const wpi::Twine& iniName, NT_Inst inst);

  /**
   * Displays menu contents.
   */
  void DisplayMenu() override;

 private:
  struct NTSourceInfo : public SourceInfo {
    std::vector<std::string> streams;
  };

  void Update() override;
  void InitCamera(SourceInfo* info) override;

  std::vector<NTSourceInfo> m_sourceInfo;

  NetworkTablesHelper m_helper;
};

}  // namespace glass
