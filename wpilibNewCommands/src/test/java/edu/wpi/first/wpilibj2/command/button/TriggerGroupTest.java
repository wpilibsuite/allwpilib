package edu.wpi.first.wpilibj2.command.button;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj2.command.CommandTestBase;
import java.util.Map;
import org.junit.jupiter.api.Test;

public class TriggerGroupTest extends CommandTestBase {
  @Test
  void addTest() {
    TriggerGroup group = new TriggerGroup();

    TriggerGroup newGroup = assertDoesNotThrow(() -> group.add("a", () -> false));

    assertNotEquals(group, newGroup);

    assertThrows(IllegalArgumentException.class, () -> newGroup.add("a", () -> true));

    assertDoesNotThrow(() -> newGroup.only("a"));
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
    TriggerGroup group1 =
        new TriggerGroup(Map.of("a", () -> true, "b", () -> true, "c", () -> false));
    TriggerGroup group2 =
        new TriggerGroup(Map.of("a", () -> true, "b", () -> true, "c", () -> true));

    assertFalse(group1.all());
    assertTrue(group2.all());
  }

  @Test
  void noneTest() {
    TriggerGroup group1 =
        new TriggerGroup(Map.of("a", () -> false, "b", () -> true, "c", () -> false));
    TriggerGroup group2 =
        new TriggerGroup(Map.of("a", () -> false, "b", () -> false, "c", () -> false));

    assertFalse(group1.none());
    assertTrue(group2.none());
  }

  @Test
  void invalidNameTest() {
    TriggerGroup group = new TriggerGroup(Map.of("a", () -> false));

    assertThrows(IllegalArgumentException.class, () -> group.only("bogus"));
  }
}
