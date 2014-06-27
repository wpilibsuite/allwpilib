/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

/**
 * @author jonathanleitschuh
 *
 */
public interface ICANData {
	static final double kPotentiometerSettlingTime = 0.05;
	static final double kMotorTime = 0.5;
	static final double kEncoderSettlingTime = 0.25;
	static final double kEncoderPositionTolerance = 5.0/360.0; // +/-5 degrees
	static final double kPotentiometerPositionTolerance = 10.0/360.0; // +/-10 degrees
}
