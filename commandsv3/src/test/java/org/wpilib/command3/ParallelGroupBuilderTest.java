// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Set;
import org.junit.jupiter.api.Test;

class ParallelGroupBuilderTest {
  @Test
  void optionalVarargNullArray() {
    var builder = new ParallelGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.optional((Command[]) null));
  }

  @Test
  void optionalNullInVarargs() {
    var builder = new ParallelGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.optional((Command) null));
  }

  @Test
  void optionalPopulatesOptionalOnly() {
    var a = Command.noRequirements().executing(Coroutine::park).named("A");
    var b = Command.noRequirements().executing(Coroutine::park).named("B");

    var group = new ParallelGroupBuilder().optional(a, b).withAutomaticName();

    assertEquals(Set.of(a, b), group.getOptionalCommands());
    assertTrue(group.getRequiredCommands().isEmpty());
    assertEquals("(A | B)", group.name());
  }

  @Test
  void requiringVarargNullArray() {
    var builder = new ParallelGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.requiring((Command[]) null));
  }

  @Test
  void requiringNullInVarargs() {
    var builder = new ParallelGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.requiring((Command) null));
  }

  @Test
  void requiringPopulatesRequiredOnly() {
    var a = Command.noRequirements().executing(Coroutine::park).named("A");
    var b = Command.noRequirements().executing(Coroutine::park).named("B");

    var group = new ParallelGroupBuilder().requiring(a, b).withAutomaticName();

    assertEquals(Set.of(a, b), group.getRequiredCommands());
    assertTrue(group.getOptionalCommands().isEmpty());
    assertEquals("(A & B)", group.name());
  }

  @Test
  void mixedRequiredAndOptional() {
    var reqA = Command.noRequirements().executing(Coroutine::park).named("ReqA");
    var reqB = Command.noRequirements().executing(Coroutine::park).named("ReqB");
    var optX = Command.noRequirements().executing(Coroutine::park).named("OptX");
    var optY = Command.noRequirements().executing(Coroutine::park).named("OptY");

    var group =
        new ParallelGroupBuilder().requiring(reqA, reqB).optional(optX, optY).withAutomaticName();

    assertEquals(Set.of(reqA, reqB), group.getRequiredCommands());
    assertEquals(Set.of(optX, optY), group.getOptionalCommands());
    assertEquals("[(ReqA & ReqB) * (OptX | OptY)]", group.name());
  }

  @Test
  void namedWithNull() {
    var builder = new ParallelGroupBuilder();
    assertThrows(NullPointerException.class, () -> builder.named(null));
  }
}
