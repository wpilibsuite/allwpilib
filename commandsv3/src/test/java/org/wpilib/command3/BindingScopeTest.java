// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertSame;

import org.junit.jupiter.api.Test;

class BindingScopeTest extends SchedulerTest {
  @Test
  void narrowestScopeInOpmode() {
    m_opModeId = 1;
    m_opModeName = "Opmode 1";

    var scope = BindingScope.createNarrowestScope(m_scheduler);
    assertInstanceOf(BindingScope.ForOpmode.class, scope);
    assertEquals(m_opModeId, ((BindingScope.ForOpmode) scope).opmodeId());
  }

  @Test
  void narrowestScopeInCommand() {
    BindingScope[] scopeRef = new BindingScope[1];

    var command =
        Command.noRequirements()
            .executing(
                coroutine -> {
                  BindingScope scope = BindingScope.createNarrowestScope(m_scheduler);
                  scopeRef[0] = scope;
                })
            .named("Command 1");

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertNotNull(scopeRef[0], "Command did not execute");
    assertInstanceOf(BindingScope.ForCommand.class, scopeRef[0]);
    assertSame(command, ((BindingScope.ForCommand) scopeRef[0]).command());
  }

  @Test
  void narrowestScopeInOpmodeAndCommand() {
    m_opModeId = 314;
    m_opModeName = "Opmode pi";

    BindingScope[] scopeRef = new BindingScope[1];
    var command =
        Command.noRequirements()
            .executing(
                coroutine -> {
                  BindingScope scope = BindingScope.createNarrowestScope(m_scheduler);
                  scopeRef[0] = scope;
                })
            .named("Command pi");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // Scope should be for the command, not the opmode
    assertNotNull(scopeRef[0], "Command did not execute");
    assertInstanceOf(BindingScope.ForCommand.class, scopeRef[0]);
    assertSame(command, ((BindingScope.ForCommand) scopeRef[0]).command());
  }

  @Test
  void narrowestScopeWithoutOpmodeOrCommand() {
    // Just to be explicit: an opmode ID of zero means no opmode is selected in the DS
    m_opModeId = 0;
    m_opModeName = null;

    var scope = BindingScope.createNarrowestScope(m_scheduler);
    assertSame(BindingScope.Global.INSTANCE, scope);
  }
}
