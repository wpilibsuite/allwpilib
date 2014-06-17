/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * @author jonathanleitschuh
 *
 */
public class AnalogCrossConnectTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(AnalogCrossConnectTest.class.getName());
	
	private static AnalogCrossConnectFixture analogIO;
	
	static final double kDelayTime = 0.05;
	
	@Override
	protected Logger getClassLogger() {
		return logger;
	}


	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		analogIO = TestBench.getAnalogCrossConnectFixture();
	}

	/**
	 * @throws java.lang.Exception
	 */
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		analogIO.teardown();
	}

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		analogIO.setup();
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testAnalogOuput() {
		for(int i = 0; i < 50; i++) {
			analogIO.getOutput().setVoltage(i / 10.0f);
	        Timer.delay(kDelayTime);
	        assertEquals(analogIO.getOutput().getVoltage(), analogIO.getInput().getVoltage(), 0.01);
		}
	}
	
	@Test(expected=RuntimeException.class)
	public void testRuntimeExceptionOnInvalidAccumulatorPort(){
		analogIO.getInput().getAccumulatorCount();
	}
}
