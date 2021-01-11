// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.RelayDataJNI;
import edu.wpi.first.wpilibj.Relay;

/** Class to control a simulated relay. */
public class RelaySim {
  private final int m_index;

  /**
   * Constructs from a Relay object.
   *
   * @param relay Relay to simulate
   */
  public RelaySim(Relay relay) {
    m_index = relay.getChannel();
  }

  /**
   * Constructs from a relay channel number.
   *
   * @param channel Channel number
   */
  public RelaySim(int channel) {
    m_index = channel;
  }

  /**
   * Register a callback to be run when the forward direction is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedForwardCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerInitializedForwardCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelInitializedForwardCallback);
  }

  /**
   * Check whether the forward direction has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitializedForward() {
    return RelayDataJNI.getInitializedForward(m_index);
  }

  /**
   * Define whether the forward direction has been initialized.
   *
   * @param initializedForward whether this object is initialized
   */
  public void setInitializedForward(boolean initializedForward) {
    RelayDataJNI.setInitializedForward(m_index, initializedForward);
  }

  /**
   * Register a callback to be run when the reverse direction is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedReverseCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerInitializedReverseCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelInitializedReverseCallback);
  }

  /**
   * Check whether the reverse direction has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitializedReverse() {
    return RelayDataJNI.getInitializedReverse(m_index);
  }

  /**
   * Define whether the reverse direction has been initialized.
   *
   * @param initializedReverse whether this object is initialized
   */
  public void setInitializedReverse(boolean initializedReverse) {
    RelayDataJNI.setInitializedReverse(m_index, initializedReverse);
  }

  /**
   * Register a callback to be run when the forward direction changes state.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerForwardCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerForwardCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelForwardCallback);
  }

  /**
   * Check whether the forward direction is active.
   *
   * @return true if active
   */
  public boolean getForward() {
    return RelayDataJNI.getForward(m_index);
  }

  /**
   * Set whether the forward direction is active.
   *
   * @param forward true to make active
   */
  public void setForward(boolean forward) {
    RelayDataJNI.setForward(m_index, forward);
  }

  /**
   * Register a callback to be run when the reverse direction changes state.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerReverseCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerReverseCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelReverseCallback);
  }

  /**
   * Check whether the reverse direction is active.
   *
   * @return true if active
   */
  public boolean getReverse() {
    return RelayDataJNI.getReverse(m_index);
  }

  /**
   * Set whether the reverse direction is active.
   *
   * @param reverse true to make active
   */
  public void setReverse(boolean reverse) {
    RelayDataJNI.setReverse(m_index, reverse);
  }

  /** Reset all simulation data. */
  public void resetData() {
    RelayDataJNI.resetData(m_index);
  }
}
