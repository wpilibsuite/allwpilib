/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.List;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class MotorEncoderTest extends AbstractComsSetup {
	
	private static final double MOTOR_RUNTIME = .5;
	
	private static final List<MotorEncoderFixture> pairs = new ArrayList<MotorEncoderFixture>();
	

	@BeforeClass
	public static void classSetup() {
		// Set up the fixture before the test is created
		pairs.add(TestBench.getInstance().getTalonPair());
		pairs.add(TestBench.getInstance().getVictorPair());
		pairs.add(TestBench.getInstance().getJaguarPair());
		pairs.add(TestBench.getInstance().getCanJaguarPair());
		
		for(MotorEncoderFixture me : pairs){
			me.reset();
		}
	}
	
	/**
	 * Checks to see if this MotorEncoderFixture is fully configured for encoder testing
	 * @param me The motor encoder under test
	 * @return true if this motor encoder has an encoder attached to it
	 */
	boolean shouldRunTest(MotorEncoderFixture me){
		return me.getType().equals(Victor.class.getSimpleName()) || me.getType().equals(Talon.class.getSimpleName());
	}

	@Before
	public void setUp() {
		for(MotorEncoderFixture me : pairs){
			double initialSpeed = me.getMotor().get();
			assertTrue(me.getType() + " Did not start with an initial speeed of 0 instead got: " + initialSpeed, Math.abs(initialSpeed) < 0.001);
			me.setup();
		}
		
	}

	@AfterClass
	public static void tearDown() {
		// Clean up the fixture after the test
		for(MotorEncoderFixture me : pairs){
			me.teardown();
		}
		pairs.clear();
	}
	
	/**
	 * Test to ensure that the isMotorWithinRange method is functioning properly.
	 * Really only needs to run on one MotorEncoderFixture to ensure that it is working correctly.
	 */
	@Test
	public void testIsMotorWithinRange(){
		for(MotorEncoderFixture me: pairs){
			double range = 0.01;
			assertTrue(me.getType()  + " 1", me.isMotorSpeedWithinRange(0.0, range));
			assertTrue(me.getType()  + " 2", me.isMotorSpeedWithinRange(0.0, -range));
			assertFalse(me.getType() + " 3", me.isMotorSpeedWithinRange(1.0, range));
			assertFalse(me.getType() + " 4", me.isMotorSpeedWithinRange(-1.0, range));
		}
	}

	/**
	 * This test is designed to see if the values of different motors will increment when spun forward
	 */
	@Test
	public void testIncrement() {
		for(MotorEncoderFixture me: pairs){
			int startValue = me.getEncoder().get();
			
			me.getMotor().set(.75);
			Timer.delay(MOTOR_RUNTIME);
			int currentValue = me.getEncoder().get();
			if(shouldRunTest(me)){ //TODO REMOVE THIS WHEN ALL ENCODERS ARE PROPERLY ATTACHED
				assertTrue(me.getType() + " Encoder not incremented: start: " + startValue + "; current: " + currentValue, startValue < currentValue);
			}
			me.reset();
			encodersResetCheck(me);
		}
	}
	
	/**
	 * This test is designed to see if the values of different motors will decrement when spun in reverse
	 */
	@Test
	public void testDecrement(){
		for(MotorEncoderFixture me: pairs){
			int startValue = me.getEncoder().get();
			
			me.getMotor().set(-.75);
			Timer.delay(MOTOR_RUNTIME);
			int currentValue = me.getEncoder().get();
			if(shouldRunTest(me)){ //TODO REMOVE THIS WHEN ALL ENCODERS ARE PROPERLY ATTACHED
				assertTrue(me.getType() + " Encoder not decremented: start: " + startValue + "; current: " + currentValue, startValue > currentValue);
			}
			me.reset();
			encodersResetCheck(me);
		}
	}
	
	/**
	 * This method test if the counters count when the motors rotate
	 */
	@Test
	public void testCouter(){
		for(MotorEncoderFixture me: pairs){
			int counter1Start = me.getCounters()[0].get();
			int counter2Start = me.getCounters()[1].get();
			
			me.getMotor().set(.75);
			Timer.delay(MOTOR_RUNTIME);
			int counter1End = me.getCounters()[0].get();
			int counter2End = me.getCounters()[1].get();
			if(shouldRunTest(me)){ //TODO REMOVE THIS WHEN ALL ENCODERS ARE PROPERLY ATTACHED
				assertTrue(me.getType() + " Counter not incremented: start: " + counter1Start + "; current: " + counter1End, counter1Start < counter1End);
				assertTrue(me.getType() + " Counter not incremented: start: " + counter1Start + "; current: " + counter2End, counter2Start < counter2End);
			}
			me.reset();
			encodersResetCheck(me);
		}
	}
	
	/**
	 * Tests to see if you set the speed to something not <= 1.0 if the code appropriately throttles the value
	 */
	@Test
	public void testSetHighForwardSpeed(){
		for(MotorEncoderFixture me:pairs){
			me.getMotor().set(15);
			assertTrue(me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get(), me.isMotorSpeedWithinRange(1.0, 0.001));
			me.reset();
		}
	}
	
	/**
	 * Tests to see if you set the speed to something not >= -1.0 if the code appropriately throttles the value
	 */
	@Test
	public void testSetHighReverseSpeed(){
		for(MotorEncoderFixture me:pairs){
			me.getMotor().set(-15);
			assertTrue(me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get(), me.isMotorSpeedWithinRange(-1.0, 0.001));
			me.reset();
		}
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
	}

}
