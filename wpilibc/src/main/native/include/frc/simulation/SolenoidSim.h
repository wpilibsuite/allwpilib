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
  ~SolenoidSim() = default;

  /**
   * Returns the solenoid output.
   * @return the solenoid output.
   */
  [[deprecated("Use IsOn or IsOff methods instead.")]]
  bool GetOutput() const;

  /**
   * Sets the solenoid output.
   * @param output The new solenoid output.
   */
  void SetOutput(bool output);

  /**
   * Returns true if the solenoid is on.
   *
   * @return true if the solenoid is on.
   *
   */
  bool IsOn();

  /**
   * Returns true if the solenoid is off.
   *
   * @return true if the solenoid is off.
   *
   */
  bool IsOff();

  /**
   * Turns the solenoid on.
   */
  void SetOn();

  /**
   * Turns the solenoid off.
   */
  void SetOff();

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
  std::unique_ptr<CallbackStore> RegisterOutputCallback(NotifyCallback callback,
                                                        bool initialNotify);

  std::shared_ptr<PneumaticsBaseSim> GetModuleSim() const;

 private:
  std::shared_ptr<PneumaticsBaseSim> m_module;
  int m_channel;
};

}  // namespace frc::sim
