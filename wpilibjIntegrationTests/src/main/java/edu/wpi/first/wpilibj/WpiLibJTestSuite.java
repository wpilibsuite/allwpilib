/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
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
 * Holds all of the tests in the root wpilibj directory. Please list alphabetically so that it is
 * easy to determine if a test is missing from the list.
 */
@RunWith(Suite.class)
@SuiteClasses({AnalogCrossConnectTest.class, AnalogPotentiometerTest.class,
    BuiltInAccelerometerTest.class,
    CircularBufferTest.class, ConstantsPortsTest.class, CounterTest.class,
    DigitalGlitchFilterTest.class, DIOCrossConnectTest.class, DriverStationTest.class,
    EncoderTest.class, FilterNoiseTest.class, FilterOutputTest.class, GyroTest.class,
    MotorEncoderTest.class, MotorInvertingTest.class, PCMTest.class, PDPTest.class,
    PIDTest.class, PIDToleranceTest.class, PreferencesTest.class, RelayCrossConnectTest.class,
    SampleTest.class, TimerTest.class})
public class WpiLibJTestSuite extends AbstractTestSuite {
}
