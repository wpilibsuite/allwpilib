/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Represents a Dashboard which can provide data to be sent by the DriverStation
 * class.
 * @author pmalmsten
 */
public interface IDashboard {
    /**
     * Gets a reference to the current data to be sent to the dashboard.
     * @return Byte array of data.
     */
    public byte[] getBytes();

    /**
     * Gets the length of the current data to be sent to the
     * dashboard.
     * @return The length of the data array to be sent to the dashboard.
     */
    public int getBytesLength();

    /**
     * If the dashboard had data buffered to be sent, calling this method
     * will reset the output buffer.
     */
    public void flush();
}
