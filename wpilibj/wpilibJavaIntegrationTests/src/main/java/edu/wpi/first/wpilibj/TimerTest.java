package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertTrue;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;


public class TimerTest extends AbstractComsSetup {
	
	private static double TIMER_TOLARANCE = 0.005;
	private static double TIMER_RUNTIME = 5.0;
	
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
		double startTime = Timer.getFPGATimestamp();
		Timer.delay(TIMER_RUNTIME);
		double endTime =Timer.getFPGATimestamp();
		
		double difference  = endTime - startTime;
		
		double offset = difference - TIMER_RUNTIME;
		
		assertTrue("Timer.delay ran " + offset + " seconds too long", Math.abs(offset) < TIMER_TOLARANCE);
		
	}

}
