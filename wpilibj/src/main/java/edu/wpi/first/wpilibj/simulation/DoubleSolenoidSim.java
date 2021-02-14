// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DoubleSolenoid;

/** Class to control a simulated {@link edu.wpi.first.wpilibj.DoubleSolenoid}. */
public class DoubleSolenoidSim {
  private final PCMSim m_pcm;
  private final int m_fwd;
  private final int m_rev;

  /**
   * Constructs for a double solenoid on the default PCM.
   *
   * @param fwd the forward solenoid channel.
   * @param rev the reverse solenoid channel.
   * @see PCMSim#PCMSim()
   */
  public DoubleSolenoidSim(int fwd, int rev) {
    this.m_pcm = new PCMSim();
    this.m_fwd = fwd;
    this.m_rev = rev;
  }

  /**
   * Constructs for a double solenoid.
   *
   * @param fwd the forward solenoid channel.
   * @param rev the reverse solenoid channel.
   * @see PCMSim#PCMSim(int)
   */
  public DoubleSolenoidSim(int module, int fwd, int rev) {
    this(new PCMSim(module), fwd, rev);
  }

  /**
   * Constructs for a double solenoid on the given PCM.
   *
   * @param pcm the PCM the double solenoid is on.
   * @param fwd the forward solenoid channel.
   * @param rev the reverse solenoid channel.
   */
  public DoubleSolenoidSim(PCMSim pcm, int fwd, int rev) {
    this.m_pcm = pcm;
    this.m_fwd = fwd;
    this.m_rev = rev;
  }

  /**
   * Register a callback to be run when the forward solenoid is initialized.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerFwdInitializedCallback(
      NotifyCallback callback, boolean initialNotify) {
    return m_pcm.registerSolenoidInitializedCallback(m_fwd, callback, initialNotify);
  }

  /**
   * Check if the forward solenoid has been initialized.
   *
   * @return true if initialized
   */
  public boolean getFwdInitialized() {
    return m_pcm.getSolenoidInitialized(m_fwd);
  }

  /**
   * Define whether the forward solenoid has been initialized.
   *
   * @param initialized whether the solenoid is intiialized.
   */
  public void setFwdInitialized(boolean initialized) {
    m_pcm.setSolenoidInitialized(m_fwd, initialized);
  }

  /**
   * Register a callback to be run when the reverse solenoid is initialized.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerRevInitializedCallback(
      NotifyCallback callback, boolean initialNotify) {
    return m_pcm.registerSolenoidInitializedCallback(m_rev, callback, initialNotify);
  }

  /**
   * Check if the reverse solenoid has been initialized.
   *
   * @return true if initialized
   */
  public boolean getRevInitialized() {
    return m_pcm.getSolenoidInitialized(m_rev);
  }

  /**
   * Define whether the reverse solenoid has been initialized.
   *
   * @param initialized whether the solenoid is intiialized.
   */
  public void setRevInitialized(boolean initialized) {
    m_pcm.setSolenoidInitialized(m_rev, initialized);
  }

  /**
   * Set the value of the double solenoid output.
   *
   * @param value The value to set (Off, Forward, Reverse)
   */
  public void set(final DoubleSolenoid.Value value) {
    boolean forward = false;
    boolean reverse = false;

    switch (value) {
      case kOff:
        forward = false;
        reverse = false;
        break;
      case kForward:
        forward = true;
        reverse = false;
        break;
      case kReverse:
        forward = false;
        reverse = true;
        break;
      default:
        throw new AssertionError("Illegal value: " + value);
    }

    m_pcm.setSolenoidOutput(m_fwd, forward);
    m_pcm.setSolenoidOutput(m_rev, reverse);
  }

  /**
   * Check the value of the double solenoid output.
   *
   * @return the output value of the double solenoid.
   */
  public DoubleSolenoid.Value get() {
    boolean fwdState = m_pcm.getSolenoidOutput(m_fwd);
    boolean revState = m_pcm.getSolenoidOutput(m_rev);
    if (!fwdState && !revState) {
      return DoubleSolenoid.Value.kOff;
    } else if (fwdState && !revState) {
      return DoubleSolenoid.Value.kForward;
    } else if (!fwdState && revState) {
      return DoubleSolenoid.Value.kReverse;
    } else {
      throw new AssertionError(
          "In a double solenoid, both fwd and rev can't be on at the same time.");
    }
  }

  /**
   * Get the wrapped {@link PCMSim} object.
   *
   * @return the wrapped {@link PCMSim} object.
   */
  public PCMSim getPCMSim() {
    return m_pcm;
  }
}
