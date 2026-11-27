// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class StagedCommandBuilderTest {
  private static final Runnable no_op = () -> {};

  private Mechanism m_mech1;
  private Mechanism m_mech2;

  @BeforeEach
  void setUp() {
    Scheduler scheduler = Scheduler.createIndependentScheduler();
    m_mech1 = new DummyMechanism("Mech 1", scheduler);
    m_mech2 = new DummyMechanism("Mech 2", scheduler);
  }

  // The next two tests are to check that various forms of builder usage are able to compile.

  @Test
  void streamlined() {
    Command command =
        StagedCommandBuilder.noRequirements()
            .executing(Coroutine::park)
            .until(() -> false)
            .named("Name");

    assertEquals("Name", command.name());
  }

  @Test
  void allOptions() {
    var mech = new DummyMechanism("Mech", Scheduler.createIndependentScheduler());

    Command command =
        StagedCommandBuilder.noRequirements()
            .requiring(mech)
            .requiring(mech, mech)
            .requiring(List.of(mech))
            .executing(Coroutine::park)
            .whenCanceled(no_op)
            .whenExited(no_op)
            .until(() -> false)
            .withPriority(10)
            .named("Name");

    assertEquals("Name", command.name());
  }

  @Test
  void stagesAreReusable() {
    var base = StagedCommandBuilder.requiring(m_mech1).executing(Coroutine::park);

    var low = base.withPriority(1).named("Low");
    var high = base.withPriority(5).named("High");
    var again = base.named("Again");

    assertEquals(1, low.priority());
    assertEquals(5, high.priority());
    assertEquals(Command.DEFAULT_PRIORITY, again.priority());
    assertEquals(Set.of(m_mech1), high.requirements());
  }

  @Test
  void requirementStagesDoNotShareState() {
    var base = StagedCommandBuilder.requiring(m_mech1);
    var extended = base.requiring(m_mech2);

    assertEquals(Set.of(m_mech1), base.executing(Coroutine::park).named("Base").requirements());
    assertEquals(
        Set.of(m_mech1, m_mech2), extended.executing(Coroutine::park).named("Ext").requirements());
  }

  @Test
  void nullCallbacksAreTreatedAsNoOps() {
    var command =
        StagedCommandBuilder.noRequirements()
            .executing(Coroutine::park)
            .whenCanceled(null)
            .whenExited(null)
            .until(null)
            .named("Name");

    command.onCancel();
    command.onExit();
    assertEquals("Name", command.name());
  }

  @Test
  void starting_requiringVarargs_nullFirstRequirement_throwsNPE() {
    assertThrows(NullPointerException.class, () -> StagedCommandBuilder.requiring(null, m_mech2));
  }

  @Test
  void starting_requiringVarargs_nullArray_throwsNPE() {
    assertThrows(
        NullPointerException.class,
        () -> StagedCommandBuilder.requiring(m_mech1, (Mechanism[]) null));
  }

  @Test
  void starting_requiringVarargs_nullInExtra_throwsNPE() {
    assertThrows(
        NullPointerException.class, () -> StagedCommandBuilder.requiring(m_mech1, m_mech2, null));
  }

  @Test
  void starting_requiringCollection_nullCollection_throwsNPE() {
    assertThrows(
        NullPointerException.class,
        () -> StagedCommandBuilder.requiring((Collection<Mechanism>) null));
  }

  @Test
  void starting_requiringCollection_nullElement_throwsNPE() {
    var listWithNull = Arrays.asList(m_mech1, null, m_mech2); // Arrays.asList allows nulls
    assertThrows(NullPointerException.class, () -> StagedCommandBuilder.requiring(listWithNull));
  }

  @Test
  void requirements_requiringSingle_null_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring((Mechanism) null));
  }

  @Test
  void requirements_requiringVarargs_nullFirstRequirement_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring(null, m_mech2));
  }

  @Test
  void requirements_requiringVarargs_nullArray_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring(m_mech1, (Mechanism[]) null));
  }

  @Test
  void requirements_requiringVarargs_nullInExtra_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring(m_mech1, m_mech2, null));
  }

  @Test
  void requirements_requiringCollection_nullCollection_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring((Collection<Mechanism>) null));
  }

  @Test
  void requirements_requiringCollection_nullElement_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    var listWithNull = Arrays.asList(m_mech1, null); // Arrays.asList allows nulls
    assertThrows(NullPointerException.class, () -> req.requiring(listWithNull));
  }

  @Test
  void requirements_executing_nullImpl_throwsNPE() {
    var req = StagedCommandBuilder.noRequirements();
    assertThrows(NullPointerException.class, () -> req.executing(null));
  }

  @Test
  void execution_named_nullName_throwsNPE() {
    var exec = StagedCommandBuilder.noRequirements().executing(c -> {});
    assertThrows(NullPointerException.class, () -> exec.named(null));
  }
}
