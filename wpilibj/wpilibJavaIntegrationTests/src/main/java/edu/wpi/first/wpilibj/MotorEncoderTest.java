package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertTrue;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.groups.MotorEncoder;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class MotorEncoderTest extends AbstractComsSetup {
	
	private static final double MOTOR_RUNTIME = 2.5;
	
	static MotorEncoder pairs [] = new MotorEncoder[3];
	

	@BeforeClass
	public static void classSetup() {
		// Set up the fixture before the test is created
		pairs[0] = TestBench.getInstance().getTalonPair();
		pairs[1] = TestBench.getInstance().getVictorPair();
		pairs[2] = TestBench.getInstance().getJaguarPair();
		//pairs[3] = TestBench.getInstance().getCanJaguarPair();
		
		for(MotorEncoder me : pairs){
			me.reset();
		}
	}

	@Before
	public void setUp() {
		// Reset the fixture elements before every test
		
	}

	@AfterClass
	public static void tearDown() {
		// Clean up the fixture after the test
		for(MotorEncoder me : pairs){
			me.reset();
		}
	}

	/**
	 * This test is designed to see if the values of different motors will increment
	 */
	@Test
	public void testIncrement() {
		int i = 0;
		for(MotorEncoder me: pairs){
			int startValue = me.getEncoder().get();
			
			me.getMotor().set(.75);
			Timer.delay(MOTOR_RUNTIME);
			int currentValue = me.getEncoder().get();
			if(i == 1){ //TODO REMOVE THIS WHEN ALL ENCODERS ARE PROPERLY ATTACHED
				assertTrue("Encoder not incremented: start: " + startValue + "; current: " + currentValue, startValue < currentValue);
			}
			me.reset();
			i++;
		}
	}
	
	@Test
	public void testDecrement(){
		int i = 0;
		for(MotorEncoder me: pairs){
			int startValue = me.getEncoder().get();
			
			me.getMotor().set(-.75);
			Timer.delay(MOTOR_RUNTIME);
			int currentValue = me.getEncoder().get();
			if(i == 1){ //TODO REMOVE THIS WHEN ALL ENCODERS ARE PROPERLY ATTACHED
				assertTrue("Encoder not decremented: start: " + startValue + "; current: " + currentValue, startValue > currentValue);
			}
			me.reset();
			i++;
		}
	}
	
	@Test
	public void testCouter(){
		int i = 0;
		for(MotorEncoder me: pairs){
			int counter1Start = me.getCounters()[0].get();
			int counter2Start = me.getCounters()[1].get();
			
			me.getMotor().set(.75);
			Timer.delay(MOTOR_RUNTIME);
			int counter1End = me.getCounters()[0].get();
			int counter2End = me.getCounters()[1].get();
			if(i == 1){ //TODO REMOVE THIS WHEN ALL ENCODERS ARE PROPERLY ATTACHED
				assertTrue("Counter not incremented: start: " + counter1Start + "; current: " + counter1End, counter1Start < counter1End);
				assertTrue("Counter not incremented: start: " + counter1Start + "; current: " + counter2End, counter2Start < counter2End);
			}
			me.reset();
			i++;
		}
	}

}
