/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

import org.junit.After;
import org.junit.Before;
import org.junit.rules.TestWatcher;
import org.junit.runner.Description;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.fixtures.CANMotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Provides a base implementation for CAN Tests that forces a test of a particular mode to provide tests of a certain type.
 * This also allows for a standardized base setup for each test.
 * @author jonathanleitschuh
 *
 */
public abstract class AbstractCANTest extends AbstractComsSetup{
	public static final double kMotorStopTime = 2;
	public static final double kMotorTime = 3;
	public static final double kMotorTimeSettling = 10;
	public static final double kPotentiometerSettlingTime = 10.0;
	public static final double kEncoderSettlingTime = 0.50;
	public static final double kEncoderSpeedTolerance = 30.0;
	public static final double kLimitSettlingTime = 20.0; //timeout in seconds
	public static final double kStartupTime = 0.50;
	public static final double kEncoderPositionTolerance = .75;
	public static final double kPotentiometerPositionTolerance = 10.0/360.0; // +/-10 degrees
	public static final double kCurrentTolerance = 0.1;
	
	
	/** Stores the status value for the previous test. This is set immediately after a failure or success and before the me is torn down. */
	private String status = "";
	/**
	 * Extends the default test watcher in order to provide more information about a tests failure at runtime.
	 * @author jonathanleitschuh
	 *
	 */
	public class CANTestWatcher extends DefaultTestWatcher{
		@Override
		protected void failed(Throwable e, Description description) {
			super.failed(e, description, status);
		}
	}
	@Override
	protected TestWatcher getOverridenTestWatcher(){
		return new CANTestWatcher();
	}
	
	/** The Fixture under test */
	private CANMotorEncoderFixture me;
	
	/**
	 * Retrieves the CANMotorEncoderFixture
	 * @return the CANMotorEncoderFixture for this test.
	 */
	public CANMotorEncoderFixture getME(){
		return me;
	}
	
	/**
	 * This runs BEFORE the setup of the inherited class
	 */
	@Before
	public final void preSetup(){
		status = "";
		me = TestBench.getInstance().getCanJaguarPair();
		me.setup();
		me.getMotor().setSafetyEnabled(false);
	}
	
	@After
	public final void tearDown() throws Exception {
		try{
			//Stores the status data before tearing it down.
			//If the test fails unexpectedly then this could cause an exception.
			status = me.printStatus();
		} finally{
			me.teardown();
		}
		me = null;
	}
	
	protected void setCANJaguar(double seconds, double value){
		for(int i = 0; i < 50; i++) {
			getME().getMotor().set(value);
			Timer.delay(seconds / 50.0);
		}
	}
}