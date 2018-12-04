/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.helpers;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.extension.ExtendWith;

import edu.wpi.first.wpilibj.command.Scheduler;

/**
 * Extend this class when your test requires commands or command groups
 * to be exercised with the full WPI scheduler. Use 
 * {@link SchedulerPumpHelper#runForDuration(int, int...)} to pump the scheduler.
 */
@ExtendWith(MockHardwareExtension.class)
public class TestWithScheduler {
  @BeforeAll
  public static void schedulerStart() {
    Scheduler.getInstance().removeAll();
    Scheduler.getInstance().enable();
  }

  @BeforeEach
  public void schedulerClear() {
    Scheduler.getInstance().removeAll();
  }

  @AfterAll
  public static void schedulerDestroy() {
    Scheduler.getInstance().disable();
    Scheduler.getInstance().close();
  }
}
