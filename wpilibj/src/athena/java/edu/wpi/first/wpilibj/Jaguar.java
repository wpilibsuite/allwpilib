/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * Texas Instruments / Vex Robotics Jaguar Speed Controller as a PWM device.
 *$
 * @see CANJaguar CANJaguar for CAN control
 */
public class Jaguar extends SafePWM implements SpeedController {
  private boolean isInverted = false;

  /**
   * Common initialization code called by all constructors.
   */
  private void initJaguar() {
    /*
     * Input profile defined by Luminary Micro.
     *$
     * Full reverse ranges from 0.671325ms to 0.6972211ms Proportional reverse
     * ranges from 0.6972211ms to 1.4482078ms Neutral ranges from 1.4482078ms to
     * 1.5517922ms Proportional forward ranges from 1.5517922ms to 2.3027789ms
     * Full forward ranges from 2.3027789ms to 2.328675ms
     */
    setBounds(2.31, 1.55, 1.507, 1.454, .697);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setRaw(m_centerPwm);
    setZeroLatch();

    UsageReporting.report(tResourceType.kResourceType_Jaguar, getChannel());
    LiveWindow.addActuator("Jaguar", getChannel(), this);
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the Jaguar is attached to. 0-9 are
   *        on-board, 10-19 are on the MXP port
   */
  public Jaguar(final int channel) {
    super(channel);
    initJaguar();
  }

  /**
   * Set the PWM value.
   *
   * @deprecated For compatibility with CANJaguar
   *
   *             The PWM value is set using a range of -1.0 to 1.0,
   *             appropriately scaling the value for the FPGA.
   *
   * @param speed The speed to set. Value should be between -1.0 and 1.0.
   * @param syncGroup The update group to add this Set() to, pending
   *        UpdateSyncGroup(). If 0, update immediately.
   */
  @Deprecated
  @Override
  public void set(double speed, byte syncGroup) {
    setSpeed(isInverted ? -speed : speed);
    Feed();
  }

  /**
   * Set the PWM value.
   *
   * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling
   * the value for the FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  @Override
  public void set(double speed) {
    setSpeed(isInverted ? -speed : speed);
    Feed();
  }

  /**
   * Common interface for inverting direction of a speed controller.
   *
   * @param isInverted The state of inversion, true is inverted.
   */
  @Override
  public void setInverted(boolean isInverted) {
    this.isInverted = isInverted;
  }

  /**
   * Common interface for the inverting direction of a speed controller.
   *
   * @return isInverted The state of inversion, true is inverted.
   *
   */
  @Override
  public boolean getInverted() {
    return this.isInverted;
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return getSpeed();
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  @Override
  public void pidWrite(double output) {
    set(output);
  }
}
