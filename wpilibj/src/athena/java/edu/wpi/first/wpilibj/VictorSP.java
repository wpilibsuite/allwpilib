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
 * VEX Robotics Victor SP Speed Controller.
 */
public class VictorSP extends PWMSpeedController {

  /**
   * Common initialization code called by all constructors.
   *
   * <p>Note that the VictorSP uses the following bounds for PWM values. These values should work
   * reasonably well for most controllers, but if users experience issues such as asymmetric
   * behavior around the deadband or inability to saturate the controller in either direction,
   * calibration is recommended. The calibration procedure can be found in the VictorSP User Manual
   * available from CTRE.
   *
   * <p>- 2.004ms = full "forward" - 1.52ms = the "high end" of the deadband range - 1.50ms =
   * center of the deadband range (off) - 1.48ms = the "low end" of the deadband range - .997ms =
   * full "reverse"
   */
  protected void initVictorSP() {
    setBounds(2.004, 1.52, 1.50, 1.48, .997);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setRaw(m_centerPwm);
    setZeroLatch();

    LiveWindow.addActuator("VictorSP", getChannel(), this);
    UsageReporting.report(tResourceType.kResourceType_VictorSP, getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the VictorSP is attached to. 0-9 are on-board, 10-19 are on
   *                the MXP port
   */
  public VictorSP(final int channel) {
    super(channel);
    initVictorSP();
  }
}
