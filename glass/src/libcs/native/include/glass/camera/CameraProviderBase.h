// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "glass/WindowManager.h"

namespace glass {

class CameraModel;

/**
 * Base class for provider for CameraServer models and views.
 */
class CameraProviderBase : protected WindowManager {
 public:
  explicit CameraProviderBase(const wpi::Twine& iniName);
  ~CameraProviderBase() override;

  /**
   * Perform global initialization.  This should be called prior to
   * wpi::gui::Initialize().
   */
  void GlobalInit() override;

  CameraModel* GetModel(wpi::StringRef name);

 protected:
  struct SourceInfo {
    std::string name;
    std::string description;
    bool connected = false;
    CameraModel* camera = nullptr;
    Window* window = nullptr;
  };

  virtual void Update() = 0;
  virtual void InitCamera(SourceInfo* info) = 0;

  void MenuItem(const char* label, SourceInfo* info);
  void Show(SourceInfo* info, Window* window);

  void DisplayWindows() override;
  CameraModel* CreateModel(const wpi::Twine& name);

  std::vector<std::unique_ptr<CameraModel>> m_models;

 private:
  void UpdateModels();
};

}  // namespace glass
