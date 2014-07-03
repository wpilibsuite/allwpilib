/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

/**
 * @author jonathanleitschuh
 *
 */
public class CANDefaultTest extends AbstractCANTest{
	private static final Logger logger = Logger.getLogger(CANDefaultTest.class.getName());
	@Override
	protected Logger getClassLogger() {
		return logger;
	}

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		getME().getMotor().enableControl();
		getME().getMotor().set(0.0f);
		/* The motor might still have momentum from the previous test. */
		Timer.delay(kStartupTime);
	}
	
	@Test
	public void testDefaultGet(){
		assertEquals("CAN Jaguar did not initilize stopped", 0.0, getME().getMotor().get(), .01f);
	}
	
	@Test
	public void testDefaultBusVoltage(){
		assertEquals("CAN Jaguar did not start at 14 volts", 14.0f, getME().getMotor().getBusVoltage(), 2.0f);
	}
	
	@Test
	public void testDefaultOutputVoltage(){
		assertEquals("CAN Jaguar did not start with an output voltage of 0", 0.0f, getME().getMotor().getOutputVoltage(), 0.3f);

	}
	
	@Test
	public void testDefaultOutputCurrent(){
		assertEquals("CAN Jaguar did not start with an output current of 0", 0.0f, getME().getMotor().getOutputCurrent(), 0.3f);
	}
	
	@Test
	public void testDefaultTemperature(){
		double room_temp = 18.0f;
		assertThat("CAN Jaguar did not start with an initial temperature greater than " + room_temp, getME().getMotor().getTemperature(), is(greaterThan(room_temp)));
	}
	
	@Test
	public void testDefaultForwardLimit(){
		getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
		assertTrue("CAN Jaguar did not start with the Forward Limit Switch Off", getME().getMotor().getForwardLimitOK());
	}
	
	@Test
	public void testDefaultReverseLimit(){
		getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
		assertTrue("CAN Jaguar did not start with the Reverse Limit Switch Off", getME().getMotor().getReverseLimitOK());
	}
	
	@Test
	public void testDefaultNoFaults(){
		assertEquals("CAN Jaguar initialized with Faults", 0, getME().getMotor().getFaults());
	}
	
	
	
	@Test
	public void testFakeLimitSwitchForwards() {
		getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
		getME().getMotor().enableControl();
		assertTrue("CAN Jaguar did not start with the Forward Limit Switch low", getME().getMotor().getForwardLimitOK());
		getME().getForwardLimit().set(true);
		
		delayTillInCorrectStateWithMessage(Level.FINE, kLimitSettlingTime+10, "Forward Limit settling", new BooleanCheck(){@Override
			public boolean getAsBoolean(){
				getME().getMotor().set(0);
				return !getME().getMotor().getForwardLimitOK();
			}
		});
		
		assertFalse("Setting the forward limit switch high did not cause the forward limit switch to trigger after " + kLimitSettlingTime + " seconds", getME().getMotor().getForwardLimitOK());
	}

	
	@Test
	public void testFakeLimitSwitchReverse() {
		getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
		getME().getMotor().enableControl();
		assertTrue("CAN Jaguar did not start with the Reverse Limit Switch low", getME().getMotor().getReverseLimitOK());
		getME().getReverseLimit().set(true);
		
		delayTillInCorrectStateWithMessage(Level.FINE, kLimitSettlingTime+10, "Reverse Limit settling", new BooleanCheck(){@Override
			public boolean getAsBoolean(){
				getME().getMotor().set(0);
				return !getME().getMotor().getReverseLimitOK();
			}
		});
		
		assertFalse("Setting the reverse limit switch high did not cause the reverse limit switch to trigger after " + kLimitSettlingTime + " seconds", getME().getMotor().getReverseLimitOK());
	}	

}
