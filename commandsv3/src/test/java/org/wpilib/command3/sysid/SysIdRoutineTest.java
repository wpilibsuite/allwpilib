// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.sysid;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.atLeastOnce;
import static org.mockito.Mockito.clearInvocations;
import static org.mockito.Mockito.inOrder;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.wpilib.command3.sysid.SysIdRoutine.Direction.FORWARD;
import static org.wpilib.command3.sysid.SysIdRoutine.Direction.REVERSE;
import static org.wpilib.units.Units.Microseconds;
import static org.wpilib.units.Units.Second;
import static org.wpilib.units.Units.Volts;

import java.util.concurrent.atomic.AtomicLong;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.command3.Command;
import org.wpilib.command3.CommandTestBase;
import org.wpilib.command3.Mechanism;
import org.wpilib.command3.sysid.SysIdRoutine.Config;
import org.wpilib.command3.sysid.SysIdRoutine.SysIdMechanism;
import org.wpilib.sysid.SysIdRoutineLog;
import org.wpilib.system.RobotController;
import org.wpilib.units.measure.Voltage;

class SysIdRoutineTest extends CommandTestBase {
  interface TestMechanism extends Mechanism {
    void recordState(SysIdRoutineLog.State state);

    void drive(Voltage voltage);

    void log(SysIdRoutineLog log);
  }

  TestMechanism m_mechanism;
  SysIdRoutine m_sysidRoutine;
  Command m_quasistaticForward;
  Command m_quasistaticReverse;
  Command m_dynamicForward;
  Command m_dynamicReverse;
  AtomicLong m_time;

  void runCommand(Command command) {
    m_scheduler.schedule(command);
    m_scheduler.run();
    m_scheduler.run();
    m_time.addAndGet((long) Microseconds.convertFrom(1, Second));
    m_scheduler.run();
    m_scheduler.cancel(command);
  }

  @BeforeEach
  void setup() {
    m_mechanism = mock(TestMechanism.class);
    m_sysidRoutine =
        new SysIdRoutine(
            new Config(
                Config.DEFAULT_RAMP_RATE,
                Config.DEFAULT_STEP_VOLTAGE,
                Config.DEFAULT_TIMEOUT,
                m_mechanism::recordState),
            new SysIdMechanism(m_mechanism::drive, m_mechanism::log, new Mechanism() {}));
    m_quasistaticForward = m_sysidRoutine.quasistatic(FORWARD);
    m_quasistaticReverse = m_sysidRoutine.quasistatic(REVERSE);
    m_dynamicForward = m_sysidRoutine.dynamic(FORWARD);
    m_dynamicReverse = m_sysidRoutine.dynamic(REVERSE);

    m_time = new AtomicLong();
    RobotController.setTimeSource(m_time::get);
  }

  @Test
  void recordStateBookendsMotorLogging() {
    runCommand(m_quasistaticForward);

    var orderCheck = inOrder(m_mechanism);

    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.QUASISTATIC_FORWARD);
    orderCheck.verify(m_mechanism).drive(any());
    orderCheck.verify(m_mechanism).log(any());
    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.NONE);
    orderCheck.verifyNoMoreInteractions();

    clearInvocations(m_mechanism);
    orderCheck = inOrder(m_mechanism);
    runCommand(m_dynamicForward);

    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.DYNAMIC_FORWARD);
    orderCheck.verify(m_mechanism).drive(any());
    orderCheck.verify(m_mechanism).log(any());
    orderCheck.verify(m_mechanism).recordState(SysIdRoutineLog.State.NONE);
    orderCheck.verifyNoMoreInteractions();
  }

  @Test
  void testsDeclareCorrectState() {
    runCommand(m_quasistaticForward);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.QUASISTATIC_FORWARD);

    runCommand(m_quasistaticReverse);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.QUASISTATIC_REVERSE);

    runCommand(m_dynamicForward);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.DYNAMIC_FORWARD);

    runCommand(m_dynamicReverse);
    verify(m_mechanism, atLeastOnce()).recordState(SysIdRoutineLog.State.DYNAMIC_REVERSE);
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
