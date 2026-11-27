// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj2.command.CommandTestBase;
import java.util.Map;
import org.junit.jupiter.api.Test;

class TriggerGroupTest extends CommandTestBase {
  @Test
  void addTest() {
    TriggerGroup group1 = new TriggerGroup();

    TriggerGroup group2 = assertDoesNotThrow(() -> group1.add("a", () -> false));

    assertNotEquals(group1, group2);

    assertThrows(
        IllegalArgumentException.class,
        () -> {
          @SuppressWarnings("unused")
          TriggerGroup group3 = group2.add("a", () -> true);
        });

    assertDoesNotThrow(() -> group2.only("a"));
  }

  @Test
  void onlyTest() {
    TriggerGroup group =
        new TriggerGroup(Map.of("a", () -> true, "b", () -> true, "c", () -> false));

    assertTrue(group.only("a", "b"));
    assertFalse(group.only("a"));
    assertFalse(group.only("a", "c"));
    assertFalse(group.only("a", "b", "c"));
  }

  @Test
  void anyTest() {
    TriggerGroup group =
        new TriggerGroup(Map.of("a", () -> true, "b", () -> true, "c", () -> false));

    assertTrue(group.any("a", "b"));
    assertTrue(group.any("a", "c"));
    assertFalse(group.any("c"));
  }

  @Test
  void allOfTest() {
    TriggerGroup group =
        new TriggerGroup(Map.of("a", () -> true, "b", () -> true, "c", () -> false));

    assertTrue(group.allOf("a", "b"));
    assertTrue(group.allOf("a"));
    assertFalse(group.allOf("c"));
  }

  @Test
  void allTest() {
    TriggerGroup group1 = new TriggerGroup(Map.of("a", () -> true, "b", () -> false));
    TriggerGroup group2 = new TriggerGroup(Map.of("a", () -> true, "b", () -> true));

    assertFalse(group1.all());
    assertTrue(group2.all());
  }

  @Test
  void noneTest() {
    TriggerGroup group1 = new TriggerGroup(Map.of("a", () -> true, "b", () -> false));
    TriggerGroup group2 = new TriggerGroup(Map.of("a", () -> false, "b", () -> false));

    assertFalse(group1.none());
    assertTrue(group2.none());
  }

  @Test
  void invalidNameTest() {
    TriggerGroup group = new TriggerGroup(Map.of("a", () -> false));

    assertThrows(IllegalArgumentException.class, () -> group.only("bogus"));
  }
}
