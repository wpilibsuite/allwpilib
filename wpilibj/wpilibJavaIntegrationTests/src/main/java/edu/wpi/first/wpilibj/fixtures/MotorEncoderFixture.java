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
public abstract class MotorEncoderFixture implements ITestFixture {
	private boolean initialized = false;
	private boolean tornDown = false;
	protected SpeedController motor;
	private Encoder encoder;
	private Counter counters[] = new Counter[2];
	protected DigitalInput aSource; //Stored so it can be freed at tear down
	protected DigitalInput bSource;

	/**
	 * Default constructor for a MotorEncoderFixture
	 * @param motor The SpeedControler for this MotorEncoder pair
	 * @param aSource One of the inputs for the encoder
	 * @param bSource The other input for the encoder
	 */
	public MotorEncoderFixture(){
	}

	/**
	 * Where the implementer of this class should pass the speed controller
	 * Constructor should only be called from outside this class if the Speed controller
	 * is not also an implementation of PWM interface
	 * @return
	 */
	abstract protected SpeedController giveSpeedController();
	/**
	 * Where the implementer of this class should pass one of the digital inputs<br>
	 * CONSTRUCTOR SHOULD NOT BE CALLED FROM OUTSIDE THIS CLASS!
	 * @return
	 */
	abstract protected DigitalInput giveDigitalInputA();
	/**
	 * Where the implementer fo this class should pass the other digital input<br>
	 * CONSTRUCTOR SHOULD NOT BE CALLED FROM OUTSIDE THIS CLASS!
	 * @return Input B to be used when this class is instantiated
	 */
	abstract protected DigitalInput giveDigitalInputB();

	final private void initialize(){
		synchronized(this){
			if(!initialized){
				aSource = giveDigitalInputA();
				bSource = giveDigitalInputB();
				
				motor = giveSpeedController();
				
				encoder = new Encoder(aSource, bSource);
				counters[0] = new Counter(aSource);
				counters[1] = new Counter(bSource);
				for(Counter c: counters){
					c.start();
				}
				initialized = true;
			}
		}
	}

	@Override
	public boolean setup() {
		initialize();
		encoder.start();
		return true;
	}

	/**
	 * Gets the motor for this Object
	 * @return the motor this object refers too
	 */
	public SpeedController getMotor(){
		initialize();
		return motor;
	}

	/**
	 * Gets the encoder for this object
	 * @return the encoder that this object refers too
	 */
	public Encoder getEncoder(){
		initialize();
		return encoder;
	}

	public Counter[] getCounters(){
		initialize();
		return counters;
	}

	/**
	 * Retrieves the name of the motor that this object refers to
	 * @return The simple name of the motor {@link Class#getSimpleName()}
	 */
	public String getType(){
		initialize();
		return motor.getClass().getSimpleName();
	}

	/**
	 * Checks to see if the speed of the motor is within some range of a given value.
	 * This is used instead of equals() because doubles can have inaccuracies.
	 * @param value The value to compare against
	 * @param acuracy The accuracy range to check against to see if the
	 * @return true if the range of values between motors speed accuracy contains the 'value'.
	 */
	public boolean isMotorSpeedWithinRange(double value, double accuracy){
		initialize();
		return Math.abs((Math.abs(motor.get()) - Math.abs(value))) < Math.abs(accuracy);
	}

	@Override
	public boolean reset(){
		initialize();
		boolean wasReset = true;

		motor.set(0);
		Timer.delay(TestBench.MOTOR_STOP_TIME); //DEFINED IN THE TestBench
		encoder.reset();
		for(Counter c : counters){
			c.reset();
		}

		wasReset = wasReset && motor.get() == 0;
		wasReset = wasReset && encoder.get() == 0;
		for(Counter c : counters){
			wasReset = wasReset && c.get() == 0;
		}

		return wasReset;
	}



	@Override
	public boolean teardown() {
		String type = getType();
		if(!tornDown){
			initialize();
			reset();
			if(motor instanceof PWM){
				((PWM) motor).free();
			}
			motor = null;
			encoder.free();
			counters[0].free();
			counters[0] = null;
			counters[1].free();
			counters[1] = null;

			aSource.free();
			aSource = null;
			bSource.free();
			bSource = null;
			tornDown = true;
		} else {
			throw new RuntimeException(type + " Motor Encoder torn down multiple times");
		}

		return true;
	}

}
