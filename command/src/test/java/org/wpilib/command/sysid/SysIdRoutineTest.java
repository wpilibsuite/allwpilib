// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.sysid;

import static edu.wpi.first.units.Units.Volts;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.atLeastOnce;
import static org.mockito.Mockito.clearInvocations;
import static org.mockito.Mockito.inOrder;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import edu.wpi.first.wpilibj.sysid.SysIdRoutineLog;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Subsystem;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class SysIdRoutineTest {
  interface Mechanism extends Subsystem {
    void recordState(SysIdRoutineLog.State state);

    void drive(Voltage voltage);

    void log(SysIdRoutineLog log);
  }

  Mechanism m_mechanism;
  SysIdRoutine m_sysidRoutine;
  Command m_quasistaticForward;
  Command m_quasistaticReverse;
  Command m_dynamicForward;
  Command m_dynamicReverse;

  void runCommand(Command command) {
    command.initialize();
    command.execute();
    command.execute();
    SimHooks.stepTiming(1);
    command.execute();
    command.end(true);
  }

  @BeforeEach
  void setup() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    m_mechanism = mock(Mechanism.class);
    m_sysidRoutine =
        new SysIdRoutine(
            new SysIdRoutine.Config(null, null, null, m_mechanism::recordState),
            new SysIdRoutine.Mechanism(m_mechanism::drive, m_mechanism::log, new Subsystem() {}));
    m_quasistaticForward = m_sysidRoutine.quasistatic(SysIdRoutine.Direction.kForward);
    m_quasistaticReverse = m_sysidRoutine.quasistatic(SysIdRoutine.Direction.kReverse);
    m_dynamicForward = m_sysidRoutine.dynamic(SysIdRoutine.Direction.kForward);
    m_dynamicReverse = m_sysidRoutine.dynamic(SysIdRoutine.Direction.kReverse);
  }

  @AfterEach
  void cleanupAll() {
    SimHooks.resumeTiming();
  }

  @Test
  void recordStateBookendsMotorLogging() {
    runCommand(m_quasistaticForward);

    var orderCheck = inOrder(m_mechanism);

    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.kQuasistaticForward);
    orderCheck.verify(m_mechanism).drive(any());
    orderCheck.verify(m_mechanism).log(any());
    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.kNone);
    orderCheck.verifyNoMoreInteractions();

    clearInvocations(m_mechanism);
    orderCheck = inOrder(m_mechanism);
    runCommand(m_dynamicForward);

    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.kDynamicForward);
    orderCheck.verify(m_mechanism).drive(any());
    orderCheck.verify(m_mechanism).log(any());
    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.kNone);
    orderCheck.verifyNoMoreInteractions();
  }

  @Test
  void testsDeclareCorrectState() {
    runCommand(m_quasistaticForward);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.kQuasistaticForward);

    runCommand(m_quasistaticReverse);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.kQuasistaticReverse);

    runCommand(m_dynamicForward);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.kDynamicForward);

    runCommand(m_dynamicReverse);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.kDynamicReverse);
  }

  @Test
  void testsOutputCorrectVoltage() {
    runCommand(m_quasistaticForward);
    var orderCheck = inOrder(m_mechanism);

    orderCheck.verify(m_mechanism, atLeastOnce()).drive(Volts.of(1));
    orderCheck.verify(m_mechanism).drive(Volts.of(0));
    orderCheck.verify(m_mechanism, never()).drive(any());

    clearInvocations(m_mechanism);
    runCommand(m_quasistaticReverse);
    orderCheck = inOrder(m_mechanism);

    orderCheck.verify(m_mechanism, atLeastOnce()).drive(Volts.of(-1));
    orderCheck.verify(m_mechanism).drive(Volts.of(0));
    orderCheck.verify(m_mechanism, never()).drive(any());

    clearInvocations(m_mechanism);
    runCommand(m_dynamicForward);
    orderCheck = inOrder(m_mechanism);

    orderCheck.verify(m_mechanism, atLeastOnce()).drive(Volts.of(7));
    orderCheck.verify(m_mechanism).drive(Volts.of(0));
    orderCheck.verify(m_mechanism, never()).drive(any());

    clearInvocations(m_mechanism);
    runCommand(m_dynamicForward);
    orderCheck = inOrder(m_mechanism);

    runCommand(m_dynamicReverse);
    orderCheck.verify(m_mechanism, atLeastOnce()).drive(Volts.of(-7));
    orderCheck.verify(m_mechanism).drive(Volts.of(0));
    orderCheck.verify(m_mechanism, never()).drive(any());
  }
}
