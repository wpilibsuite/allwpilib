/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;
import static org.hamcrest.Matchers.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import edu.wpi.first.wpilibj.command.AbstractCommandTest;
import edu.wpi.first.wpilibj.fixtures.CANMotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * @author jonathanleitschuh
 *
 */
public class CANJaguarTest extends AbstractCommandTest {
	private static final Logger logger = Logger.getLogger(CANJaguarTest.class.getName());
	private CANMotorEncoderFixture me;
	private static final double kPotentiometerSettlingTime = 0.05;
	private static final double kMotorTime = 0.5;
	private static final double kEncoderSettlingTime = 0.25;
	private static final double kEncoderPositionTolerance = 5.0/360.0; // +/-5 degrees
	private static final double kPotentiometerPositionTolerance = 10.0/360.0; // +/-10 degrees
	
	@Override
	protected Logger getClassLogger() {
		return logger;
	}
	
	@Before
	public void setUp() {
		me = TestBench.getInstance().getCanJaguarPair();
		me.setup();
		me.getMotor().setPercentMode(CANJaguar.kEncoder, 360);
	}
	
	@After
	public void tearDown() {
		me.teardown();
	}
	
	
	@Test
	public void testDefaultSpeedGet(){
		assertEquals("CAN Jaguar did not initilize stopped", 0.0, me.getMotor().get(), .01f);
	}
	
	@Test
	public void testDefaultBusVoltage(){
		assertEquals("CAN Jaguar did not start at 14 volts", 14.0f, me.getMotor().getBusVoltage(), 2.0f);
	}
	
	@Test
	public void testDefaultOutputVoltage(){
		assertEquals("CAN Jaguar did not start with an output voltage of 0", 0.0f, me.getMotor().getOutputVoltage(), 0.3f);

	}
	
	@Test
	public void testDefaultOutputCurrent(){
		assertEquals("CAN Jaguar did not start with an output current of 0", 0.0f, me.getMotor().getOutputCurrent(), 0.3f);
	}
	
	@Test
	public void testDefaultTemperature(){
		double room_temp = 18.0f;
		assertThat("CAN Jaguar did not start with an initial temperature greater than " + room_temp, me.getMotor().getTemperature(), is(greaterThan(room_temp)));
	}
	
	@Ignore
	@Test
	public void testDefaultPosition(){
		assertEquals("CAN Jaguar did not start with an initial position of zero", 0.0f, me.getMotor().getPosition(), 0.3f);
	}
	
	@Test
	public void testDefaultSpeed(){
		assertEquals("CAN Jaguar did not start with an initial speed of zero", 0.0f, me.getMotor().getSpeed(), 0.3f);
	}

	//TODO Check that attached FPGA encoder is zeroed after a delay
	//TODO Check that attached FPGA encoder speed is zero after a delay
	
	@Test
	public void testDefaultForwardLimit(){
		assertTrue("CAN Jaguar did not start with the Forward Limit Switch Off", me.getMotor().getForwardLimitOK());
	}
	
	@Test
	public void testDefaultReverseLimit(){
		assertTrue("CAN Jaguar did not start with the Reverse Limit Switch Off", me.getMotor().getReverseLimitOK());
	}
	
	@Test
	public void testDefaultNoFaults(){
		assertEquals(0, me.getMotor().getFaults());
	}
	
	@Test
	public void testPercentForwards() {
	    me.getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
	    me.getMotor().enableControl();
	    me.getMotor().set(0.0f);

	    /* The motor might still have momentum from the previous test. */
	    Timer.delay(kEncoderSettlingTime);

	    double initialPosition = me.getMotor().getPosition();

	    /* Drive the speed controller briefly to move the encoder */
	    me.getMotor().set(1.0f);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(0.0f);

	    /* The position should have increased */
	    assertThat("CAN Jaguar position should have increased after the motor moved", me.getMotor().getPosition(), is(greaterThan(initialPosition)));
	}

	/**
	 * Test if we can drive the motor backwards in percentage mode and get a
	 * position back
	 */
	@Test
	public void testPercentReverse() {
	    me.getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
	    me.getMotor().enableControl();
	    me.getMotor().set(0.0f);

	    /* The motor might still have momentum from the previous test. */
	   Timer.delay(kEncoderSettlingTime);

	    double initialPosition = me.getMotor().getPosition();

	    /* Drive the speed controller briefly to move the encoder */
	    me.getMotor().set(-1.0f);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(0.0f);

	    /* The position should have decreased */
	    assertThat( "CAN Jaguar position should have decreased after the motor moved", me.getMotor().getPosition(), is(lessThan(initialPosition)));
	}

