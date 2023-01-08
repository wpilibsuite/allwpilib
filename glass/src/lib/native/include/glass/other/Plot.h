// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/WindowManager.h"

namespace glass {

class PlotProvider : private WindowManager {
 public:
  explicit PlotProvider(Storage& storage);
  ~PlotProvider() override;

  using WindowManager::GlobalInit;

  /**
   * Pauses or unpauses all plots.
   *
   * @param paused true to pause, false to unpause
   */
  void SetPaused(bool paused) { m_paused = paused; }

  /**
   * Returns true if all plots are paused.
   */
  bool IsPaused() { return m_paused; }

  void DisplayMenu() override;

 private:
  bool m_paused = false;
};

}  // namespace glass
