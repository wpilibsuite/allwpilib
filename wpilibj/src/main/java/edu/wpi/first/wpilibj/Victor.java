/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * VEX Robotics Victor 888 Speed Controller The Vex Robotics Victor 884 Speed Controller can also
 * be used with this class but may need to be calibrated per the Victor 884 user manual.
 *
 * <p>Note that the Victor uses the following bounds for PWM values. These values were determined
 * empirically and optimized for the Victor 888. These values should work reasonably well for
 * Victor 884 controllers also but if users experience issues such as asymmetric behavior around
 * the deadband or inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Victor 884 User Manual available
 * from VEX Robotics: http://content.vexrobotics.com/docs/ifi-v884-users-manual-9-25-06.pdf
 *
 * <p><ul>
 * <li>2.027ms = full "forward"
 * <li>1.525ms = the "high end" of the deadband range
 * <li>1.507ms = center of the deadband range (off)
 * <li>1.490ms = the "low end" of the deadband range
 * <li>1.026ms = full "reverse"
 * </ul>
 */
public class Victor extends PWMSpeedController {
  /**
   * Constructor.
   *
   * @param channel The PWM channel that the Victor is attached to. 0-9 are
   *        on-board, 10-19 are on the MXP port
   */
  public Victor(final int channel) {
    super(channel);

    setBounds(2.027, 1.525, 1.507, 1.49, 1.026);
    setPeriodMultiplier(PeriodMultiplier.k2X);
    setSpeed(0.0);
    setZeroLatch();

    HAL.report(tResourceType.kResourceType_Victor, getChannel() + 1);
    SendableRegistry.setName(this, "Victor", getChannel());
  }
}
