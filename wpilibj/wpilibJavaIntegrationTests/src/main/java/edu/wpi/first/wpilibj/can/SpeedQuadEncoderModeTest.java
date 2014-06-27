/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.fixtures.CANMotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * @author jonathanleitschuh
 *
 */
public class SpeedQuadEncoderModeTest extends AbstractCANTest implements ICANData {
	private static final Logger logger = Logger.getLogger(PercentQuadEncoderModeTest.class.getName());
	
	@Override
	protected Logger getClassLogger() {
		return logger;
	}
	
	@Before
	public void setUp() throws Exception {
		me = TestBench.getInstance().getCanJaguarPair();
		me.setup();
		me.getMotor().setSpeedMode(CANJaguar.kQuadEncoder, 360, 5.0, 0.1, 2.0);
		me.getMotor().enableControl();
		me.getMotor().set(0.0f);
		/* The motor might still have momentum from the previous test. */
		Timer.delay(kEncoderSettlingTime);
	}
	
	/**
	 * Test if we can drive the motor forward in Speed mode and get a
	 * position back
	 */
	@Test
	public void testRotateForward() {
		//Speed is rev/min
		testRotateForward(0, 1000);
	}
	
	/**
	 * Test if we can drive the motor backwards in Speed mode and get a
	 * position back
	 */
	@Test
	public void testRotateReverse() {
		//Speed is rev/min
		testRotateReverse(0, -1000);
	}
	
	
}
