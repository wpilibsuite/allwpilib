/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.logging.LogManager;
import java.util.logging.Logger;

import junit.runner.Version;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;

import edu.wpi.first.wpilibj.WpiLibJTestSuite;
import edu.wpi.first.wpilibj.can.CANTestSuite;
import edu.wpi.first.wpilibj.command.CommandTestSuite;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboardTestSuite;

/**
 * The WPILibJ Integeration Test Suite collects all of the tests to be run by
 * junit. In order for a test to be run, it must be added the list of suite
 * classes below. The tests will be run in the order they are listed in the
 * suite classes annotation.
 */
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
	
	
	
	//NOTE: THESE ARE EACH LISTED ON SEPERATE LINES TO PREVENT GIT MERGE CONFLICTS!
	private static final Class<?>[] testList = {
		WpiLibJTestSuite.class,
		CANTestSuite.class,
		CommandTestSuite.class,
		SmartDashboardTestSuite.class
	};
	
	/**
	 * Constructs a hash map of test suite names and the associated class parameters
	 */
	public static HashMap<String, Class<?>> getClassNameHash(){
		HashMap<String, Class<?>> classHash = new HashMap<String, Class<?>>();
		System.out.print("To select a test suite run with any of the following parameters:" + "\n\t");
		for(Class<?> t : testList){
			String name = t.getSimpleName().toLowerCase().replace("test", "").replace("suite", "");
			System.out.print(name + " ");
			classHash.put(name, t);
		}
		System.out.println();
		return classHash;
	}

	/**
	 * The method called at runtime
	 * @param args The test suites to run
	 */
	public static void main(String[] args) {
		System.out.println("JUnit version " + Version.id());
		
		//Select the test suites to run given the args passed at runtime
		ArrayList<Class<?>> runClasses;
		if(args.length != 0){ //If args are passed to the test suite
			//Display the list of arguments passed
			StringBuilder paramList = new StringBuilder("Params received: ");
			for(String a : args){
				a = a.toLowerCase();
				paramList.append(a+", ");
			}
			paramList.delete(paramList.length()-2, paramList.length());
			System.out.println(paramList); //Print the parsed arg list
			
			ArrayList<String> argsParsed = new ArrayList<String>(Arrays.asList(args));
			//The list of loaded classes
			HashMap<String, Class<?>> loadedClasses = getClassNameHash();
			runClasses = new ArrayList<Class<?>>();
			//List the test that will be run
			System.out.println("Running the following tests:");
			for(String a : argsParsed){
				Class<?> load = loadedClasses.get(a);
				if(load!=null){
					runClasses.add(load);
					System.out.println("\t" + a);
				}
			}
			if(argsParsed.contains("quick")){//Allows a quick test to be written without the framework overhead test functionality
				runClasses.add(QuickTest.class);
			}
			
		} else { //If no args are passed to the test suite then run all of the test suites except for the QuickTest
			runClasses = new ArrayList<Class<?>>();
			System.out.println("Running the following tests:");
			for(Class<?> c : testList){
				runClasses.add(c);
				System.out.println("\t" + c.getSimpleName());
			}
		}
		
		//Tests are run here
		Result result = JUnitCore.runClasses(runClasses.toArray(new Class[0]));
		
		
		
		
		//Results are collected and displayed here
		System.out.println("\n");
		if(!result.wasSuccessful()){
			//Prints out a list of stack traces for the failed tests
			System.out.println("Failure List: ");
			for(Failure f : result.getFailures()){
				System.out.println(f.getDescription());
				System.out.println(f.getTrace());
			}
			System.out.println();
			System.out.println("FAILURES!!!");
			//Print the test statistics
			System.out.println("Tests run: " + result.getRunCount() +
					", Failures: " + result.getFailureCount() + 
					", Ignored: " + result.getIgnoreCount() + 
					", In " + result.getRunTime() + "ms");
	
			//Prints out a list of test that failed
			System.out.println("Failure List (short):");
			String failureClass = result.getFailures().get(0).getDescription().getClassName();
			System.out.println(failureClass);
			for(Failure f : result.getFailures()){
				if(!failureClass.equals(f.getDescription().getClassName())){
					failureClass = f.getDescription().getClassName();
					System.out.println(failureClass);
				}
				System.err.println("\t" + f.getDescription());
			}
		} else {
			System.out.println("SUCCESS!");
			System.out.println("Tests run: " + result.getRunCount() +
					", Ignored: " + result.getIgnoreCount() + 
					", In " + result.getRunTime() + "ms");
		}
		System.out.println();
			
        System.exit(result.wasSuccessful() ? 0 : 1);
	}
}
