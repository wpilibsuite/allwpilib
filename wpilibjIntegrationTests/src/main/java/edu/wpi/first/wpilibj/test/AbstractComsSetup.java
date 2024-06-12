// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.MockDS;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.rules.TestWatcher;
import org.junit.runner.Description;
import org.junit.runners.model.MultipleFailureException;

/**
 * This class serves as a superclass for all tests that involve the hardware on the roboRIO. It uses
 * an {@link BeforeClass} statement to initialize network communications. Any test that needs to use
 * the hardware <b>MUST</b> extend from this class, to ensure that all of the hardware will be able
 * to run.
 */
public abstract class AbstractComsSetup {
  /** Stores whether network coms have been initialized. */
  private static boolean initialized = false;

  // We have no way to stop the MockDS, so its thread is daemon.
  private static MockDS ds;

  /*
   * This sets up the network communications library to enable the driver station. After starting
   * network coms, it will loop until the driver station returns that the robot is enabled, to
   * ensure that tests will be able to run on the hardware.
   */
  static {
    if (!initialized) {
      try {
        // Set some implementations so that the static methods work properly
        HAL.initialize(500, 0);
        DriverStationJNI.observeUserProgramStarting();
        DriverStation.getAlliance();

        ds = new MockDS();
        ds.start();

        LiveWindow.setEnabled(false);
        TestBench.out().println("Started coms");
      } catch (Exception ex) {
        TestBench.out().println("Exception during AbstractComsSetup initialization: " + ex);
        ex.printStackTrace(TestBench.out());
        throw ex;
      }

      // Wait until the robot is enabled before starting the tests
      int enableCounter = 0;
      DriverStation.refreshData();
      while (!DriverStation.isEnabled()) {
        if (enableCounter > 50) {
          // Robot did not enable properly after 5 seconds.
          // Force exit
          TestBench.err().println("Failed to enable. Aborting");
          System.exit(1);
        }
        try {
          Thread.sleep(100);
        } catch (InterruptedException ex) {
          ex.printStackTrace();
        }
        TestBench.out().println("Waiting for enable: " + enableCounter++);
        DriverStation.refreshData();
      }
      TestBench.out().println();

      // Ready to go!
      initialized = true;
      TestBench.out().println("Running!");
    }
  }

  protected abstract Logger getClassLogger();

  /** This causes a stack trace to be printed as the test is running as well as at the end. */
  @Rule
  public final TestWatcher getTestWatcher() {
    return getOverridenTestWatcher();
  }

  /**
   * Given as a way to provide a custom test watcher for a test or set of tests.
   *
   * @return the test watcher to use
   */
  protected TestWatcher getOverridenTestWatcher() {
    return new DefaultTestWatcher();
  }

  protected class DefaultTestWatcher extends TestWatcher {
    /**
     * Allows a failure to supply a custom status message to be displayed along with the stack
     * trace.
     */
    protected void failed(Throwable throwable, Description description, String status) {
      TestBench.out().println();
      // Instance of is the best way I know to retrieve this data.
      if (throwable instanceof MultipleFailureException mfe) {
        /*
         * MultipleFailureExceptions hold multiple exceptions in one exception.
         * In order to properly display these stack traces we have to cast the
         * throwable and work with the list of thrown exceptions stored within
         * it.
         */
        int exceptionCount = 1; // Running exception count
        int failureCount = mfe.getFailures().size();
        for (Throwable singleThrown : mfe.getFailures()) {
          getClassLogger()
              .logp(
                  Level.SEVERE,
                  description.getClassName(),
                  description.getMethodName(),
                  (exceptionCount++)
                      + "/"
                      + failureCount
                      + " "
                      + description.getDisplayName()
                      + " "
                      + "failed "
                      + singleThrown.getMessage()
                      + "\n"
                      + status,
                  singleThrown);
        }

      } else {
        getClassLogger()
            .logp(
                Level.SEVERE,
                description.getClassName(),
                description.getMethodName(),
                description.getDisplayName() + " failed " + throwable.getMessage() + "\n" + status,
                throwable);
      }
      super.failed(throwable, description);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.junit.rules.TestWatcher#failed(java.lang.Throwable,
     * org.junit.runner.Description)
     */
    @Override
    protected void failed(Throwable exception, Description description) {
      failed(exception, description, "");
    }

    /*
     * (non-Javadoc)
     *
     * @see org.junit.rules.TestWatcher#starting(org.junit.runner.Description)
     */
    @Override
    protected void starting(Description description) {
      TestBench.out().println();
      // Wait until the robot is enabled before starting the next tests
      int enableCounter = 0;
      while (!DriverStation.isEnabled()) {
        try {
          Thread.sleep(100);
        } catch (InterruptedException ex) {
          ex.printStackTrace();
        }
        // Prints the message on one line overwriting itself each time
        TestBench.out().print("\rWaiting for enable: " + enableCounter++);
      }
      getClassLogger()
          .logp(Level.INFO, description.getClassName(), description.getMethodName(), "Starting");
      super.starting(description);
    }

    @Override
    protected void succeeded(Description description) {
      simpleLog(Level.INFO, "TEST PASSED!");
      super.succeeded(description);
    }
  }

  /**
   * Logs a simple message without the logger formatting associated with it.
   *
   * @param level The level to log the message at
   * @param message The message to log
   */
  protected void simpleLog(Level level, String message) {
    if (getClassLogger().isLoggable(level)) {
      TestBench.out().println(message);
    }
  }

  /**
   * Provided as a replacement to lambda functions to allow for repeatable checks to see if a
   * correct state is reached.
   */
  public abstract static class BooleanCheck {
    public BooleanCheck() {}

    /**
     * Runs the enclosed code and evaluates it to determine what state it should return.
     *
     * @return true if the code provided within the method returns true
     */
    public abstract boolean getAsBoolean();
  }

  /**
   * Delays until either the correct state is reached or we reach the timeout.
   *
   * @param level The level to log the message at.
   * @param timeout How long the delay should run before it should timeout and allow the test to
   *     continue
   * @param message The message to accompany the delay. The message will display 'message' took
   *     'timeout' seconds if it passed.
   * @param correctState A method to determine if the test has reached a state where it is valid to
   *     continue
   * @return a double representing how long the delay took to run in seconds.
   */
  public double delayTillInCorrectStateWithMessage(
      Level level, double timeout, String message, BooleanCheck correctState) {
    int timeoutIndex;
    // As long as we are not in the correct state and the timeout has not been
    // reached then continue to run this loop
    for (timeoutIndex = 0;
        timeoutIndex < (timeout * 100) && !correctState.getAsBoolean();
        timeoutIndex++) {
      Timer.delay(0.01);
    }
    if (correctState.getAsBoolean()) {
      simpleLog(level, message + " took " + (timeoutIndex * 0.01) + " seconds");
    } else {
      simpleLog(level, message + " timed out after " + (timeoutIndex * 0.01) + " seconds");
    }
    return timeoutIndex * 0.01;
  }
}
