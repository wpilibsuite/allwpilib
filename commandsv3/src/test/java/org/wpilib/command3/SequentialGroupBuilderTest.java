// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.List;
import org.junit.jupiter.api.Test;

class SequentialGroupBuilderTest {
  @Test
  void andThenSingle() {
    var c1 = Command.noRequirements().executing(Coroutine::park).named("C1");

    var builder = new SequentialGroupBuilder();
    var sequence = builder.andThen(c1).named("Seq");
    assertInstanceOf(SequentialGroup.class, sequence);
    assertEquals(List.of(c1), ((SequentialGroup) sequence).getCommands());
    assertEquals("Seq", sequence.name());
  }

  @Test
  void andThenMultiple() {
    var c1 = Command.noRequirements().executing(Coroutine::park).named("C1");
    var c2 = Command.noRequirements().executing(Coroutine::park).named("C2");

    var builder = new SequentialGroupBuilder();
    var sequence = builder.andThen(c1, c2).named("Seq");

    assertInstanceOf(SequentialGroup.class, sequence);
    assertEquals(List.of(c1, c2), ((SequentialGroup) sequence).getCommands());
    assertEquals("Seq", sequence.name());
  }

  @Test
  void andThenRepeated() {
    var c1 = Command.noRequirements().executing(Coroutine::park).named("C1");
    var c2 = Command.noRequirements().executing(Coroutine::park).named("C2");

    var builder = new SequentialGroupBuilder();
    var sequence = builder.andThen(c1).andThen(c2).named("Seq");

    assertInstanceOf(SequentialGroup.class, sequence);
    assertEquals(List.of(c1, c2), ((SequentialGroup) sequence).getCommands());
    assertEquals("Seq", sequence.name());
  }

  @Test
  void andThenSingleNull() {
    var builder = new SequentialGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.andThen((Command) null));
  }

  @Test
  void andThenThenVarargNullArray() {
    var builder = new SequentialGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.andThen((Command[]) null));
  }

  @Test
  void andThenNullInVarargs() {
    var builder = new SequentialGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.andThen((Command) null));
  }

  @Test
  void untilReturnsParallelGroup() {
    var c1 = Command.noRequirements().executing(Coroutine::park).named("C1");
    var builder = new SequentialGroupBuilder();
    var sequence = builder.andThen(c1).until(() -> false).named("Seq");
    assertInstanceOf(ParallelGroup.class, sequence);
    assertEquals("Seq", sequence.name());
  }

  @Test
  void namedWithNull() {
    var builder = new SequentialGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.named(null));
  }

  @Test
  void automaticNameWithEmptyGroup() {
    var builder = new SequentialGroupBuilder();
    var sequence = builder.withAutomaticName();
    assertEquals("", sequence.name());
  }

  @Test
  void automaticNameWithOneCommand() {
    var c1 = Command.noRequirements().executing(Coroutine::park).named("C1");
    var builder = new SequentialGroupBuilder();
    var sequence = builder.andThen(c1).withAutomaticName();
    assertEquals("C1", sequence.name());
  }

  @Test
  void automaticNameWithMultipleCommands() {
    var c1 = Command.noRequirements().executing(Coroutine::park).named("C1");
    var c2 = Command.noRequirements().executing(Coroutine::park).named("C2");
    var builder = new SequentialGroupBuilder();
    var sequence = builder.andThen(c1, c2).withAutomaticName();
    assertEquals("C1 -> C2", sequence.name());
  }
}
