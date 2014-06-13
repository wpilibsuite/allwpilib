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

import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class TiltPanCameraTest extends AbstractComsSetup {
	
	private static final Logger logger = Logger.getLogger(TiltPanCameraTest.class.getName());
	
	public static final double TEST_ANGLE = 185.0;
	
	private static TiltPanCameraFixture tpcam;

	
	protected Logger getClassLogger(){
		return logger;
	}
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		logger.fine("Setup: TiltPan camera");
		tpcam = TestBench.getInstance().getTiltPanCam();
		
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		tpcam.teardown();
	}

	@Before
	public void setUp() throws Exception {
		boolean setupSucsess = tpcam.setup();
		assertTrue(tpcam.getSetupError(), setupSucsess);
		double resetTime = tpcam.getLastResetTimeGyro();
		assertEquals("The Gyroscoe reset time was " + resetTime + " seconds", 0, resetTime, .03);
	}

	@After
	public void tearDown() throws Exception {
		tpcam.reset();
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
		for(double i = 0; i < 1.0; i+=.01){
			//System.out.println("i: " + i);
			//System.out.println("Angle " + tpcam.getGyro().getAngle());
			tpcam.getPan().set(i);
			Timer.delay(.05);
		}
		//Timer.delay(TiltPanCameraFixture.RESET_TIME);
		double angle = tpcam.getGyro().getAngle();
		
		double difference = TEST_ANGLE - angle;
		
		double diff = Math.abs(difference);
		
		
		assertEquals(errorMessage(diff, TEST_ANGLE), TEST_ANGLE, angle, 8);
	}
	
	@Test
	public void testDeviationOverTime(){
		double angle = tpcam.getGyro().getAngle();
		assertEquals(errorMessage(angle, 0), 0, angle, .5);
		Timer.delay(5);
		angle = tpcam.getGyro().getAngle();
		assertEquals(errorMessage(angle, 0), 0, angle, .5);
	}
	
	private String errorMessage(double difference, double target){
		return "Gryo angle skewed " + difference + " deg away from target " + target;
	}

}
