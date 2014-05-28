/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Talon;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.can.CANTimeoutException;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;


/**
 * This class provides access to all of the elements on the test bench, for use
 * in fixtures. This class is a singleton, you should use {@link #getInstance()}
 * to obtain a reference to the {@code TestBench}.
 * 
 * TODO: This needs to be updated to the most recent test bench setup.
 * 
 * @author Fredric Silberberg
 */
public final class TestBench {
	
	
	/** The time that it takes to have a motor go from rotating at full speed to completely stopped */
	public static final double MOTOR_STOP_TIME = 0.50;

	/** The Singleton instance of the Test Bench */
	private static TestBench instance = null;
	
	private CANJaguar canJag = null; //This is declared externally because it does not have a free method
	

	
	/**
	 * The single constructor for the TestBench.
	 * This method is private in order to prevent multiple TestBench objects from being allocated
	 */
	private TestBench(){		
	}
	
	
	/**
	 * Constructs a new set of objects representing a connected set of Talon controlled Motors and an encoder
	 * 
	 * @return a freshly allocated Talon, Encoder pair
	 */
	public MotorEncoderFixture getTalonPair(){
		Talon talon = new Talon(1);
		DigitalInput encA1 = new DigitalInput(1);
		DigitalInput encB1 = new DigitalInput(2);
		
		MotorEncoderFixture talonPair = new MotorEncoderFixture(talon, encA1, encB1);
		return talonPair;
	}
	
	/**
	 * Constructs a new set of objects representing a connected set of Victor controlled Motors and an encoder
	 * 
	 * @return a freshly allocated Victor, Encoder pair
	 */
	public MotorEncoderFixture getVictorPair(){
		Victor vic = new Victor(2);
		DigitalInput encA2 = new DigitalInput(3);
		DigitalInput encB2 = new DigitalInput(4);
		MotorEncoderFixture vicPair = new MotorEncoderFixture(vic, encA2, encB2);
		return vicPair;
	}
	
	
	/**
	 * Constructs a new set of objects representing a connected set of Jaguar controlled Motors and an encoder
	 * 
	 * @return a freshly allocated Jaguar, Encoder pair
	 */
	public MotorEncoderFixture getJaguarPair(){
		Jaguar jag = new Jaguar(3);
		DigitalInput encA3 = new DigitalInput(5);
		DigitalInput encB3 = new DigitalInput(6);
		MotorEncoderFixture jagPair = new MotorEncoderFixture(jag, encA3, encB3);
		return jagPair;
	}
	
	/**
	 * Constructs a new set of objects representing a connected set of CANJaguar controlled Motors and an encoder<br>
	 * Note: The CANJaguar is not freshly allocated because the CANJaguar lacks a free() method
	 * @return an existing CANJaguar and a freshly allocated Encoder
	 */
	public MotorEncoderFixture getCanJaguarPair(){
		
		DigitalInput encA4 = new DigitalInput(7);
		DigitalInput encB4 = new DigitalInput(8);
		MotorEncoderFixture canPair;
		if(canJag == null){		//Again this is because the CanJaguar does not have a free method
			try {
				canJag = new CANJaguar(1);
			} catch (CANTimeoutException e) {
				e.printStackTrace();
			}
		}
		canPair = new MotorEncoderFixture(canJag, encA4, encB4);
		assert canPair != null;
		return canPair;
	}
	
	/**
	 * Constructs a new set of two Servo's and a Gyroscope.
	 * @return a freshly allocated Servo's and a freshly allocated Gyroscope
	 */
	public TiltPanCameraFixture getTiltPanCam(){
		Gyro gyro = new Gyro(1);
		gyro.setSensitivity(.007); //If a different gyroscope is used this value will be different
		
		Servo tilt = new Servo(10);
		Servo pan = new Servo(9);
		
		TiltPanCameraFixture tpcam = new TiltPanCameraFixture(tilt, pan, gyro);
		
		return tpcam;
	}
	
	
	/**
	 * Gets the singleton of the TestBench. If the TestBench is not already allocated in constructs an new instance of it.
	 * Otherwise it returns the existing instance.
	 * @return The Singleton instance of the TestBench
	 */
	public static TestBench getInstance() {
		if (instance == null) {
			instance = new TestBench();
		}
		return instance;
	}
}
