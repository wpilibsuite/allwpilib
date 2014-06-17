/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import java.io.IOException;
import java.io.InputStream;
import java.util.logging.LogManager;
import java.util.logging.Logger;

import org.junit.runner.JUnitCore;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import edu.wpi.first.wpilibj.AnalogCrossConnectTest;
import edu.wpi.first.wpilibj.CounterTest;
import edu.wpi.first.wpilibj.DIOCrossConnectTest;
import edu.wpi.first.wpilibj.EncoderTest;
import edu.wpi.first.wpilibj.MotorEncoderTest;
import edu.wpi.first.wpilibj.PIDTest;
import edu.wpi.first.wpilibj.PrefrencesTest;
import edu.wpi.first.wpilibj.RelayCrossConnectTest;
import edu.wpi.first.wpilibj.SampleTest;
import edu.wpi.first.wpilibj.TiltPanCameraTest;
import edu.wpi.first.wpilibj.TimerTest;
import edu.wpi.first.wpilibj.WpiLibJTestSuite;
import edu.wpi.first.wpilibj.command.ButtonTest;
import edu.wpi.first.wpilibj.command.CommandParallelGroupTest;
import edu.wpi.first.wpilibj.command.CommandScheduleTest;
import edu.wpi.first.wpilibj.command.CommandSequentialGroupTest;
import edu.wpi.first.wpilibj.command.CommandSupersedeTest;
import edu.wpi.first.wpilibj.command.CommandTestSuite;
import edu.wpi.first.wpilibj.command.CommandTimeoutTest;
import edu.wpi.first.wpilibj.command.DefaultCommandTest;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboardTest;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboardTestSuite;

/**
 * The WPILibJ Integeration Test Suite collects all of the tests to be run by
 * junit. In order for a test to be run, it must be added the list of suite
 * classes below. The tests will be run in the order they are listed in the
 * suite classes annotation.
 */
@RunWith(Suite.class)
@SuiteClasses({ 
				WpiLibJTestSuite.class,
				CommandTestSuite.class,
				SmartDashboardTestSuite.class
				})
//NOTE: THESE ARE EACH LISTED ON SEPERATE LINES TO PREVENT GIT MERGE CONFLICTS!
@SuppressWarnings("unused")
public class TestSuite {
	static{
		final InputStream inputStream = TestSuite.class.getResourceAsStream("/logging.properties");
		try
		{
			if(inputStream == null ) throw new NullPointerException("./logging.properties was not loaded");
		    LogManager.getLogManager().readConfiguration(inputStream);
		    Logger.getAnonymousLogger().info("Loaded");
		}
		catch (final IOException | NullPointerException e)
		{
		    Logger.getAnonymousLogger().severe("Could not load default logging.properties file");
		    Logger.getAnonymousLogger().severe(e.getMessage());
		}
		
		System.out.println("Starting Tests");
	}
	private static final Logger WPILIBJ_ROOT_LOGGER = Logger.getLogger("edu.wpi.first.wpilibj");
	private static final Logger WPILIBJ_COMMAND_ROOT_LOGGER = Logger.getLogger("edu.wpi.first.wpilibj.command");

	public static void main(String[] args) {
		JUnitCore.main("edu.wpi.first.wpilibj.test.TestSuite");
	}

}
