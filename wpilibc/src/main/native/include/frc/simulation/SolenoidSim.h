// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/PneumaticsModuleType.h"
#include "frc/simulation/PneumaticsBaseSim.h"

namespace frc::sim {

class SolenoidSim {
 public:
  SolenoidSim(std::shared_ptr<PneumaticsBaseSim> moduleSim, int channel);
  SolenoidSim(int module, PneumaticsModuleType type, int channel);
  SolenoidSim(PneumaticsModuleType type, int channel);
  virtual ~SolenoidSim() = default;

  bool GetOutput() const;
  void SetOutput(bool output);

  /**
   * Register a callback to be run when the output of this solenoid has changed.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   * Save a reference to this object; it being deconstructed cancels the
   * callback.
   */
  [[nodiscard]]
  virtual std::unique_ptr<CallbackStore> RegisterOutputCallback(
      NotifyCallback callback, bool initialNotify);

  std::shared_ptr<PneumaticsBaseSim> GetModuleSim() const;

 private:
  std::shared_ptr<PneumaticsBaseSim> m_module;
  int m_channel;
};

}  // namespace frc::sim
