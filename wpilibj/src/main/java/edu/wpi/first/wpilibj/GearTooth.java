// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Alias for counter class. Implement the gear tooth sensor supplied by FIRST. Currently there is no
 * reverse sensing on the gear tooth sensor, but in future versions we might implement the necessary
 * timing in the FPGA to sense direction.
 *
 * @deprecated The only sensor this works with is no longer available and no teams use it according
 *             to FMS usage reporting.
 */
@Deprecated(since = "2020", forRemoval = true)
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
      HAL.report(tResourceType.kResourceType_GearTooth, channel + 1, 0, "D");
    } else {
      HAL.report(tResourceType.kResourceType_GearTooth, channel + 1, 0);
    }
    SendableRegistry.setName(this, "GearTooth", channel);
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
      HAL.report(tResourceType.kResourceType_GearTooth, source.getChannel() + 1, 0, "D");
    } else {
      HAL.report(tResourceType.kResourceType_GearTooth, source.getChannel() + 1, 0);
    }
    SendableRegistry.setName(this, "GearTooth", source.getChannel());
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
