// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class StagedCommandBuilderTest {
  private static final Runnable no_op = () -> {};

  private Mechanism m_mech1;
  private Mechanism m_mech2;

  @BeforeEach
  void setUp() {
    Scheduler scheduler = Scheduler.createIndependentScheduler();
    m_mech1 = new Mechanism("Mech 1", scheduler);
    m_mech2 = new Mechanism("Mech 2", scheduler);
  }

  // The next two tests are to check that various forms of builder usage are able to compile.

  @Test
  void streamlined() {
    Command command =
        new StagedCommandBuilder()
            .noRequirements()
            .executing(Coroutine::park)
            .until(() -> false)
            .named("Name");

    assertEquals("Name", command.name());
  }

  @Test
  void allOptions() {
    var mech = new Mechanism("Mech", Scheduler.createIndependentScheduler());

    Command command =
        new StagedCommandBuilder()
            .noRequirements()
            .requiring(mech)
            .requiring(mech, mech)
            .requiring(List.of(mech))
            .executing(Coroutine::park)
            .whenCanceled(no_op)
            .until(() -> false)
            .withPriority(10)
            .named("Name");

    assertEquals("Name", command.name());
  }

  @Test
  void starting_noRequirements_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var ignored = builder.noRequirements().executing(c -> {}).named("cmd");

    var err = assertThrows(IllegalStateException.class, builder::noRequirements);
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void starting_requiringVarargs_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var ignored = builder.noRequirements().executing(c -> {}).named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> builder.requiring(m_mech1, m_mech2));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void starting_requiringCollection_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var ignored = builder.noRequirements().executing(c -> {}).named("cmd");

    var err =
        assertThrows(
            IllegalStateException.class, () -> builder.requiring(List.of(m_mech1, m_mech2)));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void requirements_requiringSingle_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var reqStage = builder.noRequirements();
    var ignored = reqStage.executing(c -> {}).named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> reqStage.requiring(m_mech1));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void requirements_requiringVarargs_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var reqStage = builder.noRequirements();
    var ignored = reqStage.executing(Coroutine::park).named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> reqStage.requiring(m_mech1, m_mech2));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void requirements_requiringCollection_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var reqStage = builder.noRequirements();
    var ignored = reqStage.executing(Coroutine::park).named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> reqStage.requiring(List.of(m_mech1)));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void requirements_executing_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var reqStage = builder.noRequirements();
    var ignored = reqStage.executing(c -> {}).named("cmd");

    Consumer<Coroutine> impl = Coroutine::park;
    var err = assertThrows(IllegalStateException.class, () -> reqStage.executing(impl));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void execution_whenCanceled_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var execStage = builder.noRequirements().executing(c -> {});
    var ignored = execStage.named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> execStage.whenCanceled(() -> {}));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void execution_withPriority_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var execStage = builder.noRequirements().executing(c -> {});
    var ignored = execStage.named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> execStage.withPriority(7));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void execution_until_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var execStage = builder.noRequirements().executing(c -> {});
    var ignored = execStage.named("cmd");

    BooleanSupplier endCondition = () -> true;
    var err = assertThrows(IllegalStateException.class, () -> execStage.until(endCondition));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void execution_named_throwsAfterBuild() {
    var builder = new StagedCommandBuilder();
    var execStage = builder.noRequirements().executing(c -> {});
    var ignored = execStage.named("cmd");

    var err = assertThrows(IllegalStateException.class, () -> execStage.named("other"));
    assertEquals("Command builders cannot be reused", err.getMessage());
  }

  @Test
  void starting_requiringVarargs_nullFirstRequirement_throwsNPE() {
    var builder = new StagedCommandBuilder();
    assertThrows(NullPointerException.class, () -> builder.requiring(null, m_mech2));
  }

  @Test
  void starting_requiringVarargs_nullArray_throwsNPE() {
    var builder = new StagedCommandBuilder();
    assertThrows(NullPointerException.class, () -> builder.requiring(m_mech1, (Mechanism[]) null));
  }

  @Test
  void starting_requiringVarargs_nullInExtra_throwsNPE() {
    var builder = new StagedCommandBuilder();
    assertThrows(NullPointerException.class, () -> builder.requiring(m_mech1, m_mech2, null));
  }

  @Test
  void starting_requiringCollection_nullCollection_throwsNPE() {
    var builder = new StagedCommandBuilder();
    assertThrows(NullPointerException.class, () -> builder.requiring((Collection<Mechanism>) null));
  }

  @Test
  void starting_requiringCollection_nullElement_throwsNPE() {
    var builder = new StagedCommandBuilder();
    var listWithNull = Arrays.asList(m_mech1, null, m_mech2); // Arrays.asList allows nulls
    assertThrows(NullPointerException.class, () -> builder.requiring(listWithNull));
  }

  @Test
  void requirements_requiringSingle_null_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring((Mechanism) null));
  }

  @Test
  void requirements_requiringVarargs_nullFirstRequirement_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring(null, m_mech2));
  }

  @Test
  void requirements_requiringVarargs_nullArray_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring(m_mech1, (Mechanism[]) null));
  }

  @Test
  void requirements_requiringVarargs_nullInExtra_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring(m_mech1, m_mech2, null));
  }

  @Test
  void requirements_requiringCollection_nullCollection_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    assertThrows(NullPointerException.class, () -> req.requiring((Collection<Mechanism>) null));
  }

  @Test
  void requirements_requiringCollection_nullElement_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    var listWithNull = Arrays.asList(m_mech1, null); // Arrays.asList allows nulls
    assertThrows(NullPointerException.class, () -> req.requiring(listWithNull));
  }

  @Test
  void requirements_executing_nullImpl_throwsNPE() {
    var req = new StagedCommandBuilder().noRequirements();
    assertThrows(NullPointerException.class, () -> req.executing(null));
  }

  @Test
  void execution_named_nullName_throwsNPE() {
    var exec = new StagedCommandBuilder().noRequirements().executing(c -> {});
    assertThrows(NullPointerException.class, () -> exec.named(null));
  }
}
