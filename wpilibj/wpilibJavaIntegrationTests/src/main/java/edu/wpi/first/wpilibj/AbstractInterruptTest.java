/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.hamcrest.Matchers.both;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertThat;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import org.junit.After;
import org.junit.Test;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

/**
 * This class should not be run as a test explicitly. Instead it should be extended by tests that use the InterruptableSensorBase
 * 
 * @author jonathanleitschuh
 *
 */
public abstract class AbstractInterruptTest extends AbstractComsSetup {
	private InterruptableSensorBase interruptable = null;
	
	private InterruptableSensorBase getInterruptable(){
		if(interruptable == null){
			interruptable = giveInterruptableSensorBase();
		}
		return interruptable;
	}


	@After
	public void interruptTeardown(){
		if(interruptable != null){
			freeInterruptableSensorBase();
			interruptable = null;
		}
	}
	
	/**
	 * Give the interruptible sensor base that interrupts can be attached to.
	 * @return
	 */
	abstract InterruptableSensorBase giveInterruptableSensorBase();
	/**
	 * Cleans up the interruptible sensor base. This is only called if {@link #giveInterruptableSensorBase()} is called.
	 */
	abstract void freeInterruptableSensorBase();
	/**
	 * Perform whatever action is required to set the interrupt high.
	 */
	abstract void setInterruptHigh();
	/**
	 * Perform whatever action is required to set the interrupt low.
	 */
	abstract void setInterruptLow();
	
	
	private class InterruptCounter{
		private final AtomicInteger count  = new AtomicInteger();
		void increment(){
			count.addAndGet(1);
		}
		
		int getCount(){
			return count.get();
		}
	};
	
	private class TestInterruptHandlerFunction extends InterruptHandlerFunction<InterruptCounter>{
		protected final AtomicBoolean exceptionThrown = new AtomicBoolean(false);
		/** Stores the time that the interrupt fires */
		final AtomicLong interruptFireTime = new AtomicLong();
		/** Stores if the interrupt has completed at least once */
		final AtomicBoolean interruptComplete = new AtomicBoolean(false);
		protected Exception ex;
		final InterruptCounter counter;
		
		TestInterruptHandlerFunction(InterruptCounter counter){
			this.counter = counter;
		}
		
		@Override
		void interruptFired(int interruptAssertedMask, InterruptCounter param) {
			interruptFireTime.set(Utility.getFPGATime());
			counter.increment();
			try{
				//This won't cause the test to fail
				assertSame(counter, param);
			} catch (Exception ex){
				//So we must throw the exception within the test
				exceptionThrown.set(true);
				this.ex = ex;
			}
			interruptComplete.set(true);
		};
		
		@Override
		public InterruptCounter overridableParamater(){
			return counter;
		}
	};
	
	@Test(timeout = 1000)
	public void testSingleInterruptsTriggering() throws Exception{
		//Given
		final InterruptCounter counter = new InterruptCounter();
		TestInterruptHandlerFunction function = new TestInterruptHandlerFunction(counter);
		
		//When
		getInterruptable().requestInterrupts(function);
		getInterruptable().enableInterrupts();
		
		setInterruptLow();
		Timer.delay(0.01);
		//Note: Utility.getFPGATime() is used because double values can turn over after the robot has been running for a long time
		final long interruptTriggerTime = Utility.getFPGATime();
		setInterruptHigh();
		
		//Delay until the interrupt is complete
		while(!function.interruptComplete.get()){
			Timer.delay(.005);
		}
		
		
		//Then
		assertEquals("The interrupt did not fire the expected number of times", 1, counter.getCount());
		//If the test within the interrupt failed
		if(function.exceptionThrown.get()){
			throw function.ex;
		}
		final long range = 10000; //in microseconds 
		assertThat("The interrupt did not fire within the expected time period (values in milliseconds)",
		           function.interruptFireTime.get(), both(greaterThan(interruptTriggerTime - range))
		                                             .and(lessThan(interruptTriggerTime + range)));
		assertThat("The readInterruptTimestamp() did not return the correct value (values in seconds)",
		           getInterruptable().readInterruptTimestamp(), both(greaterThan((interruptTriggerTime - range)/1e6))
		                                                    .and(lessThan((interruptTriggerTime + range)/1e6)));
	}
	
	@Test(timeout = 1000)
	public void testMultipleInterruptsTriggering() throws Exception{
		//Given
		final InterruptCounter counter = new InterruptCounter();
		TestInterruptHandlerFunction function = new TestInterruptHandlerFunction(counter);
		
		//When
		getInterruptable().requestInterrupts(function);
		getInterruptable().enableInterrupts();
		
		final int fireCount = 50;
		for(int i = 0; i < fireCount; i ++){
			setInterruptLow();
			setInterruptHigh();
			//Wait for the interrupt to complete before moving on
			while(!function.interruptComplete.getAndSet(false)){
				Timer.delay(.005);
			}
		}
		//Then
		assertEquals("The interrupt did not fire the expected number of times", fireCount, counter.getCount());
	}
	
	/** The timeout length for this test in seconds */
	private static final int synchronousTimeout = 5;
	@Test(timeout = (long)(synchronousTimeout*1e3))
	public void testSynchronousInterruptsTriggering(){
		//Given
		getInterruptable().requestInterrupts();
		
		final double synchronousDelay = synchronousTimeout/2.;
		Runnable r = new Runnable(){
			@Override
			public void run() {
				Timer.delay(synchronousDelay);
				setInterruptLow();
				setInterruptHigh();
			}
		};
		
		//When
		
		//Note: the long time value is used because doubles can flip if the robot is left running for long enough
		final long startTimeStamp = Utility.getFPGATime();
		new Thread(r).start();
		//Delay for twice as long as the timeout so the test should fail first
		getInterruptable().waitForInterrupt(synchronousTimeout * 2);
		final long stopTimeStamp = Utility.getFPGATime();
		
		//Then
		//The test will not have timed out and:
		final double interruptRunTime = (stopTimeStamp - startTimeStamp)*1e-6;
		assertEquals("The interrupt did not run for the expected amount of time (units in seconds)", synchronousDelay, interruptRunTime, .1);
	}
	
	
	@Test(timeout = 2000)
	public void testDisableStopsInterruptFiring(){
		final InterruptCounter counter = new InterruptCounter();
		TestInterruptHandlerFunction function = new TestInterruptHandlerFunction(counter);
		
		//When
		getInterruptable().requestInterrupts(function);
		getInterruptable().enableInterrupts();
		
		final int fireCount = 50;
		for(int i = 0; i < fireCount; i ++){
			setInterruptLow();
			setInterruptHigh();
			//Wait for the interrupt to complete before moving on
			while(!function.interruptComplete.getAndSet(false)){
				Timer.delay(.005);
			}
		}
		getInterruptable().disableInterrupts();
		//TestBench.out().println("Finished disabling the robot");
		
		for(int i = 0; i < fireCount; i ++){
			setInterruptLow();
			setInterruptHigh();
			//Just wait because the interrupt should not fire
			Timer.delay(.005);
		}
		
		//Then
		assertEquals("The interrupt did not fire the expected number of times", fireCount, counter.getCount());
	}
	
}
