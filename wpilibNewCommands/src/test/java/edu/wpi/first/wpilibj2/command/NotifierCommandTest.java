/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.condition.DisabledOnOs;
import org.junit.jupiter.api.condition.OS;

import edu.wpi.first.wpilibj.Timer;

import static org.junit.jupiter.api.Assertions.assertTrue;

@DisabledOnOs(OS.MAC)
class NotifierCommandTest extends CommandTestBase {
  @Test
  void notifierCommandScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Counter counter = new Counter();

    NotifierCommand command = new NotifierCommand(counter::increment, .01);

    scheduler.schedule(command);
    Timer.delay(.25);
    scheduler.cancel(command);

    assertTrue(counter.m_counter > 10, "Should have hit at least 10 triggers");
    assertTrue(counter.m_counter < 100, "Shouldn't hit more then 100 triggers");
  }
}
