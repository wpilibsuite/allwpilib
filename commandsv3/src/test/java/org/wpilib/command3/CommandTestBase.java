// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.BeforeEach;
import org.wpilib.system.RobotController;

class CommandTestBase {
  protected Scheduler m_scheduler;
  protected List<SchedulerEvent> m_events;

  @BeforeEach
  void initScheduler() {
    RobotController.setTimeSource(() -> System.nanoTime() / 1000L);
    m_scheduler = Scheduler.createIndependentScheduler();
    m_events = new ArrayList<>();
    m_scheduler.addEventListener(m_events::add);
  }
}
