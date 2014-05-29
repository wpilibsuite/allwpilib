/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import org.junit.BeforeClass;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * This class serves as a superclass for all tests that involve the hardware on
 * the roboRIO. It uses an {@link BeforeClass} statement to initialize network
 * communications. Any test that needs to use the hardware <b>MUST</b> extend
 * from this class, to ensure that all of the hardware will be able to run.
 * 
 * @author Fredric Silberberg
 */
public abstract class AbstractComsSetup {

	/** Stores whether network coms have been initialized */
	private static boolean initialized = false;

	/**
	 * This sets up the network communications library to enable the driver
	 * station. After starting network coms, it will loop until the driver
	 * station returns that the robot is enabled, to ensure that tests will be
	 * able to run on the hardware.
	 * 
	 * @throws InterruptedException
	 *             If we failed to sleep while waiting for the driver station to
	 *             enable.
	 */
	@BeforeClass
	public static void setupNetworkComs() throws InterruptedException {
		if (!initialized) {
			// Start up the network communications
			FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationReserve();
			FRCNetworkCommunicationsLibrary
					.FRCNetworkCommunicationObserveUserProgramStarting();
			LiveWindow.setEnabled(false);
			System.out.println("Started coms");

			// Wait until the robot is enabled before starting the tests
			while (!DriverStation.getInstance().isEnabled()) {
				Thread.sleep(100);
				System.out.println("Waiting for enable");
			}

			// Ready to go!
			initialized = true;
			System.out.println("Running!");
		}
	}
}
