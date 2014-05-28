/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class TiltPanCameraTest extends AbstractComsSetup {
	
	public static final double TEST_ANGLE = 180.0;
	
	private TiltPanCameraFixture tpcam;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		tpcam = TestBench.getInstance().getTiltPanCam();
		tpcam.reset();
	}

	@After
	public void tearDown() throws Exception {
		tpcam.teardown();
	}

	/**
	 * Test to see if the Servo and the gyroscope is turning 180 degrees
	 */
	@Test
	public void testGyroAngle() {
		for(double i = 0; i < 1.0; i+=.01){
			//System.out.println("i: " + i);
			tpcam.getPan().set(i);
			Timer.delay(.05);
		}
		Timer.delay(TiltPanCameraFixture.RESET_TIME);
		double angle = tpcam.getGyro().getAngle();
		
		double difference = TEST_ANGLE - angle;
		
		double diff = Math.abs(difference);
		
		
		assertTrue("Gryo angle skewed " + difference + " deg away from target " + TEST_ANGLE, diff < 4.0);
		
		
	}

}
