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
 * Alias for counter class. Implement the gear tooth sensor supplied by FIRST.
 * Currently there is no reverse sensing on the gear tooth sensor, but in future
 * versions we might implement the necessary timing in the FPGA to sense
 * direction.
 */
public class GearTooth extends Counter {

  private static final double kGearToothThreshold = 55e-6;

  /**
   * Common code called by the constructors.
   */
  public void enableDirectionSensing(boolean directionSensitive) {
    if (directionSensitive) {
      setPulseLengthMode(kGearToothThreshold);
    }
  }

  /**
   * Construct a GearTooth sensor given a channel.
   *
   * No direction sensing is assumed.
   *
   * @param channel The GPIO channel that the sensor is connected to.
   */
  public GearTooth(final int channel) {
    this(channel, false);
  }

  /**
   * Construct a GearTooth sensor given a channel.
   *
   * @param channel The DIO channel that the sensor is connected to. 0-9 are
   *        on-board, 10-25 are on the MXP port
   * @param directionSensitive True to enable the pulse length decoding in
   *        hardware to specify count direction.
   */
  public GearTooth(final int channel, boolean directionSensitive) {
    super(channel);
    enableDirectionSensing(directionSensitive);
    if (directionSensitive) {
      UsageReporting.report(tResourceType.kResourceType_GearTooth, channel, 0, "D");
    } else {
      UsageReporting.report(tResourceType.kResourceType_GearTooth, channel, 0);
    }
    LiveWindow.addSensor("GearTooth", channel, this);
  }

  /**
   * Construct a GearTooth sensor given a digital input. This should be used
   * when sharing digital inputs.
   *
   * @param source An existing DigitalSource object (such as a DigitalInput)
   * @param directionSensitive True to enable the pulse length decoding in
   *        hardware to specify count direction.
   */
  public GearTooth(DigitalSource source, boolean directionSensitive) {
    super(source);
    enableDirectionSensing(directionSensitive);
  }

  /**
   * Construct a GearTooth sensor given a digital input. This should be used
   * when sharing digial inputs.
   *
   * No direction sensing is assumed.
   *
   * @param source An object that fully descibes the input that the sensor is
   *        connected to.
   */
  public GearTooth(DigitalSource source) {
    this(source, false);
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Gear Tooth";
  }
}
