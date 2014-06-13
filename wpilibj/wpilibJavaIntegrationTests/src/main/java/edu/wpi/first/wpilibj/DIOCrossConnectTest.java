/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.Collection;
import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Tests to see if the Digital ports are working properly
 * @author jonathanleitschuh
 */
@RunWith(Parameterized.class)
public class DIOCrossConnectTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(DIOCrossConnectTest.class.getName());

	private static DIOCrossConnectFixture dio = null;
	
	protected Logger getClassLogger(){
		return logger;
	}

	/**
	 * Default constructor for the DIOCrossConnectTest
	 * This test is parameterized in order to allow it to be tested using a variety of different input/output pairs
	 * without duplicate code.<br>
	 * This class takes Integer port values instead of DigitalClasses because it would force them to be instantiated
	 * at the same time which could (untested) cause port binding errors.
	 * 
	 * @param input The port for the input wire
	 * @param output The port for the output wire
	 */
	public DIOCrossConnectTest(Integer input, Integer output) {
		if(dio != null){
			dio.teardown();
		}
		dio = new DIOCrossConnectFixture(input, output);
	}
	
	

	/**
	 * Test data generator. This method is called the the JUnit
	 * parameterized test runner and returns a Collection of Arrays. For each
	 * Array in the Collection, each array element corresponds to a parameter
	 * in the constructor.
	 */
	@Parameters
	public static Collection<Integer[]> generateData() {
		// In this example, the parameter generator returns a List of
		// arrays. Each array has two elements: { Digital input port, Digital output port}.
		// These data are hard-coded into the class, but they could be
		// generated or loaded in any way you like.
		return TestBench.getInstance().getDIOCrossConnectCollection();
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {

	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		dio.teardown();
	}

	@Before
	public void setUp() throws Exception {
		dio.reset();

	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * Tests to see if the DIO can create and recognize a high value
	 */
	@Test
	public void testSetHigh() {
		dio.getOutput().set(true);
		assertTrue("DIO Not High after no delay", dio.getInput().get());
		Timer.delay(.02);
		assertTrue("DIO Not High after .05s delay", dio.getInput().get());
	}
	
	/**
	 * Tests to see if the DIO can create and recognize a low value
	 */
	@Test
	public void testSetLow() {
		dio.getOutput().set(false);
		assertFalse("DIO Not Low after no delay", dio.getInput().get());
		Timer.delay(.02);
		assertFalse("DIO Not Low after .05s delay", dio.getInput().get());
	}
}
