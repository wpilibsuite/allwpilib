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
	
	public static final double RESET_TIME = 3.5;
	
	private final Gyro gyro;
	private final Servo tilt;
	private final Servo pan;
	
	private double lastResetTimeGyro = 0;
	
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
		boolean wasSetup = true;
		pan.set(0.0);
		tilt.set(0.0);
		Timer.delay(RESET_TIME);
		gyro.reset();
		double startTime = Timer.getFPGATimestamp();
		while(Math.abs(gyro.getAngle()) > 0.1){
			Timer.delay(.0001);
		}
		double endTime = Timer.getFPGATimestamp();
		lastResetTimeGyro = endTime - startTime;
		//System.out.println("Gyro reset in " + lastResetTimeGyro + " seconds");
		
		wasSetup = wasSetup && Math.abs(pan.get()) < 0.01;
		wasSetup = wasSetup && Math.abs(tilt.get()) < 0.01;
		wasSetup = wasSetup && Math.abs(gyro.getAngle()) < .2;
		return wasSetup;
	}
	
	/**
	 * When resetting the gyroscope the reset time is stored as a variable that can be retrieved with this method
	 * @return The last time that it took to reset the gyroscope
	 */
	public double getLastResetTimeGyro() {
		return lastResetTimeGyro;
	}
	
	public String getSetupError(){
		StringBuilder error = new StringBuilder("SETUP ERROR: ");
		if(Math.abs(pan.get()) >= 0.01) error.append("Pan " + pan.get() + ", ");
		if(Math.abs(tilt.get()) >= 0.01) error.append("Tilt " + tilt.get() + ", ");
		if(Math.abs(gyro.getAngle()) >= 0.2) error.append("Gyro " + gyro.getAngle() + ", ");
		error.delete(error.length()-2, error.length());
		return error.toString();
	}
	
	@Override
	public boolean reset(){
		gyro.reset();
		return true;
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
		setup();
		tilt.free();
		pan.free();
		gyro.free();
		return true;
	}
	
}
