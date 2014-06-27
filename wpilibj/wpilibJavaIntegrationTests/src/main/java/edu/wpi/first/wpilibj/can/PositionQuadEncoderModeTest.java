/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

import static org.junit.Assert.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * @author jonathanleitschuh
 *
 */
public class PositionQuadEncoderModeTest extends AbstractCANTest {
	private static final Logger logger = Logger.getLogger(PositionQuadEncoderModeTest.class.getName());
	@Override
	protected Logger getClassLogger() {
		return logger;
	}
	
	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		me = TestBench.getInstance().getCanJaguarPair();
		me.setup();
		me.getMotor().setPositionMode(CANJaguar.kQuadEncoder, 360, 5.0, 0.1, 2.0);
		me.getMotor().enableControl();
		/* The motor might still have momentum from the previous test. */
		Timer.delay(kEncoderSettlingTime);
	}

	@Test
	@Override
	public void testRotateForward() {
		double initial = me.getMotor().getPosition();
		testRotateForward(initial, initial + 50);
	}
	
	@Test
	@Override
	public void testRotateReverse() {
		double initial = me.getMotor().getPosition();
		testRotateReverse(initial, initial - 50);
	}
}
