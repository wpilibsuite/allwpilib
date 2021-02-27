// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/Solenoid.h"
#include "frc/simulation/CallbackStore.h"
#include "frc/simulation/PCMSim.h"

namespace frc::sim {

/**
 * Class to control a simulated Pneumatic Control Module (PCM).
 */
class SolenoidSim {
 public:
  /**
   * Constructs for a solenoid on the default PCM.
   *
   * @param channel the solenoid channel.
   */
  explicit SolenoidSim(int channel);

  /**
   * Constructs for the given solenoid.
   *
   * @param doubleSolenoid the solenoid to simulate.
   */
  explicit SolenoidSim(Solenoid& solenoid);

  /**
   * Constructs for a solenoid.
   *
   * @param module the CAN ID of the PCM the solenoid is connected to.
   * @param channel the solenoid channel.
   *
   * @see PCMSim#PCMSim(int)
   */
  SolenoidSim(int module, int channel);

  /**
   * Constructs for a solenoid on the given PCM.
   *
   * @param pcm the PCM the solenoid is connected to.
   * @param channel the solenoid channel.
   */
  SolenoidSim(PCMSim& pcm, int channel);

  /**
   * Register a callback to be run when this solenoid is initialized.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if this solenoid has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether this solenoid has been initialized.
   *
   * @param initialized whether the solenoid is intiialized.
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run when the output of this solenoid has changed.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterOutputCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the solenoid output.
   *
   * @return the solenoid output
   */
  bool GetOutput() const;

  /**
   * Change the solenoid output.
   *
   * @param output the new solenoid output
   */
  void SetOutput(bool output);

 private:
  PCMSim m_pcm;
  int m_channel;
};
}  // namespace frc::sim
