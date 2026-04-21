// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.wpilib.system.RobotController;

class CommandTestBase {
  protected Scheduler m_scheduler;
  protected List<SchedulerEvent> m_events;
  protected long m_opModeId = 0;
  protected String m_opModeName = "";

  @BeforeEach
  void initScheduler() {
    RobotController.setTimeSource(() -> System.nanoTime() / 1000L);
    m_scheduler = Scheduler.createIndependentScheduler();
    m_events = new ArrayList<>();
    m_scheduler.addEventListener(m_events::add);
  }

  @BeforeEach
  void initOpmodeFetcher() {
    OpModeFetcher.setFetcher(
        new OpModeFetcher() {
          @Override
          long getOpModeId() {
            return m_opModeId;
          }

          @Override
          String getOpModeName() {
            return m_opModeName;
          }
        });
  }

  @AfterEach
  void resetOpmodeFetcher() {
    m_opModeId = 0;
    m_opModeName = "";
  }
}
