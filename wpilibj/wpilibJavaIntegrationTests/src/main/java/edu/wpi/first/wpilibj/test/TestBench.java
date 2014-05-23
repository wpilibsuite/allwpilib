/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Talon;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.can.CANTimeoutException;
import edu.wpi.first.wpilibj.groups.MotorEncoder;


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
	
	
	public static final double MOTOR_STOP_TIME = 0.10;

	public static TestBench instance;
	
	
	public Talon talon = new Talon(1);
	public DigitalInput encA1 = new DigitalInput(1);
	public DigitalInput encB1 = new DigitalInput(2);
	
	public MotorEncoder talonPair = new MotorEncoder(talon, encA1, encB1);
	
	public Victor vic = new Victor(2);
	public DigitalInput encA2 = new DigitalInput(3);
	public DigitalInput encB2 = new DigitalInput(4);
	public MotorEncoder vicPair = new MotorEncoder(vic, encA2, encB2);
	
	private Jaguar jag = new Jaguar(3);
	public DigitalInput encA3 = new DigitalInput(5);
	public DigitalInput encB3 = new DigitalInput(6);
	private MotorEncoder jagPair = new MotorEncoder(jag, encA3, encB3);
	
	//private CANJaguar canJag = null;
	//private Encoder enc4 = new Encoder(7, 8);
	//private MotorEncoder canPair;
	
	public TestBench(){
//		try {
//			canJag = new CANJaguar(1);
//		} catch (CANTimeoutException e) {
//			e.printStackTrace();
//		}
		//canPair = new MotorEncoder(canJag, enc4);
	}
	
	
	public MotorEncoder getTalonPair(){
		return talonPair;
	}
	
	public MotorEncoder getVictorPair(){
		return vicPair;
	}
	
	
	public MotorEncoder getJaguarPair(){
		return jagPair;
	}
	
//	public MotorEncoder getCanJaguarPair(){
//		return canPair;
//	}
	
	
	
	public static TestBench getInstance() {
		if (instance == null) {
			instance = new TestBench();
		}
		return instance;
	}
}
