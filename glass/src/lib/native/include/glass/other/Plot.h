/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
