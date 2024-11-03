// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/WindowManager.h"

namespace glass {

class CameraProvider : private WindowManager {
 public:
  explicit CameraProvider(Storage& storage);
  ~CameraProvider() override;

  using WindowManager::GlobalInit;

  void DisplayMenu() override;
};

}  // namespace glass
