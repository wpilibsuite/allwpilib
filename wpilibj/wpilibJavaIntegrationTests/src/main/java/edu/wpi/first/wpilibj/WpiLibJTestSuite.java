/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import edu.wpi.first.wpilibj.test.AbstractTestSuite;

/**
 * @author jonathanleitschuh
 * Holds all of the tests in the roor wpilibj directory
 * Please list alphabetically so that it is easy to determine if a test is missing from the list
 */
@RunWith(Suite.class)
@SuiteClasses({
				BuiltInAccelerometerTest.class,
				AnalogCrossConnectTest.class,
				AnalogPotentiometerTest.class,
				CounterTest.class,
				DIOCrossConnectTest.class,
				EncoderTest.class,
				MotorEncoderTest.class,
				PCMTest.class,
				PIDTest.class,
				PDPTest.class,
				PrefrencesTest.class,
				RelayCrossConnectTest.class,
				SampleTest.class,
				TiltPanCameraTest.class,
				TimerTest.class
				})
public class WpiLibJTestSuite extends AbstractTestSuite {
}
