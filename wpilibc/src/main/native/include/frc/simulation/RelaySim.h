// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class Relay;

namespace sim {

/**
 * Class to control a simulated relay.
 */
class RelaySim {
 public:
  /**
   * Constructs from a Relay object.
   *
   * @param relay Relay to simulate
   */
  explicit RelaySim(const Relay& relay);

  /**
   * Constructs from a relay channel number.
   *
   * @param channel Channel number
   */
  explicit RelaySim(int channel);

  /**
   * Register a callback to be run when the forward direction is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterInitializedForwardCallback(NotifyCallback callback,
                                     bool initialNotify);

  /**
   * Check whether the forward direction has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitializedForward() const;

  /**
   * Define whether the forward direction has been initialized.
   *
   * @param initializedForward whether this object is initialized
   */
  void SetInitializedForward(bool initializedForward);

  /**
   * Register a callback to be run when the reverse direction is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterInitializedReverseCallback(NotifyCallback callback,
                                     bool initialNotify);

  /**
   * Check whether the reverse direction has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitializedReverse() const;

  /**
   * Define whether the reverse direction has been initialized.
   *
   * @param initializedReverse whether this object is initialized
   */
  void SetInitializedReverse(bool initializedReverse);

  /**
   * Register a callback to be run when the forward direction changes state.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterForwardCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the forward direction is active.
   *
   * @return true if active
   */
  bool GetForward() const;

  /**
   * Set whether the forward direction is active.
   *
   * @param forward true to make active
   */
  void SetForward(bool forward);

  /**
   * Register a callback to be run when the reverse direction changes state.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterReverseCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the reverse direction is active.
   *
   * @return true if active
   */
  bool GetReverse() const;

  /**
   * Set whether the reverse direction is active.
   *
   * @param reverse true to make active
   */
  void SetReverse(bool reverse);

  /**
   * Reset all simulation data.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
