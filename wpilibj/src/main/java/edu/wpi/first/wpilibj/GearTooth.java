/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Alias for counter class. Implement the gear tooth sensor supplied by FIRST. Currently there is no
 * reverse sensing on the gear tooth sensor, but in future versions we might implement the necessary
 * timing in the FPGA to sense direction.
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
   * <p>No direction sensing is assumed.
   *
   * @param channel The GPIO channel that the sensor is connected to.
   */
  public GearTooth(final int channel) {
    this(channel, false);
  }

  /**
   * Construct a GearTooth sensor given a channel.
   *
   * @param channel            The DIO channel that the sensor is connected to. 0-9 are on-board,
   *                           10-25 are on the MXP port
   * @param directionSensitive True to enable the pulse length decoding in hardware to specify count
   *                           direction.
   */
  public GearTooth(final int channel, boolean directionSensitive) {
    super(channel);
    enableDirectionSensing(directionSensitive);
    if (directionSensitive) {
      HAL.report(tResourceType.kResourceType_GearTooth, channel, 0, "D");
    } else {
      HAL.report(tResourceType.kResourceType_GearTooth, channel, 0);
    }
    setName("GearTooth", channel);
  }

  /**
   * Construct a GearTooth sensor given a digital input. This should be used when sharing digital
   * inputs.
   *
   * @param source             An existing DigitalSource object (such as a DigitalInput)
   * @param directionSensitive True to enable the pulse length decoding in hardware to specify count
   *                           direction.
   */
  public GearTooth(DigitalSource source, boolean directionSensitive) {
    super(source);
    enableDirectionSensing(directionSensitive);
    if (directionSensitive) {
      HAL.report(tResourceType.kResourceType_GearTooth, source.getChannel(), 0, "D");
    } else {
      HAL.report(tResourceType.kResourceType_GearTooth, source.getChannel(), 0);
    }
    setName("GearTooth", source.getChannel());
  }

  /**
   * Construct a GearTooth sensor given a digital input. This should be used when sharing digital
   * inputs.
   *
   * <p>No direction sensing is assumed.
   *
   * @param source An object that fully describes the input that the sensor is connected to.
   */
  public GearTooth(DigitalSource source) {
    this(source, false);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);
    builder.setSmartDashboardType("Gear Tooth");
  }
}
