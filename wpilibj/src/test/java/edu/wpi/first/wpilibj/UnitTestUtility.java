/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import com.google.common.base.Stopwatch;

import java.util.concurrent.TimeUnit;

import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

/**
 * Utility class for configuring unit tests.
 */
public final class UnitTestUtility {
  private UnitTestUtility() {
        /* no-op */
  }

  /**
   * Sets up the base system WPILib so that it does not rely on hardware.
   */
  public static void setupMockBase() {
    try {
      // Check to see if this has been setup
      Timer.getFPGATimestamp();
    } catch (BaseSystemNotInitializedException ex) {
      // If it hasn't been then do this setup

      HLUsageReporting.SetImplementation(new HLUsageReporting.Null());
      RobotState.SetImplementation(new MockRobotStateInterface());
      Timer.SetImplementation(new Timer.StaticInterface() {

        @Override
        public double getFPGATimestamp() {
          return System.currentTimeMillis() / 1000.0;
        }

        @Override
        public double getMatchTime() {
          return 0;
        }

        @Override
        public void delay(double seconds) {
          try {
            Thread.sleep((long) (seconds * 1e3));
          } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            throw new RuntimeException("Thread was interrupted", ex);
          }
        }

        @Override
        public Timer.Interface newTimer() {
          return new Timer.Interface() {
            private final Stopwatch m_stopwatch = Stopwatch.createUnstarted();

            @Override
            public double get() {
              return m_stopwatch.elapsed(TimeUnit.SECONDS);
            }

            @Override
            public void reset() {
              m_stopwatch.reset();
            }

            @Override
            public void start() {
              m_stopwatch.start();
            }

            @Override
            public void stop() {
              m_stopwatch.stop();
            }

            @Override
            public boolean hasPeriodPassed(double period) {
              if (get() > period) {
                // Advance the start time by the period.
                // Don't set it to the current time... we want to avoid drift.
                m_stopwatch.reset().start();
                return true;
              }
              return false;
            }
          };
        }
      });
    }
  }
}
