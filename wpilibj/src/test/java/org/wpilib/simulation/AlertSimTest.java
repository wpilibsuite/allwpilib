// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Arrays;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInfo;
import org.wpilib.driverstation.Alert;
import org.wpilib.driverstation.Alert.Level;
import org.wpilib.hardware.hal.HAL;

class AlertSimTest {
  private String m_groupName;

  @BeforeEach
  void setup(TestInfo info) {
    HAL.initialize(500, 0);
    m_groupName = "AlertTest_" + info.getDisplayName();
  }

  @AfterEach
  void cleanup() {
    AlertSim.resetData();
  }

  private String[] getActiveAlerts(Level type) {
    return Arrays.stream(AlertSim.getAll())
        .filter(a -> a.isActive() && a.level == type)
        .map(a -> a.text)
        .toArray(String[]::new);
  }

  private boolean isAlertActive(String text, Alert.Level type) {
    return Arrays.stream(AlertSim.getAll())
        .filter(a -> a.isActive() && a.level == type)
        .anyMatch(a -> a.text.equals(text));
  }

  private void assertState(Alert.Level type, List<String> state) {
    assertEquals(state, Arrays.asList(getActiveAlerts(type)));
  }

  private Alert makeAlert(String text, Alert.Level type) {
    return new Alert(m_groupName, text, type);
  }

  @Test
  void testNoAlertsInitially() {
    assertEquals(0, AlertSim.getCount());
    assertEquals(0, AlertSim.getAll().length);
  }

  @Test
  void testNoAlertsAfterReset() {
    try (var alert = makeAlert("alert", Level.HIGH)) {
      alert.set(true);
      assertTrue(isAlertActive("alert", Level.HIGH));
      AlertSim.resetData();
      assertEquals(0, AlertSim.getCount());
      assertEquals(0, AlertSim.getAll().length);
    }
  }

  @Test
  void setUnsetSingle() {
    try (var one = makeAlert("one", Level.LOW)) {
      assertFalse(isAlertActive("one", Level.LOW));
      one.set(true);
      assertTrue(isAlertActive("one", Level.LOW));
      one.set(false);
      assertFalse(isAlertActive("one", Level.LOW));
    }
  }

  @Test
  void setUnsetMultiple() {
    try (var one = makeAlert("one", Level.HIGH);
        var two = makeAlert("two", Level.LOW)) {
      assertFalse(isAlertActive("one", Level.HIGH));
      assertFalse(isAlertActive("two", Level.LOW));
      one.set(true);
      assertTrue(isAlertActive("one", Level.HIGH));
      assertFalse(isAlertActive("two", Level.LOW));
      one.set(true);
      two.set(true);
      assertTrue(isAlertActive("one", Level.HIGH));
      assertTrue(isAlertActive("two", Level.LOW));
      one.set(false);
      assertFalse(isAlertActive("one", Level.HIGH));
      assertTrue(isAlertActive("two", Level.LOW));
    }
  }

  @Test
  void setIsIdempotent() {
    try (var a = makeAlert("A", Level.LOW);
        var b = makeAlert("B", Level.LOW);
        var c = makeAlert("C", Level.LOW)) {
      a.set(true);
      b.set(true);
      c.set(true);

      var startState = List.of(getActiveAlerts(Level.LOW));
      assertEquals(List.of("A", "B", "C"), startState);

      b.set(true);
      assertState(Level.LOW, startState);

      a.set(true);
      assertState(Level.LOW, startState);
    }
  }

  @Test
  void closeUnsetsAlert() {
    try (var alert = makeAlert("alert", Level.MEDIUM)) {
      alert.set(true);
      assertTrue(isAlertActive("alert", Level.MEDIUM));
    }
    assertFalse(isAlertActive("alert", Level.MEDIUM));
  }

  @Test
  void setTextWhileUnset() {
    try (var alert = makeAlert("BEFORE", Level.LOW)) {
      assertEquals("BEFORE", alert.getText());
      alert.set(true);
      assertTrue(isAlertActive("BEFORE", Level.LOW));
      alert.set(false);
      assertFalse(isAlertActive("BEFORE", Level.LOW));
      alert.setText("AFTER");
      assertEquals("AFTER", alert.getText());
      alert.set(true);
      assertFalse(isAlertActive("BEFORE", Level.LOW));
      assertTrue(isAlertActive("AFTER", Level.LOW));
    }
  }

  @Test
  void setTextWhileSet() {
    try (var alert = makeAlert("BEFORE", Level.LOW)) {
      assertEquals("BEFORE", alert.getText());
      alert.set(true);
      assertTrue(isAlertActive("BEFORE", Level.LOW));
      alert.setText("AFTER");
      assertEquals("AFTER", alert.getText());
      assertFalse(isAlertActive("BEFORE", Level.LOW));
      assertTrue(isAlertActive("AFTER", Level.LOW));
    }
  }

  @Test
  void getActive() {
    try (var a = makeAlert("A", Level.HIGH);
        var b = makeAlert("B", Level.HIGH);
        var c = makeAlert("C", Level.HIGH)) {
      a.set(true);
      b.set(true);
      c.set(false);

      var active = AlertSim.getActive();
      var all = AlertSim.getAll();
      assertEquals(2, active.length);
      assertEquals(3, all.length);
      assertTrue(Arrays.stream(active).anyMatch(x -> "A".equals(x.text)));
      assertTrue(Arrays.stream(active).anyMatch(x -> "B".equals(x.text)));

      a.set(false);
      active = AlertSim.getActive();
      all = AlertSim.getAll();
      assertEquals(1, active.length);
      assertEquals(3, all.length);
      assertEquals(active[0].text, "B");
    }
  }
}
