// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/DoubleSolenoid.h"
#include "frc/simulation/CallbackStore.h"
#include "frc/simulation/PCMSim.h"

namespace frc::sim {

/**
 * Class to control a simulated Pneumatic Control Module (PCM).
 */
class DoubleSolenoidSim {
 public:
  /**
   * Constructs for a solenoid on the default PCM.
   *
   * @param channel the solenoid channel.
   */
  DoubleSolenoidSim(int fwd, int rev);

  /**
   * Constructs for a solenoid on the given PCM.
   *
   * @param pcm the PCM the solenoid is connected to.
   * @param channel the solenoid channel.
   */
  DoubleSolenoidSim(int module, int fwd, int rev);

  /**
   * Constructs from a PCMSim object.
   *
   * @param pcm the PCM the solenoid is connected to.
   */
  DoubleSolenoidSim(PCMSim& pcm, int fwd, int rev);

  /**
   * Constructs for the given solenoid.
   *
   * @param solenoid the solenoid to simulate.
   */
  explicit DoubleSolenoidSim(DoubleSolenoid& solenoid);

  /**
   * Register a callback to be run when the forward solenoid is initialized.
   *
   * @param callback      the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterFwdInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the forward solenoid has been initialized.
   *
   * @return true if initialized
   */
  bool GetFwdInitialized() const;

  /**
   * Define whether the forward solenoid has been initialized.
   *
   * @param initialized whether the solenoid is intiialized.
   */
  void SetFwdInitialized(bool initialized);

  /**
   * Register a callback to be run when the reverse solenoid is initialized.
   *
   * @param callback      the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterRevInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Define whether the reverse solenoid has been initialized.
   *
   * @param initialized whether the solenoid is intiialized.
   */
  void SetRevInitialized(bool initialized);

  /**
   * Check if the reverse solenoid has been initialized.
   *
   * @return true if initialized
   */
  bool GetRevInitialized() const;

  /**
   * Set the value of the double solenoid output.
   *
   * @param value The value to set (Off, Forward, Reverse)
   */
  void Set(DoubleSolenoid::Value value);

  /**
   * Check the value of the double solenoid output.
   *
   * @return the output value of the double solenoid.
   */
  DoubleSolenoid::Value Get() const;

 private:
  PCMSim m_pcm;
  int m_fwd;
  int m_rev;
};
}  // namespace frc::sim
