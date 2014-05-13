/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;


/**
 * This class provides access to all of the elements on the test bench, for use
 * in fixtures. This class is a singleton, you should use {@link #getInstance()}
 * to obtain a reference to the {@code TestBench}.
 * 
 * TODO: This needs to be updated to the most recent test bench setup.
 * 
 * @author Fredric Silberberg
 */
public final class TestBench {

	public static TestBench instance;

	public static TestBench getInstance() {
		if (instance == null) {
			instance = new TestBench();
		}
		return instance;
	}
}
