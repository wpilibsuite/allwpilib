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
 * REV Robotics SPARK Speed Controller.
 */
public class Spark extends PWMSpeedController {

  /**
   * Common initialization code called by all constructors.
   *
   * <p>Note that the SPARK uses the following bounds for PWM values. These values should work
   * reasonably well for most controllers, but if users experience issues such as asymmetric
   * behavior around the deadband or inability to saturate the controller in either direction,
   * calibration is recommended. The calibration procedure can be found in the Spark User Manual
   * available from REV Robotics.
   *
   * <p>- 2.003ms = full "forward" - 1.55ms = the "high end" of the deadband range - 1.50ms =
   * center of the deadband range (off) - 1.46ms = the "low end" of the deadband range - .999ms =
   * full "reverse"
   */
  protected void initSpark() {
    setBounds(2.003, 1.55, 1.50, 1.46, .999);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    LiveWindow.addActuator("Spark", getChannel(), this);
    HAL.report(tResourceType.kResourceType_RevSPARK, getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the SPARK is attached to. 0-9 are on-board, 10-19 are on
   *                the MXP port
   */
  public Spark(final int channel) {
    super(channel);
    initSpark();
  }
}
