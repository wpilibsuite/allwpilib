// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Arrays;
import java.util.Set;
import java.util.stream.Collectors;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;

class AlertTest {
  @AfterEach
  void resetAlerts() {
    AlertDataJNI.resetData();
  }

  @Test
  void setGetTextAndLevel() {
    try (Alert alert = new Alert("group", "id", "initial", Alert.Level.HIGH)) {
      assertEquals(1, AlertDataJNI.getNumAlerts());
      assertFalse(alert.get());
      assertEquals("initial", alert.getText());
      assertEquals(Alert.Level.HIGH, alert.getLevel());

      alert.set(true);
      assertTrue(alert.get());

      AlertDataJNI.AlertInfo[] infos = AlertDataJNI.getAlerts();
      assertEquals(1, infos.length);
      assertAll(
          () -> assertEquals("group", infos[0].group),
          () -> assertEquals("id", infos[0].id),
          () -> assertEquals("initial", infos[0].text),
          () -> assertNotEquals(0, infos[0].activeStartTime),
          () -> assertEquals(Alert.Level.HIGH.getValue(), infos[0].level));

      alert.setText("updated");
      assertEquals("updated", alert.getText());
      alert.set(false);
      assertFalse(alert.get());
    }

    assertEquals(0, AlertDataJNI.getNumAlerts());
  }

  @Test
  void defaultGroupAndCloseRemoveAlert() {
    try (Alert alert = new Alert("id", "text", Alert.Level.LOW)) {
      AlertDataJNI.AlertInfo[] infos = AlertDataJNI.getAlerts();
      assertEquals(1, infos.length);
      assertAll(
          () -> assertEquals("Alerts", infos[0].group),
          () -> assertEquals("id", infos[0].id),
          () -> assertEquals("text", infos[0].text),
          () -> assertEquals(Alert.Level.LOW.getValue(), infos[0].level));
    }

    assertEquals(0, AlertDataJNI.getNumAlerts());
  }

  @Test
  void duplicateIdsOnlyConflictWithinSameLevel() {
    try (Alert high = new Alert("group", "id", "high", Alert.Level.HIGH);
        Alert low = new Alert("group", "id", "low", Alert.Level.LOW)) {
      assertEquals(2, AlertDataJNI.getNumAlerts());

      assertThrows(
          IllegalArgumentException.class,
          () -> new Alert("group", "id", "duplicate", Alert.Level.HIGH));

      AlertDataJNI.AlertInfo[] infos = AlertDataJNI.getAlerts();
      assertEquals(2, infos.length);
      Set<Integer> levels =
          Arrays.stream(infos).map(info -> info.level).collect(Collectors.toSet());
      assertEquals(Set.of(Alert.Level.HIGH.getValue(), Alert.Level.LOW.getValue()), levels);
    }
  }

  @Test
  void resetDataClearsAlerts() {
    try (Alert alert = new Alert("group", "id", "text", Alert.Level.MEDIUM)) {
      alert.set(true);
      assertEquals(1, AlertDataJNI.getNumAlerts());

      AlertDataJNI.resetData();

      assertEquals(0, AlertDataJNI.getNumAlerts());
      assertEquals(0, AlertDataJNI.getAlerts().length);
      assertThrows(IllegalArgumentException.class, () -> alert.get());
    }
  }

  @Test
  void getActiveAndAllAlertsThroughDataJni() {
    try (Alert a = new Alert("group", "A", "A", Alert.Level.HIGH);
        Alert b = new Alert("group", "B", "B", Alert.Level.HIGH);
        Alert c = new Alert("group", "C", "C", Alert.Level.MEDIUM)) {
      a.set(true);
      b.set(true);
      c.set(false);

      AlertDataJNI.AlertInfo[] infos = AlertDataJNI.getAlerts();
      assertEquals(3, infos.length);
      assertEquals(
          Set.of("A", "B", "C"),
          Arrays.stream(infos).map(info -> info.id).collect(Collectors.toSet()));

      Set<String> activeIds =
          Arrays.stream(infos)
              .filter(info -> info.activeStartTime != 0)
              .map(info -> info.id)
              .collect(Collectors.toSet());
      assertEquals(Set.of("A", "B"), activeIds);
    }
  }
}
