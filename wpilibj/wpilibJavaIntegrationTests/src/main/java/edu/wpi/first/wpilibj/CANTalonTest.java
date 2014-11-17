/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertTrue;

import java.util.logging.Logger;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.SampleFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import edu.wpi.first.wpilibj.hal.CanTalonSRX;

/**
 * Basic test (borrowed straight from SampleTest) for running the CAN TalonSRX.
 */
public class CANTalonTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(SampleTest.class.getName());

	static SampleFixture fixture = new SampleFixture();

	protected Logger getClassLogger(){
		return logger;
	}

	@BeforeClass
	public static void classSetup() {
		// Set up the fixture before the test is created
		fixture.setup();
	}

	@Before
	public void setUp() {
		// Reset the fixture elements before every test
		fixture.reset();
	}

	@AfterClass
	public static void tearDown() {
		// Clean up the fixture after the test
		fixture.teardown();
	}

	/**
   * Briefly run a CAN Talon and assert true.
	 */
	@Test
	public void test() {
    // The constructor takes a device ID (settable in roboRIO web interface).
    CanTalonSRX talon = new CanTalonSRX(0);
    // Make sure that the Talon is in the basic throttle mode.
    talon.SetModeSelect(0);
    // Set Talon to 50% forwards throttle.
    talon.Set(0.5);
    Timer.delay(1.5);
    // Turn Talon off.
    talon.Set(0.0);
		assertTrue(true);
	}

}
