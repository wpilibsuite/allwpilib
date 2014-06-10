/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;


@RunWith(Parameterized.class)
public class MotorEncoderTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(MotorEncoderTest.class.getName());
	
	private static final double MOTOR_RUNTIME = .25;
	
	//private static final List<MotorEncoderFixture> pairs = new ArrayList<MotorEncoderFixture>();
	private static MotorEncoderFixture me = null;
	
	public MotorEncoderTest(MotorEncoderFixture mef){
		logger.fine("Constructor with: " + mef.getType());
		if(me != null && !me.equals(mef)) me.teardown();
		me = mef;
	}
	
	@Parameters
	public static Collection<MotorEncoderFixture[]> generateData(){
		//logger.fine("Loading the MotorList");
		return Arrays.asList(new MotorEncoderFixture[][]{
				 {TestBench.getInstance().getTalonPair()},
				 {TestBench.getInstance().getVictorPair()},
				 {TestBench.getInstance().getJaguarPair()}
			//	 TestBench.getInstance().getCanJaguarPair()
		});
	}
	
	/**
	 * Checks to see if this MotorEncoderFixture is fully configured for encoder testing
	 * @param me The motor encoder under test
	 * @return true if this motor encoder has an encoder attached to it
	 */
	boolean shouldRunTest(MotorEncoderFixture me){
		return true;//me.getType().equals(Victor.class.getSimpleName()) || me.getType().equals(Talon.class.getSimpleName());
	}

	@Before
	public void setUp() {
		double initialSpeed = me.getMotor().get();
		assertTrue(me.getType()
				+ " Did not start with an initial speeed of 0 instead got: "
				+ initialSpeed, Math.abs(initialSpeed) < 0.001);
		me.setup();

	}
	
	@After
	public void tearDown() throws Exception {
		me.reset();
	}

	@AfterClass
	public static void tearDownAfterClass() {
		// Clean up the fixture after the test
		me.teardown();
	}
	
	/**
	 * Test to ensure that the isMotorWithinRange method is functioning properly.
	 * Really only needs to run on one MotorEncoderFixture to ensure that it is working correctly.
	 */
	@Test
	public void testIsMotorWithinRange(){
		double range = 0.01;
		assertTrue(me.getType() + " 1", me.isMotorSpeedWithinRange(0.0, range));
		assertTrue(me.getType() + " 2", me.isMotorSpeedWithinRange(0.0, -range));
		assertFalse(me.getType() + " 3", me.isMotorSpeedWithinRange(1.0, range));
		assertFalse(me.getType() + " 4",
				me.isMotorSpeedWithinRange(-1.0, range));
	}

	/**
	 * This test is designed to see if the values of different motors will increment when spun forward
	 */
	@Test
	public void testIncrement() {
		int startValue = me.getEncoder().get();

		me.getMotor().set(.75);
		Timer.delay(MOTOR_RUNTIME);
		int currentValue = me.getEncoder().get();
		if (shouldRunTest(me)) { // TODO REMOVE THIS WHEN ALL ENCODERS ARE
									// PROPERLY ATTACHED
			assertTrue(me.getType() + " Encoder not incremented: start: "
					+ startValue + "; current: " + currentValue,
					startValue < currentValue);
		}
		me.reset();
		encodersResetCheck(me);
	}

	/**
	 * This test is designed to see if the values of different motors will decrement when spun in reverse
	 */
	@Test
	public void testDecrement(){
		int startValue = me.getEncoder().get();

		me.getMotor().set(-.75);
		Timer.delay(MOTOR_RUNTIME);
		int currentValue = me.getEncoder().get();
		if (shouldRunTest(me)) { // TODO REMOVE THIS WHEN ALL ENCODERS ARE
									// PROPERLY ATTACHED
			assertTrue(me.getType() + " Encoder not decremented: start: "
					+ startValue + "; current: " + currentValue,
					startValue > currentValue);
		}
		me.reset();
		encodersResetCheck(me);
	}
	
	/**
	 * This method test if the counters count when the motors rotate
	 */
	@Test
	public void testCouter(){
		int counter1Start = me.getCounters()[0].get();
		int counter2Start = me.getCounters()[1].get();

		me.getMotor().set(.75);
		Timer.delay(MOTOR_RUNTIME);
		int counter1End = me.getCounters()[0].get();
		int counter2End = me.getCounters()[1].get();
		if (shouldRunTest(me)) { // TODO REMOVE THIS WHEN ALL ENCODERS ARE
									// PROPERLY ATTACHED
			assertTrue(me.getType() + " Counter not incremented: start: "
					+ counter1Start + "; current: " + counter1End,
					counter1Start < counter1End);
			assertTrue(me.getType() + " Counter not incremented: start: "
					+ counter1Start + "; current: " + counter2End,
					counter2Start < counter2End);
		}
		me.reset();
		encodersResetCheck(me);
	}
	
	/**
	 * Tests to see if you set the speed to something not <= 1.0 if the code appropriately throttles the value
	 */
	@Test
	public void testSetHighForwardSpeed(){
		me.getMotor().set(15);
		assertTrue(me.getType() + " Motor speed was not close to 1.0, was: "
				+ me.getMotor().get(), me.isMotorSpeedWithinRange(1.0, 0.001));
		me.reset();
	}
	
	/**
	 * Tests to see if you set the speed to something not >= -1.0 if the code appropriately throttles the value
	 */
	@Test
	public void testSetHighReverseSpeed() {
		me.getMotor().set(-15);
		assertTrue(me.getType() + " Motor speed was not close to 1.0, was: "
				+ me.getMotor().get(), me.isMotorSpeedWithinRange(-1.0, 0.001));
		me.reset();
	}
	
	
	@Test
	public void testPIDController() {
		PIDController pid = new PIDController(0.003, 0.001, 0, me.getEncoder(),
				me.getMotor());
		pid.setAbsoluteTolerance(15);
		pid.setOutputRange(-0.2, 0.2);
		pid.setSetpoint(2500);

		pid.enable();
		Timer.delay(5.0);
		pid.disable();

		assertTrue("PID loop did not reach setpoint within 5 seconds. The error was: " + pid.getError(),
				pid.onTarget());

		pid.free();
		me.reset();
	}
	 
	
	
	/**
	 * Checks to see if the encoders and counters are appropriately reset to zero when reset
	 * @param me The MotorEncoderFixture under test
	 */
	private void encodersResetCheck(MotorEncoderFixture me){
		int encoderVal =  me.getEncoder().get();
		int counterVal[] = new int[2];
		for(int i = 0; i < 2; i++){
			counterVal[i] = me.getCounters()[i].get();
		}
		double motorVal = me.getMotor().get();
		assertTrue(me.getType() + " Encoder value: " + encoderVal + " when it should be 0", encoderVal == 0);
		assertTrue(me.getType() + " Motor value: " + motorVal + " when it should be 0", motorVal == 0);
		assertTrue(me.getType() + " Counter value " + counterVal[0] + " when is should be 0", counterVal[0] == 0);
		assertTrue(me.getType() + " Counter value " + counterVal[1] + " when is should be 0", counterVal[1] == 0);
		Timer.delay(.1);
		assertTrue(me.getType() + " Encoder.getStopped() returned false", me.getEncoder().getStopped());
	}

}
