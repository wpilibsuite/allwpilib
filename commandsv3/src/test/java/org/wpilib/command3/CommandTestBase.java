// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Predicate;
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
    RobotController.setTimeSource(System::nanoTime);
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

  /**
   * Asserts that any scheduler event of the given class occurred and that the given predicate
   * returns true for the event. Does not make any assertions on the order of the event.
   *
   * @param eventClass the class of the event to assert
   * @param tester a predicate that tests the event
   * @param message the message to include in the exception if the event is not found
   * @param <E> the type of the event to assert
   */
  <E extends SchedulerEvent> void assertSchedulerEvent(
      Class<E> eventClass, Predicate<E> tester, String message) {
    if (m_events.stream().filter(eventClass::isInstance).map(eventClass::cast).anyMatch(tester)) {
      return;
    }

    fail(message);
  }
}
