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
 * REV Robotics SPARK MAX Speed Controller with PWM control.
 *
 * <P>Note that the SPARK MAX uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric
 * behavior around the deadband or inability to saturate the controller in either direction,
 * calibration is recommended. The calibration procedure can be found in the SPARK MAX User Manual
 * available from REV Robotics.
 *
 * <p><ul>
 * <li> 2.003ms = full "forward"
 * <li> 1.550ms = the "high end" of the deadband range
 * <li> 1.500ms = center of the deadband range (off)
 * <li> 1.460ms = the "low end" of the deadband range
 * <li> 0.999ms = full "reverse"
 * </ul>
 */
public class PWMSparkMax extends PWMSpeedController {
  /**
   * Common initialization code called by all constructors.
   *
   */
  public PWMSparkMax(final int channel) {
    super(channel);

    setBounds(2.003, 1.55, 1.50, 1.46, 0.999);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    HAL.report(tResourceType.kResourceType_RevSparkMaxPWM, getChannel() + 1);
    SendableRegistry.setName(this, "PWMSparkMax", getChannel());
  }
}
