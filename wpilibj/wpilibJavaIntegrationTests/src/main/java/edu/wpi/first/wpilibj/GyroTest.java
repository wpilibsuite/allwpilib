/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class GyroTest extends AbstractComsSetup {

	private static final Logger logger = Logger.getLogger(GyroTest.class.getName());

	public static final double TEST_ANGLE = 90.0;

	private TiltPanCameraFixture tpcam;


	@Override
	protected Logger getClassLogger(){
		return logger;
	}

	@Before
	public void setUp() throws Exception {
		logger.fine("Setup: TiltPan camera");
		tpcam = TestBench.getInstance().getTiltPanCam();
		tpcam.setup();
	}

	@After
	public void tearDown() throws Exception {
		tpcam.reset();
		tpcam.teardown();
	}

	@Test
	public void testInitial(){
		double angle = tpcam.getGyro().getAngle();
		assertEquals(errorMessage(angle, 0), 0, angle, .5);
	}

	/**
	 * Test to see if the Servo and the gyroscope is turning 180 degrees
	 */
	@Test
	public void testGyroAngle() {
		//Set angle
		for(int i = 0; i < 5; i++) {
			tpcam.getPan().setAngle(45);
			Timer.delay(.1);
		}
		//Reset for setup
		tpcam.getGyro().reset();
		
		//Prevent drift
		for(int i = 0; i < 10; i++) {
			tpcam.getPan().setAngle(45);
			Timer.delay(.1);
		}
		
		//Perform test
		for(int i = 450; i < 1350; i++) {
			tpcam.getPan().setAngle(i / 10.0);
			Timer.delay(0.005);
		}
		Timer.delay(.2);

		double angle = tpcam.getGyro().getAngle();

		double difference = TEST_ANGLE - angle;

		double diff = Math.abs(difference);

		assertEquals(errorMessage(diff, TEST_ANGLE), TEST_ANGLE, angle, 4);
	}

	@Test
	public void testDeviationOverTime(){
		double angle = tpcam.getGyro().getAngle();
		assertEquals(errorMessage(angle, 0), 0, angle, .5);
		Timer.delay(5);
		angle = tpcam.getGyro().getAngle();
		assertEquals("After 5 seconds " + errorMessage(angle, 0), 0, angle, 1);
	}

	private String errorMessage(double difference, double target){
		return "Gryo angle skewed " + difference + " deg away from target " + target;
	}

}
