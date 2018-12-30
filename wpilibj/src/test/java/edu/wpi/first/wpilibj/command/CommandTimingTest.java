/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

/**
 * Test the timing telemetry of a {@link Command}.
 */
class CommandTimingTest extends AbstractCommandTest {
  /**
   * Test timing telemetry of a short delay in initialization.
   */
  @Test
  void initializeTimingTest() {
    final MockCommand command = new MockCommand() {
      @Override
      public void initialize() {
        ++m_initializeCount;
        sleep(50);
      }

      @Override
      public void execute() {
        ++m_executeCount;
        sleep(10);
      }
    };

    command.start();
    assertCommandState(command, 0, 0, 0, 0, 0);
    assertEquals(command.getInitializeTime(), -1);
    Scheduler.getInstance().run();
    assertCommandState(command, 0, 0, 0, 0, 0);
    assertEquals(command.getInitializeTime(), -1);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 1, 1, 0, 0);
    assertApprox(command.getInitializeTime(), 0.05);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 2, 2, 0, 0);
    assertApprox(command.getInitializeTime(), 0.05);
  }

  /**
   * Test timing telemetry of a short delay in execution.
   */
  @Test
  void executeTimingTest() {
    final MockCommand command = new MockCommand() {
      @Override
      public void execute() {
        ++m_executeCount;
        sleep(100);
      }
    };

    command.start();
    assertCommandState(command, 0, 0, 0, 0, 0);
    assertEquals(command.getExecuteTime(), -1);
    Scheduler.getInstance().run();
    assertCommandState(command, 0, 0, 0, 0, 0);
    assertEquals(command.getExecuteTime(), -1);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 1, 1, 0, 0);
    assertApprox(command.getExecuteTime(), 0.1);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 2, 2, 0, 0);
    assertApprox(command.getExecuteTime(), 0.1);
  }

  /**
   * Test timing telemetry of a long delay in initialization and execution.
   */
  @Test
  void longTimingTest() {
    final MockCommand command = new MockCommand() {
      @Override
      public void initialize() {
        ++m_initializeCount;
        sleep(500);
      }

      @Override
      public void execute() {
        ++m_executeCount;
        sleep(2500);
      }
    };

    command.start();
    assertCommandState(command, 0, 0, 0, 0, 0);
    assertEquals(command.getInitializeTime(), -1);
    assertEquals(command.getExecuteTime(), -1);
    Scheduler.getInstance().run();
    assertCommandState(command, 0, 0, 0, 0, 0);
    assertEquals(command.getInitializeTime(), -1);
    assertEquals(command.getExecuteTime(), -1);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 1, 1, 0, 0);
    assertApprox(command.getInitializeTime(), 0.5);
    assertApprox(command.getExecuteTime(), 2.5);
    Scheduler.getInstance().run();
    assertCommandState(command, 1, 2, 2, 0, 0);
    assertApprox(command.getInitializeTime(), 0.5);
    assertApprox(command.getExecuteTime(), 2.5);
  }

  void assertApprox(double value, double real) {
    double lower = real * 0.9;
    double upper = real * 1.1;

    assertTrue(value >= lower && value <= upper);
  }
}
