// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicLong;
import org.junit.jupiter.api.BeforeEach;

abstract class TriggerRescopeTestBase extends CommandTestBase {
  private AtomicLong m_nextScopeId;

  @BeforeEach
  void setUpScopeIds() {
    m_nextScopeId = new AtomicLong();
  }

  protected Runnable enterScope(ScopeType scope, Runnable setupAction) {
    return switch (scope) {
      case GLOBAL -> {
        m_opModeId = 0;
        m_opModeName = "";
        setupAction.run();
        yield () -> {};
      }
      case OPMODE -> {
        m_opModeId = nextScopeId();
        m_opModeName = "mode " + m_opModeId;
        setupAction.run();
        yield () -> {
          m_opModeId = 0;
          m_opModeName = "";
        };
      }
      case COMMAND -> {
        var scopeCommand =
            Command.noRequirements(
                    co -> {
                      setupAction.run();
                      co.park();
                    })
                .named("Scope command");
        m_scheduler.schedule(scopeCommand);
        m_scheduler.run();
        assertTrue(m_scheduler.isRunning(scopeCommand), "Scope command should still be running");

        yield () -> {
          m_scheduler.cancel(scopeCommand);
          m_scheduler.run();
        };
      }
    };
  }

  private long nextScopeId() {
    return m_nextScopeId.incrementAndGet();
  }

  protected enum ScopeType {
    COMMAND,
    OPMODE,
    GLOBAL,
  }
}
