package edu.wpi.first.wpilibj.test;

import org.junit.runner.JUnitCore;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;


import edu.wpi.first.wpilibj.MotorEncoderTest;
import edu.wpi.first.wpilibj.SampleTest;
import edu.wpi.first.wpilibj.TimerTest;

/**
 * The WPILibJ Integeration Test Suite collects all of the tests to be run by
 * junit. In order for a test to be run, it must be added the list of suite
 * classes below. The tests will be run in the order they are listed in the
 * suite classes annotation.
 */
@RunWith(Suite.class)
@SuiteClasses({ SampleTest.class, MotorEncoderTest.class, TimerTest.class })
public class TestSuite {

	public static void main(String[] args) {
		JUnitCore.main("edu.wpi.first.wpilibj.test.TestSuite");
	}

}
