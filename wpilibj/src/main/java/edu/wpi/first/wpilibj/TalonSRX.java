/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * Cross the Road Electronics (CTRE) Talon SRX Speed Controller with PWM control.
 */
public class TalonSRX extends PWMSpeedController {

  /**
   * Constructor for a TalonSRX connected via PWM.
   *
   * <p>Note that the TalonSRX uses the following bounds for PWM values. These values should work
   * reasonably well for most controllers, but if users experience issues such as asymmetric
   * behavior around the deadband or inability to saturate the controller in either direction,
   * calibration is recommended. The calibration procedure can be found in the TalonSRX User Manual
   * available from CTRE.
   *
   * <p>- 2.0004ms = full "forward" - 1.52ms = the "high end" of the deadband range - 1.50ms =
   * center
   * of the deadband range (off) - 1.48ms = the "low end" of the deadband range - .997ms = full
   * "reverse"
   *
   * @param channel The PWM channel that the TalonSRX is attached to. 0-9 are on-board, 10-19 are
   *                on the MXP port
   */
  public TalonSRX(final int channel) {
    super(channel);

    setBounds(2.004, 1.52, 1.50, 1.48, .997);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    LiveWindow.addActuator("TalonSRX", getChannel(), this);
    HAL.report(tResourceType.kResourceType_TalonSRX, getChannel());
  }
}
