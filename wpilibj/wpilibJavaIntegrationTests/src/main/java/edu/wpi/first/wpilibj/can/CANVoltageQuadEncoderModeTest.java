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
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

/**
 * @author jonathanleitschuh
 *
 */
public class CANVoltageQuadEncoderModeTest extends AbstractCANTest {
	private static final Logger logger = Logger.getLogger(CANVoltageQuadEncoderModeTest.class.getName());
	/** The stopped value in volts */
	private static final double kStoppedValue = 0;
	/** The running value in volts */
	private static final double kRunningValue = 14;
	
	private static final double kVoltageTolerance = .25;
	
	/* (non-Javadoc)
	 * @see edu.wpi.first.wpilibj.can.AbstractCANTest#stopMotor()
	 */
	protected void stopMotor() {
		getME().getMotor().set(kStoppedValue);
	}

	/* (non-Javadoc)
	 * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorForward()
	 */
	protected void runMotorForward() {
		getME().getMotor().set(kRunningValue);
	}

	/* (non-Javadoc)
	 * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorReverse()
	 */
	protected void runMotorReverse() {
		getME().getMotor().set(-kRunningValue);
	}
	
	@Override
	protected Logger getClassLogger() {
		return logger;
	}

	@Before
	public void setUp() throws Exception {
		getME().getMotor().setVoltageMode(CANJaguar.kQuadEncoder, 360);
		getME().getMotor().set(kStoppedValue);
		getME().getMotor().enableControl();
		
		/* The motor might still have momentum from the previous test. */
		Timer.delay(kStartupTime);
	}

	@Test
	public void testRotateForwardToVoltage() {
		setCANJaguar(kMotorTime, Math.PI);
		assertEquals("The output voltage did not match the desired voltage set-point", Math.PI, getME().getMotor().getOutputVoltage(), kVoltageTolerance);
	}

	@Test
	public void testRotateReverseToVoltage() {
		setCANJaguar(kMotorTime, -Math.PI);
		assertEquals("The output voltage did not match the desired voltage set-point", -Math.PI, getME().getMotor().getOutputVoltage(), kVoltageTolerance);
	}
	
	
	@Test
	public void testMaxOutputVoltagePositive(){
		//given
		double maxVoltage = 5;
		getME().getMotor().enableControl();
		runMotorForward(); //Sets the output to be #kRunningValue
		runMotorForward();
		setCANJaguar(1, kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "Voltage settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorForward();
				return Math.abs((Math.abs(getME().getMotor().getOutputVoltage()) - kRunningValue)) < 1;
			}
		});
		assertEquals(kRunningValue, getME().getMotor().get(), 0.00000001);
		final double fastSpeed = getME().getMotor().getSpeed();
		
		//when
		getME().getMotor().configMaxOutputVoltage(maxVoltage);
		
		setCANJaguar(1, kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "SpeedReducing settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorForward();
				return fastSpeed > getME().getMotor().getSpeed();
			}
		});
		//then
		assertThat("Speed did not reduce when the max output voltage was set", fastSpeed, is(greaterThan(getME().getMotor().getSpeed())));
	}
	
	@Test
	public void testMaxOutputVoltagePositiveSetToZeroStopsMotor(){
		//given
		double maxVoltage = 0;
		getME().getMotor().enableControl();
		runMotorForward(); //Sets the output to be #kRunningValue
		runMotorForward();
		setCANJaguar(1, kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "Voltage settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorForward();
				return Math.abs((Math.abs(getME().getMotor().getOutputVoltage()) - kRunningValue)) < 1;
			}
		});
		assertEquals(kRunningValue, getME().getMotor().get(), 0.00000001);
		final double fastSpeed = getME().getMotor().getSpeed();
		
		//when
		getME().getMotor().configMaxOutputVoltage(maxVoltage);
		
		setCANJaguar(1, kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "SpeedReducing settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorForward();
				return getME().getMotor().getSpeed() == 0;
			}
		});
		//then
		assertEquals("Speed did not go to zero when the max output voltage was set to " + maxVoltage, 0, getME().getMotor().getSpeed(), kEncoderSpeedTolerance);
	}
	
	
	@Test
	public void testMaxOutputVoltageNegative(){
		//given
		double maxVoltage = 5;
		getME().getMotor().enableControl();
		runMotorReverse(); //Sets the output to be #kRunningValue
		
		setCANJaguar(1, -kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "Voltage settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorReverse();
				return Math.abs((Math.abs(getME().getMotor().getOutputVoltage()) - kRunningValue)) < 1;
			}
		});
		assertEquals(-kRunningValue, getME().getMotor().get(), 0.00000001);
		final double fastSpeed = getME().getMotor().getSpeed();
		
		//when
		getME().getMotor().configMaxOutputVoltage(maxVoltage);
		
		
		setCANJaguar(1, -kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "SpeedReducing settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorReverse();
				return fastSpeed < getME().getMotor().getSpeed();
			}
		});
		//then
		assertThat("Speed did not reduce when the max output voltage was set", fastSpeed, is(lessThan(getME().getMotor().getSpeed())));
	}
	
	@Test
	public void testMaxOutputVoltageNegativeSetToZeroStopsMotor(){
		//given
		double maxVoltage = 0;
		getME().getMotor().enableControl();
		runMotorForward(); //Sets the output to be #kRunningValue
		runMotorForward();
		setCANJaguar(1, kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "Voltage settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorForward();
				return Math.abs((Math.abs(getME().getMotor().getOutputVoltage()) - kRunningValue)) < 1;
			}
		});
		assertEquals(kRunningValue, getME().getMotor().get(), 0.00000001);
		final double fastSpeed = getME().getMotor().getSpeed();
		
		//when
		getME().getMotor().configMaxOutputVoltage(maxVoltage);
		
		setCANJaguar(1, kRunningValue);
		delayTillInCorrectStateWithMessage(Level.FINE, kMotorTimeSettling, "SpeedReducing settling to max", new BooleanCheck(){
			@Override
			public boolean getAsBoolean(){
				runMotorForward();
				return getME().getMotor().getSpeed() == 0;
			}
		});
		//then
		assertEquals("Speed did not go to zero when the max output voltage was set to " + maxVoltage, 0, getME().getMotor().getSpeed(), kEncoderSpeedTolerance);
	}

}
