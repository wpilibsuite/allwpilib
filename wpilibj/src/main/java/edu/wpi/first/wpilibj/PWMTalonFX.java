/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Cross the Road Electronics (CTRE) Talon FX Speed Controller with PWM control.
 *
 * <p>Note that the TalonFX uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric
 * behavior around the deadband or inability to saturate the controller in either direction,
 * calibration is recommended. The calibration procedure can be found in the TalonFX User Manual
 * available from CTRE.
 *
 * <p><ul>
 * <li>2.004ms = full "forward"
 * <li>1.520ms = the "high end" of the deadband range
 * <li>1.500ms = center of the deadband range (off)
 * <li>1.480ms = the "low end" of the deadband range
 * <li>0.997ms = full "reverse"
 * </ul>
 */
public class PWMTalonFX extends PWMSpeedController {
  /**
   * Constructor for a TalonFX connected via PWM.
   *
   * @param channel The PWM channel that the Talon FX is attached to. 0-9 are on-board, 10-19 are
   *                on the MXP port
   */
  public PWMTalonFX(final int channel) {
    super(channel);

    setBounds(2.004, 1.52, 1.50, 1.48, 0.997);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    HAL.report(tResourceType.kResourceType_TalonFX, getChannel() + 1);
    SendableRegistry.setName(this, "PWMTalonFX", getChannel());
  }
}
