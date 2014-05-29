/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Timer;

/**
 * A class to represent the a physical Camera with two servos (tilt and pan) designed to test to see if the
 * gyroscope is operating normally.
 * 
 * @author Jonathan Leitschuh
 *
 */
public class TiltPanCameraFixture implements ITestFixture {
	
	public static final double RESET_TIME = 3.0;
	
	private final Gyro gyro;
	private final Servo tilt;
	private final Servo pan;
	
	/**
	 * Constructs the TiltPanCamera
	 * @param tilt
	 * 		The servo to tilt the camera
	 * @param pan
	 * 		The servo to pan the camera
	 * @param gyro
	 */
	public TiltPanCameraFixture (Servo tilt, Servo pan, Gyro gyro){
		this.tilt = tilt;
		this.pan = pan;
		this.gyro = gyro;
		
	}
	
	@Override
	public boolean setup() {
		return reset();
	}
	
	@Override
	public boolean reset(){
		boolean wasReset = true;
		pan.setAngle(0);
		tilt.set(0);
		Timer.delay(RESET_TIME);
		gyro.reset();
		
		wasReset &= pan.get() == 0;
		wasReset &= tilt.get() == 0;
		wasReset &= gyro.getAngle() == 0;
		return wasReset;
	}
	
	public Servo getTilt() {
		return tilt;
	}
	
	public Servo getPan() {
		return pan;
	}
	
	public Gyro getGyro() {
		return gyro;
	}

	@Override
	public boolean teardown() {
		reset();
		
		tilt.free();
		pan.free();
		gyro.free();
		return true;
	}
	
}
