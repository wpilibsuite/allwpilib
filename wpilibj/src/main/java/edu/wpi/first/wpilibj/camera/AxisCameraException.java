/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.camera;

/**
 * An exception representing a problem with communicating with the camera.
 * @author dtjones
 */
public class AxisCameraException extends Exception {
    /**
     * Create a new AxisCameraException.
     * @param msg The message to pass with the AxisCameraException.
     */
    public AxisCameraException(String msg) {
        super(msg);
    }
}
