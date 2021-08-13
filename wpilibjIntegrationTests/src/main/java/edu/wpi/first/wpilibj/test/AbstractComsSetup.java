// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.MockDS;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import java.util.logging.Logger;
import org.junit.jupiter.api.extension.ExtendWith;

/**
 * This class serves as a superclass for all tests that involve the hardware on the roboRIO. It uses
 * an {@link BeforeAll} statement to initialize network communications. Any test that needs to use
 * the hardware <b>MUST</b> extend from this class, to ensure that all of the hardware will be able
 * to run.
 */
@ExtendWith(DefaultTestWatcher.class)
public abstract class AbstractComsSetup {
  /** Stores whether network coms have been initialized. */
  private static boolean initialized = false;

  // We have no way to stop the MockDS, so its thread is daemon.
  private static MockDS ds;

  /**
   * This sets up the network communications library to enable the driver station. After starting
   * network coms, it will loop until the driver station returns that the robot is enabled, to
   * ensure that tests will be able to run on the hardware.
   */
  static {
    if (!initialized) {
      try {
        // Set some implementations so that the static methods work properly
        HAL.initialize(500, 0);
        HAL.observeUserProgramStarting();
        DriverStation.getAlliance();

        ds = new MockDS();
        ds.start();

        LiveWindow.setEnabled(false);
        System.out.println("Started coms");
      } catch (Exception ex) {
        System.out.println("Exception during AbstractComsSetup initialization: " + ex);
        ex.printStackTrace(System.out);
        throw ex;
      }

      // Wait until the robot is enabled before starting the tests
      int enableCounter = 0;
      while (!DriverStation.isEnabled()) {
        if (enableCounter > 50) {
          // Robot did not enable properly after 5 seconds.
          // Force exit
          System.err.println("Failed to enable. Aborting");
          System.exit(1);
        }
        try {
          Thread.sleep(100);
        } catch (InterruptedException ex) {
          ex.printStackTrace();
        }
        System.out.println("Waiting for enable: " + enableCounter++);
      }
      System.out.println();

      // Ready to go!
      initialized = true;
      System.out.println("Running!");
    }
  }

  protected abstract Logger getClassLogger();
}
