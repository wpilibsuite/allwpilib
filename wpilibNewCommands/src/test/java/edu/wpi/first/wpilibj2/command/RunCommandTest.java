/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class RunCommandTest extends CommandTestBase {
  @Test
  void runCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Counter counter = new Counter();

      RunCommand command = new RunCommand(counter::increment);

      scheduler.schedule(command);
      scheduler.run();
      scheduler.run();
      scheduler.run();

      assertEquals(3, counter.m_counter);
    }
  }
}
