// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/WindowManager.h"
#include "glass/support/IniSaverBase.h"

namespace glass {

class PlotProvider : private WindowManager {
 public:
  explicit PlotProvider(const wpi::Twine& iniName);
  ~PlotProvider() override;

  void GlobalInit() override;

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
  void DisplayWindows() override;

  class IniSaver : public IniSaverBase {
   public:
    explicit IniSaver(const wpi::Twine& typeName, PlotProvider* provider,
                      bool forSeries);

    void* IniReadOpen(const char* name) override;
    void IniReadLine(void* entry, const char* lineStr) override;
    void IniWriteAll(ImGuiTextBuffer* out_buf) override;

   private:
    PlotProvider* m_provider;
    bool m_forSeries;
  };

  IniSaver m_plotSaver;
  IniSaver m_seriesSaver;
  bool m_paused = false;
};

}  // namespace glass
