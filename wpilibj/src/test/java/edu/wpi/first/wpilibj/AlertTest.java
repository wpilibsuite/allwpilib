// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringArraySubscriber;
import edu.wpi.first.wpilibj.Alert.AlertType;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInfo;
import org.junit.jupiter.api.parallel.ResourceLock;

class AlertTest {
  String m_groupName;

  @BeforeEach
  void setup(TestInfo info) {
    m_groupName = "AlertTest_" + info.getDisplayName();
  }

  @AfterEach
  void checkClean() {
    update();
    assertEquals(0, getActiveAlerts(AlertType.ERROR).length);
    assertEquals(0, getActiveAlerts(AlertType.WARNING).length);
    assertEquals(0, getActiveAlerts(AlertType.INFO).length);
  }

  private String getSubtableName(Alert.AlertType type) {
    switch (type) {
      case ERROR:
        return "errors";
      case WARNING:
        return "warnings";
      case INFO:
        return "infos";
      default:
        return "unknown";
    }
  }

  private StringArraySubscriber getSubscriberForType(Alert.AlertType type) {
    return NetworkTableInstance.getDefault()
        .getStringArrayTopic("/SmartDashboard/" + m_groupName + "/" + getSubtableName(type))
        .subscribe(new String[] {});
  }

  private String[] getActiveAlerts(AlertType type) {
    update();
    try (var sub = getSubscriberForType(type)) {
      return sub.get();
    }
  }

  private void update() {
    SmartDashboard.updateValues();
  }

  private boolean isAlertActive(String text, Alert.AlertType type) {
    return Arrays.asList(getActiveAlerts(type)).contains(text);
  }

  private void assertState(Alert.AlertType type, List<String> state) {
    assertEquals(state, Arrays.asList(getActiveAlerts(type)));
  }

  private Alert makeAlert(String text, Alert.AlertType type) {
    return new Alert(m_groupName, text, type);
  }

  @Test
  void setUnsetSingle() {
    try (var one = makeAlert("one", AlertType.INFO)) {
      assertFalse(isAlertActive("one", AlertType.INFO));
      one.set(true);
      assertTrue(isAlertActive("one", AlertType.INFO));
      one.set(false);
      assertFalse(isAlertActive("one", AlertType.INFO));
    }
  }

  @Test
  void setUnsetMultiple() {
    try (var one = makeAlert("one", AlertType.ERROR);
        var two = makeAlert("two", AlertType.INFO)) {
      assertFalse(isAlertActive("one", AlertType.ERROR));
      assertFalse(isAlertActive("two", AlertType.INFO));
      one.set(true);
      assertTrue(isAlertActive("one", AlertType.ERROR));
      assertFalse(isAlertActive("two", AlertType.INFO));
      one.set(true);
      two.set(true);
      assertTrue(isAlertActive("one", AlertType.ERROR));
      assertTrue(isAlertActive("two", AlertType.INFO));
      one.set(false);
      assertFalse(isAlertActive("one", AlertType.ERROR));
      assertTrue(isAlertActive("two", AlertType.INFO));
    }
  }

  @Test
  void setIsIdempotent() {
    try (var a = makeAlert("A", AlertType.INFO);
        var b = makeAlert("B", AlertType.INFO);
        var c = makeAlert("C", AlertType.INFO)) {
      a.set(true);
      b.set(true);
      c.set(true);

      var startState = List.of(getActiveAlerts(AlertType.INFO));

      b.set(true);
      assertState(AlertType.INFO, startState);

      a.set(true);
      assertState(AlertType.INFO, startState);
    }
  }

  @Test
  void closeUnsetsAlert() {
    try (var alert = makeAlert("alert", AlertType.WARNING)) {
      alert.set(true);
      assertTrue(isAlertActive("alert", AlertType.WARNING));
    }
    assertFalse(isAlertActive("alert", AlertType.WARNING));
  }

  @Test
  void setTextWhileUnset() {
    try (var alert = makeAlert("BEFORE", AlertType.INFO)) {
      assertEquals("BEFORE", alert.getText());
      alert.set(true);
      assertTrue(isAlertActive("BEFORE", AlertType.INFO));
      alert.set(false);
      assertFalse(isAlertActive("BEFORE", AlertType.INFO));
      alert.setText("AFTER");
      assertEquals("AFTER", alert.getText());
      alert.set(true);
      assertFalse(isAlertActive("BEFORE", AlertType.INFO));
      assertTrue(isAlertActive("AFTER", AlertType.INFO));
    }
  }

  @Test
  void setTextWhileSet() {
    try (var alert = makeAlert("BEFORE", AlertType.INFO)) {
      assertEquals("BEFORE", alert.getText());
      alert.set(true);
      assertTrue(isAlertActive("BEFORE", AlertType.INFO));
      alert.setText("AFTER");
      assertEquals("AFTER", alert.getText());
      assertFalse(isAlertActive("BEFORE", AlertType.INFO));
      assertTrue(isAlertActive("AFTER", AlertType.INFO));
    }
  }

  @ResourceLock("timing")
  @Test
  void setTextDoesNotAffectFirstOrderSort() {
    SimHooks.pauseTiming();
    try (var a = makeAlert("A", AlertType.INFO);
        var b = makeAlert("B", AlertType.INFO);
        var c = makeAlert("C", AlertType.INFO)) {
      a.set(true);
      SimHooks.stepTiming(1);
      b.set(true);
      SimHooks.stepTiming(1);
      c.set(true);

      var expectedEndState = new ArrayList<>(List.of(getActiveAlerts(AlertType.INFO)));
      expectedEndState.replaceAll(s -> "B".equals(s) ? "AFTER" : s);
      b.setText("AFTER");

      assertState(AlertType.INFO, expectedEndState);
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @ResourceLock("timing")
  @Test
  void sortOrder() {
    SimHooks.pauseTiming();
    try (var a = makeAlert("A", AlertType.INFO);
        var b = makeAlert("B", AlertType.INFO);
        var c = makeAlert("C", AlertType.INFO)) {
      a.set(true);
      assertState(AlertType.INFO, List.of("A"));
      SimHooks.stepTiming(1);
      b.set(true);
      assertState(AlertType.INFO, List.of("B", "A"));
      SimHooks.stepTiming(1);
      c.set(true);
      assertState(AlertType.INFO, List.of("C", "B", "A"));

      SimHooks.stepTiming(1);
      c.set(false);
      assertState(AlertType.INFO, List.of("B", "A"));

      SimHooks.stepTiming(1);
      c.set(true);
      assertState(AlertType.INFO, List.of("C", "B", "A"));

      SimHooks.stepTiming(1);
      a.set(false);
      assertState(AlertType.INFO, List.of("C", "B"));

      SimHooks.stepTiming(1);
      b.set(false);
      assertState(AlertType.INFO, List.of("C"));

      SimHooks.stepTiming(1);
      b.set(true);
      assertState(AlertType.INFO, List.of("B", "C"));

      SimHooks.stepTiming(1);
      a.set(true);
      assertState(AlertType.INFO, List.of("A", "B", "C"));
    } finally {
      SimHooks.resumeTiming();
    }
  }
}
