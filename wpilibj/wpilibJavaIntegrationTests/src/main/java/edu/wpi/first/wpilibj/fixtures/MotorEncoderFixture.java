/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Represents a physically connected Motor and Encoder to allow for unit tests on these difrent pairs<br>
 * Designed to allow the user to easily setup and tear down the fixture to allow for reuse.
 * This class should be explicitly instantiated in the TestBed class to allow any test to access this fixture.
 * This allows tests to be mailable so that you can easily reconfigure the physical testbed without breaking the tests.
 * 
 * @author Jonathan Leitschuh
 *
 */
public class MotorEncoderFixture implements ITestFixture {
	
	private final SpeedController motor;
	private final Encoder encoder;
	private final Counter counters[] = new Counter[2];
	private final DigitalInput aSource; //Stored so it can be freed at tear down
	private final DigitalInput bSource;
	
	/**
	 * Default constructor for a MotorEncoderFixture
	 * @param motor The SpeedControler for this MotorEncoder pair
	 * @param aSource One of the inputs for the encoder
	 * @param bSource The other input for the encoder
	 */
	public MotorEncoderFixture(SpeedController motor, DigitalInput aSource, DigitalInput bSource){
		this.aSource = aSource;
		this.bSource = bSource;
		
		this.motor = motor;
		
		this.encoder = new Encoder(aSource, bSource);
		counters[0] = new Counter(aSource);
		counters[1] = new Counter(bSource);
		for(Counter c: counters){
			c.start();
		}
		encoder.start();
	}
	
	@Override
	public boolean setup() {
		return reset();
	}
	
	/**
	 * Gets the motor for this Object
	 * @return the motor this object refers too
	 */
	public SpeedController getMotor(){
		return motor;
	}
	
	/**
	 * Gets the encoder for this object
	 * @return the encoder that this object refers too
	 */
	public Encoder getEncoder(){
		return encoder;
	}
	
	public Counter[] getCounters(){
		return counters;
	}
	
	/**
	 * Retrieves the name of the motor that this object refers to
	 * @return The simple name of the motor {@link Class#getSimpleName()}
	 */
	public String getType(){
		return motor.getClass().getSimpleName();
	}
	
	/**
	 * Checks to see if the speed of the motor is within some range of a given value.
	 * This is used instead of equals() because doubles can have inaccuracies.
	 * @param value The value to compare against
	 * @param acuracy The accuracy range to check against to see if the
	 * @return true if the range of values between motors speed ± accuracy contains the 'value'<br>
	 * {@code Math.abs((Math.abs(motor.get()) - Math.abs(value))) < Math.abs(accuracy)}
	 */
	public boolean isMotorSpeedWithinRange(double value, double accuracy){
		return Math.abs((Math.abs(motor.get()) - Math.abs(value))) < Math.abs(accuracy);
	}
	
	@Override
	public boolean reset(){
		boolean wasReset = true;
		
		motor.set(0);
		Timer.delay(TestBench.MOTOR_STOP_TIME); //DEFINED IN THE TestBench
		encoder.reset();
		for(Counter c : counters){
			c.reset();
		}
		
		wasReset &= motor.get() == 0;
		wasReset &= encoder.get() == 0;
		for(Counter c : counters){
			wasReset &= c.get() == 0;
		}
		
		return wasReset;
	}

	

	@Override
	public boolean teardown() {
		reset();
		if(motor instanceof PWM){
			((PWM) motor).free();
		}
		encoder.free();
		for(Counter c : counters){
			c.free();
		}
		
		aSource.free();
		bSource.free();
		
		return true;
	}

}
