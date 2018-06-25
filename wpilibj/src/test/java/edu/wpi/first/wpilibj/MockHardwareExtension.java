/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.TimeUnit;

import com.google.common.base.Stopwatch;

import org.junit.jupiter.api.extension.BeforeAllCallback;
import org.junit.jupiter.api.extension.ExtensionContext;
import org.junit.jupiter.api.extension.ExtensionContext.Namespace;

import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

public final class MockHardwareExtension implements BeforeAllCallback {
  private static ExtensionContext getRoot(ExtensionContext context) {
    return context.getParent().map(MockHardwareExtension::getRoot).orElse(context);
  }

  @Override
  public void beforeAll(ExtensionContext context) throws Exception {
    getRoot(context).getStore(Namespace.GLOBAL).getOrComputeIfAbsent("HAL Initalized", key -> {
      initializeHardware();
      return true;
    }, Boolean.class);
  }

  private void initializeHardware() {
    HAL.initialize(500, 0);
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
