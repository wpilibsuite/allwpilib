// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.test.AbstractTestSuite;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

/**
 * Holds all of the tests in the root wpilibj directory. Please list alphabetically so that it is
 * easy to determine if a test is missing from the list.
 */
@RunWith(Suite.class)
@SuiteClasses({
  AnalogCrossConnectTest.class,
  AnalogPotentiometerTest.class,
  BuiltInAccelerometerTest.class,
  ConstantsPortsTest.class,
  CounterTest.class,
  DigitalGlitchFilterTest.class,
  DIOCrossConnectTest.class,
  DMATest.class,
  DriverStationTest.class,
  EncoderTest.class,
  GyroTest.class,
  MotorEncoderTest.class,
  MotorInvertingTest.class,
  PCMTest.class,
  PDPTest.class,
  PIDTest.class,
  RelayCrossConnectTest.class,
  SampleTest.class,
  TimerTest.class
})
public class WpiLibJTestSuite extends AbstractTestSuite {}