	/**
	 * Test if we can set a position and reach that position with PID control on
	 * the Jaguar.
	 */
	@Test
	public void testEncoderPositionPID() {
	    me.getMotor().setPositionMode(CANJaguar.kQuadEncoder, 360, 5.0, 0.1, 2.0);
	    me.getMotor().enableControl();

	    double setpoint = me.getMotor().getPosition() + 10.0f;

	    /* It should get to the setpoint within 10 seconds */
	    me.getMotor().set(setpoint);
	    Timer.delay(10.0f);

		assertEquals("CAN Jaguar should have reached setpoint with PID control", setpoint, me.getMotor().getPosition(), kEncoderPositionTolerance);
	}

	/**
	 * Test if we can get a position in potentiometer mode, using an analog output
	 * as a fake potentiometer.
	 */
	@Test
	public void testFakePotentiometerPosition() {
	    me.getMotor().setPercentMode(CANJaguar.kPotentiometer);
	    me.getMotor().enableControl();

	    me.getFakePot().setVoltage(0.0f);
	    Timer.delay(kPotentiometerSettlingTime);
	    assertEquals("CAN Jaguar should have returned the potentiometer position set by the analog output", me.getFakePot().getVoltage() / 3.0f, me.getMotor().getPosition(), kPotentiometerPositionTolerance);

	    me.getFakePot().setVoltage(1.0f);
	    Timer.delay(kPotentiometerSettlingTime);
	    assertEquals("CAN Jaguar should have returned the potentiometer position set by the analog output", me.getFakePot().getVoltage() / 3.0f, me.getMotor().getPosition(), kPotentiometerPositionTolerance);

	    me.getFakePot().setVoltage(2.0f);
	    Timer.delay(kPotentiometerSettlingTime);
	    assertEquals("CAN Jaguar should have returned the potentiometer position set by the analog output", me.getFakePot().getVoltage() / 3.0f, me.getMotor().getPosition(), kPotentiometerPositionTolerance);

	    me.getFakePot().setVoltage(3.0f);
	    Timer.delay(kPotentiometerSettlingTime);
	    assertEquals("CAN Jaguar should have returned the potentiometer position set by the analog output", me.getFakePot().getVoltage() / 3.0f, me.getMotor().getPosition(), kPotentiometerPositionTolerance);
	}

	/**
	 * Test if we can limit the Jaguar to only moving in reverse with a fake
	 * limit switch.
	 */
	@Test
	public void testFakeLimitSwitchForwards() {
	    me.getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
	    me.getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
	    me.getForwardLimit().set(true);
	    me.getReverseLimit().set(false);
	    me.getMotor().enableControl();

	    me.getMotor().set(0.0f);
	    Timer.delay(kEncoderSettlingTime);

	    /* Make sure we limits are recognized by the Jaguar. */
	    assertFalse(me.getMotor().getForwardLimitOK());
	    assertTrue(me.getMotor().getReverseLimitOK());

	    double initialPosition = me.getMotor().getPosition();

	    /* Drive the speed controller briefly to move the encoder.  If the limit
	         switch is recognized, it shouldn't actually move. */
	    me.getMotor().set(1.0f);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(0.0f);

	    /* The position should be the same, since the limit switch was on. */
	    assertEquals("CAN Jaguar should not have moved with the limit switch pressed", initialPosition, me.getMotor().getPosition(), kEncoderPositionTolerance);

	    /* Drive the speed controller in the other direction.  It should actually
	         move, since only the forward switch is activated.*/
	    me.getMotor().set(-1.0f);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(0.0f);

	    /* The position should have decreased */
	    assertThat("CAN Jaguar should have moved in reverse while the forward limit was on", me.getMotor().getPosition(), is(lessThan(initialPosition)));
	}

	/**
	 * Test if we can limit the Jaguar to only moving forwards with a fake limit
	 * switch.
	 */
	@Test
	public void testFakeLimitSwitchReverse() {
	    me.getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
	    me.getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
	    me.getForwardLimit().set(false);
	    me.getReverseLimit().set(true);
	    me.getMotor().enableControl();

	    me.getMotor().set(0.0f);
	    Timer.delay(kEncoderSettlingTime);

	    /* Make sure we limits are recognized by the Jaguar. */
	    assertTrue(me.getMotor().getForwardLimitOK());
	    assertFalse(me.getMotor().getReverseLimitOK());

	    double initialPosition = me.getMotor().getPosition();

	    /* Drive the speed controller backwards briefly to move the encoder.  If
	         the limit switch is recognized, it shouldn't actually move. */
	    me.getMotor().set(-1.0f);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(0.0f);

	    /* The position should be the same, since the limit switch was on. */
	    assertEquals("CAN Jaguar should not have moved with the limit switch pressed", initialPosition, me.getMotor().getPosition(), kEncoderPositionTolerance);

	    Timer.delay(kEncoderSettlingTime);

	    /* Drive the speed controller in the other direction.  It should actually
	         move, since only the reverse switch is activated.*/
	    me.getMotor().set(1.0f);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(0.0f);

	    /* The position should have increased */
	    assertThat("CAN Jaguar should have moved forwards while the reverse limit was on", me.getMotor().getPosition(), is(greaterThan(initialPosition)));
	}
}
