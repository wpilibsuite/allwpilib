/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRC_NetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.parsing.ISensor;

/**
 * Alias for counter class.
 * Implement the gear tooth sensor supplied by FIRST. Currently there is no reverse sensing on
 * the gear tooth sensor, but in future versions we might implement the necessary timing in the
 * FPGA to sense direction.
 */
public class GearTooth extends Counter implements ISensor {

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
     * The default module is assumed.
     *
     * @param channel The GPIO channel on the digital module that the sensor is connected to.
     * @param directionSensitive Enable the pulse length decoding in hardware to specify count direction.
     */
    public GearTooth(final int channel, boolean directionSensitive) {
        super(channel);
        enableDirectionSensing(directionSensitive);
        if(directionSensitive) {
            UsageReporting.report(tResourceType.kResourceType_GearTooth, channel, DigitalModule.getDefaultDigitalModule()-1, "D");
        } else {
            UsageReporting.report(tResourceType.kResourceType_GearTooth, channel, DigitalModule.getDefaultDigitalModule()-1);
        }
    }

    /**
     * Construct a GearTooth sensor given a channel.
     *
     * The default module is assumed.
     * No direction sensing is assumed.
     *
     * @param channel The GPIO channel on the digital module that the sensor is connected to.
     */
    public GearTooth(final int channel) {
        this(channel,false);
    }

    /**
     * Construct a GearTooth sensor given a channel and module.
     *
     * @param slot The slot in the chassis that the digital module is plugged in to.
     * @param channel The GPIO channel on the digital module that the sensor is connected to.
     * @param directionSensitive Enable the pulse length decoding in hardware to specify count direction.
     */
    public GearTooth(final int slot, final int channel, boolean directionSensitive) {
        super(slot, channel);
        enableDirectionSensing(directionSensitive);
        if(directionSensitive) {
            UsageReporting.report(tResourceType.kResourceType_GearTooth, channel, DigitalModule.getDefaultDigitalModule()-1, "D");
        } else {
            UsageReporting.report(tResourceType.kResourceType_GearTooth, channel, DigitalModule.getDefaultDigitalModule()-1);
        }
        LiveWindow.addSensor("GearTooth", slot, channel, this);
    }

    /**
     * Construct a GearTooth sensor given a channel and module.
     *
     * No direction sensing is assumed.
     *
     * @param slot The slot in the chassis that the digital module is plugged in to.
     * @param channel The GPIO channel on the digital module that the sensor is connected to.
     */
    public GearTooth(final int slot, final int channel) {
        this(slot, channel,false);
    }

    /**
     * Construct a GearTooth sensor given a digital input.
     * This should be used when sharing digial inputs.
     *
     * @param source An object that fully descibes the input that the sensor is connected to.
     * @param directionSensitive Enable the pulse length decoding in hardware to specify count direction.
     */
    public GearTooth(DigitalSource source, boolean directionSensitive) {
        super(source);
        enableDirectionSensing(directionSensitive);
    }

    /**
     * Construct a GearTooth sensor given a digital input.
     * This should be used when sharing digial inputs.
     *
     * No direction sensing is assumed.
     *
     * @param source An object that fully descibes the input that the sensor is connected to.
     */
    public GearTooth(DigitalSource source) {
        this(source,false);
    }

    /*
     * Live Window code, only does anything if live window is activated.
     */
    public String getSmartDashboardType() {
        return "Gear Tooth";
    }
}
