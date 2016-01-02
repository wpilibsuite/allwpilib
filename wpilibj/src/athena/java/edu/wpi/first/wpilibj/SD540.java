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
 * Mindsensors SD540 Speed Controller
 */
public class SD540 extends SafePWM implements SpeedController {
  private boolean isInverted = false;

  /**
   * Common initialization code called by all constructors.
   *
   * Note that the SD540 uses the following bounds for PWM values. These
   * values should work reasonably well for most controllers, but if users
   * experience issues such as asymmetric behavior around the deadband or
   * inability to saturate the controller in either direction, calibration is
   * recommended. The calibration procedure can be found in the SD540 User
   * Manual available from Mindsensors.
   *
   * - 2.05ms = full "forward" - 1.55ms = the "high end" of the deadband range
   * - 1.50ms = center of the deadband range (off) - 1.44ms = the "low end" of
   * the deadband range - .94ms = full "reverse"
   */
  protected void initSD540() {
    setBounds(2.05, 1.55, 1.50, 1.44, .94);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setRaw(m_centerPwm);
    setZeroLatch();

    LiveWindow.addActuator("SD540", getChannel(), this);
    UsageReporting.report(tResourceType.kResourceType_MindsensorsSD540, getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the SD540 is attached to. 0-9 are
   *        on-board, 10-19 are on the MXP port
   */
  public SD540(final int channel) {
    super(channel);
    initSD540();
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
  public void set(double speed) {
    setSpeed(isInverted ? -speed : speed);
    Feed();
  }

  /**
   * Common interface for inverting direction of a speed controller
   *
   * @param isInverted The state of inversion true is inverted
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
  public double get() {
    return getSpeed();
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  public void pidWrite(double output) {
    set(output);
  }
}
