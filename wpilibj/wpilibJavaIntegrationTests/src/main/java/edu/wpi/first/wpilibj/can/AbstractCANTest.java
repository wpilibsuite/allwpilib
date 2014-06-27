/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertThat;

import org.junit.After;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.fixtures.CANMotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

/**
 * @author jonathanleitschuh
 *
 */
public abstract class AbstractCANTest extends AbstractComsSetup implements ICANData{
	protected CANMotorEncoderFixture me;
	/**
	 * Tests that CAN in a certain mode will rotate forwards. The implementation of this method is left up to the extending class because each will require difrent values.
	 * Should call {@link AbstractCANTest#testRotateForward(double, double)}
	 */
	abstract public void testRotateForward();
	
	/**
	 * Tests that CAN in a certain mode will rotate forwards. The implementation of this method is left up to the extending class because each will require difrent values.
	 * Should call {@link AbstractCANTest#testRotateReverse(double, double)}
	 */
	abstract public void testRotateReverse();
	
	
	@After
	public final void tearDown() throws Exception {
		me.teardown();
	}
	
	/**
	 * Tests that a CANMotorEncoderFixture can rotate forward.
	 * Called by extending TestClasses
	 * @param stoppedValue the value where the motor will not be spinning in the current mode
	 * @param runningValue the value where the motor will be spinning in the current mode
	 */
	protected void testRotateForward(double stoppedValue, double runningValue){
		double initialPosition = me.getMotor().getPosition();
	    /* Drive the speed controller briefly to move the encoder */
	    me.getMotor().set(runningValue);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(stoppedValue);

	    /* The position should have increased */
	    assertThat("CAN Jaguar position should have increased after the motor moved", me.getMotor().getPosition(), is(greaterThan(initialPosition)));
	}
	
	
	/**
	 * Tests that a CANMotorEncoderFixture can rotate in reverse.
	 * Called by extending TestClasses
	 * @param stoppedValue the value where the motor will not be spinning in the current mode
	 * @param runningValue the value where the motor will be spinning in the current mode
	 */
	protected void testRotateReverse(double stoppedValue, double runningValue){
		double initialPosition = me.getMotor().getPosition();
	    /* Drive the speed controller briefly to move the encoder */
	    me.getMotor().set(runningValue);
	    Timer.delay(kMotorTime);
	    me.getMotor().set(stoppedValue);

	    /* The position should have decreased */
	    assertThat( "CAN Jaguar position should have decreased after the motor moved", me.getMotor().getPosition(), is(lessThan(initialPosition)));
	}
}
