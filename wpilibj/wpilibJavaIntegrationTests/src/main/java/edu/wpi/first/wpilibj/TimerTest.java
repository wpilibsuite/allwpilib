/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.logging.Logger;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;


public class TimerTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(TimerTest.class.getName());
	
	private static final long TIMER_TOLARANCE = 5 * 1000;
	private static final long TIMER_RUNTIME = 5 * 1000000;
	
	protected Logger getClassLogger(){
		return logger;
	}
	
	@BeforeClass
	public static void classSetup() {
	}
	
	@Before
	public void setUp() {
		// Reset the fixture elements before every test
		
	}

	@AfterClass
	public static void tearDown() {
		// Clean up the fixture after the test
	}
	
	@Test
	public void delayTest(){
		long startTime = Utility.getFPGATime();
		Timer.delay(TIMER_RUNTIME/1000000);
		long endTime = Utility.getFPGATime();
		
		long difference  = endTime - startTime;
		
		long offset = difference - TIMER_RUNTIME;
		
		assertEquals("Timer.delay ran " + offset + " seconds too long", 0,  Math.abs(offset), TIMER_TOLARANCE);
	}

}
