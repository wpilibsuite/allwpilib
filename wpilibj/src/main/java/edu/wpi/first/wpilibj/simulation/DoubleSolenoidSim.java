// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.DoubleSolenoid.Value;
import edu.wpi.first.wpilibj.PneumaticsBase;
import edu.wpi.first.wpilibj.PneumaticsModuleType;

/** Class to control a simulated {@link edu.wpi.first.wpilibj.DoubleSolenoid}. */
public class DoubleSolenoidSim {
  private final PneumaticsBaseSim m_module;
  private final int m_fwd;
  private final int m_rev;

  /**
   * Constructs for a solenoid on the given pneumatics module.
   *
   * @param moduleSim the PCM the solenoid is connected to.
   * @param fwd the forward solenoid channel.
   * @param rev the reverse solenoid channel.
   */
  public DoubleSolenoidSim(PneumaticsBaseSim moduleSim, int fwd, int rev) {
    m_module = moduleSim;
    m_fwd = fwd;
    m_rev = rev;
  }

  /**
   * Constructs for a solenoid on a pneumatics module of the given type and ID.
   *
   * @param module the CAN ID of the pneumatics module the solenoid is connected to.
   * @param moduleType the module type (PH or PCM)
   * @param fwd the forward solenoid channel.
   * @param rev the reverse solenoid channel.
   */
  public DoubleSolenoidSim(int module, PneumaticsModuleType moduleType, int fwd, int rev) {
    this(PneumaticsBaseSim.getForType(module, moduleType), fwd, rev);
  }

  /**
   * Constructs for a solenoid on a pneumatics module of the given type and default ID.
   *
   * @param moduleType the module type (PH or PCM)
   * @param fwd the forward solenoid channel.
   * @param rev the reverse solenoid channel.
   */
  public DoubleSolenoidSim(PneumaticsModuleType moduleType, int fwd, int rev) {
    this(PneumaticsBase.getDefaultForType(moduleType), moduleType, fwd, rev);
  }

  /**
   * Check the value of the double solenoid output.
   *
   * @return the output value of the double solenoid.
   */
  public DoubleSolenoid.Value get() {
    boolean fwdState = m_module.getSolenoidOutput(m_fwd);
    boolean revState = m_module.getSolenoidOutput(m_rev);
    if (fwdState && !revState) {
      return DoubleSolenoid.Value.kForward;
    } else if (!fwdState && revState) {
      return DoubleSolenoid.Value.kReverse;
    } else {
      return DoubleSolenoid.Value.kOff;
    }
  }

  /**
   * Returns true if the double solenoid is in a forward state.
   *
   * @return true if the double solenoid is in a forward state.
   */
  public boolean isForward() {
    return get() == Value.kForward;
  }

  /**
   * Returns true if the double solenoid is in a reverse state.
   *
   * @return true if the double solenoid is in a reverse state.
   */
  public boolean isReverse() {
    return get() == Value.kReverse;
  }

  /**
   * Returns true if the double solenoid is in an off state.
   *
   * @return true if the double solenoid in in an off state.
   */
  public boolean isOff() {
    return get() == Value.kOff;
  }

  /** Sets the double solenoid to a forward state. */
  public void setForward() {
    set(Value.kForward);
  }

  /** Sets the double solenoid to a reverse state. */
  public void setReverse() {
    set(Value.kReverse);
  }

  /** Sets the double solenoid to a off state. */
  public void setOff() {
    set(Value.kOff);
  }

  /**
   * Set the value of the double solenoid output.
   *
   * @param value The value to set (Off, Forward, Reverse)
   */
  public void set(final DoubleSolenoid.Value value) {
    m_module.setSolenoidOutput(m_fwd, value == DoubleSolenoid.Value.kForward);
    m_module.setSolenoidOutput(m_rev, value == DoubleSolenoid.Value.kReverse);
  }

  /**
   * Get the wrapped {@link PneumaticsBaseSim} object.
   *
   * @return the wrapped {@link PneumaticsBaseSim} object.
   */
  public PneumaticsBaseSim getModuleSim() {
    return m_module;
  }
}
