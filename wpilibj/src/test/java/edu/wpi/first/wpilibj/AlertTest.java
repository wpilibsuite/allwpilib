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
    assertEquals(0, getActiveAlerts(AlertType.kError).length);
    assertEquals(0, getActiveAlerts(AlertType.kWarning).length);
    assertEquals(0, getActiveAlerts(AlertType.kInfo).length);
  }

  private String getSubtableName(Alert.AlertType type) {
    switch (type) {
      case kError:
        return "errors";
      case kWarning:
        return "warnings";
      case kInfo:
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
    try (var one = makeAlert("one", AlertType.kInfo)) {
      assertFalse(isAlertActive("one", AlertType.kInfo));
      one.set(true);
      assertTrue(isAlertActive("one", AlertType.kInfo));
      one.set(false);
      assertFalse(isAlertActive("one", AlertType.kInfo));
    }
  }

  @Test
  void setUnsetMultiple() {
    try (var one = makeAlert("one", AlertType.kError);
        var two = makeAlert("two", AlertType.kInfo)) {
      assertFalse(isAlertActive("one", AlertType.kError));
      assertFalse(isAlertActive("two", AlertType.kInfo));
      one.set(true);
      assertTrue(isAlertActive("one", AlertType.kError));
      assertFalse(isAlertActive("two", AlertType.kInfo));
      one.set(true);
      two.set(true);
      assertTrue(isAlertActive("one", AlertType.kError));
      assertTrue(isAlertActive("two", AlertType.kInfo));
      one.set(false);
      assertFalse(isAlertActive("one", AlertType.kError));
      assertTrue(isAlertActive("two", AlertType.kInfo));
    }
  }

  @Test
  void setIsIdempotent() {
    try (var a = makeAlert("A", AlertType.kInfo);
        var b = makeAlert("B", AlertType.kInfo);
        var c = makeAlert("C", AlertType.kInfo)) {
      a.set(true);
      b.set(true);
      c.set(true);

      var startState = List.of(getActiveAlerts(AlertType.kInfo));

      b.set(true);
      assertState(AlertType.kInfo, startState);

      a.set(true);
      assertState(AlertType.kInfo, startState);
    }
  }

  @Test
  void closeUnsetsAlert() {
    try (var alert = makeAlert("alert", AlertType.kWarning)) {
      alert.set(true);
      assertTrue(isAlertActive("alert", AlertType.kWarning));
    }
    assertFalse(isAlertActive("alert", AlertType.kWarning));
  }

  @Test
  void setTextWhileUnset() {
    try (var alert = makeAlert("BEFORE", AlertType.kInfo)) {
      assertEquals("BEFORE", alert.getText());
      alert.set(true);
      assertTrue(isAlertActive("BEFORE", AlertType.kInfo));
      alert.set(false);
      assertFalse(isAlertActive("BEFORE", AlertType.kInfo));
      alert.setText("AFTER");
      assertEquals("AFTER", alert.getText());
      alert.set(true);
      assertFalse(isAlertActive("BEFORE", AlertType.kInfo));
      assertTrue(isAlertActive("AFTER", AlertType.kInfo));
    }
  }

  @Test
  void setTextWhileSet() {
    try (var alert = makeAlert("BEFORE", AlertType.kInfo)) {
      assertEquals("BEFORE", alert.getText());
      alert.set(true);
      assertTrue(isAlertActive("BEFORE", AlertType.kInfo));
      alert.setText("AFTER");
      assertEquals("AFTER", alert.getText());
      assertFalse(isAlertActive("BEFORE", AlertType.kInfo));
      assertTrue(isAlertActive("AFTER", AlertType.kInfo));
    }
  }

  @ResourceLock("timing")
  @Test
  void setTextDoesNotAffectFirstOrderSort() {
    SimHooks.pauseTiming();
    try (var a = makeAlert("A", AlertType.kInfo);
        var b = makeAlert("B", AlertType.kInfo);
        var c = makeAlert("C", AlertType.kInfo)) {
      a.set(true);
      SimHooks.stepTiming(1);
      b.set(true);
      SimHooks.stepTiming(1);
      c.set(true);

      var expectedEndState = new ArrayList<>(List.of(getActiveAlerts(AlertType.kInfo)));
      expectedEndState.replaceAll(s -> "B".equals(s) ? "AFTER" : s);
      b.setText("AFTER");

      assertState(AlertType.kInfo, expectedEndState);
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @ResourceLock("timing")
  @Test
  void sortOrder() {
    SimHooks.pauseTiming();
    try (var a = makeAlert("A", AlertType.kInfo);
        var b = makeAlert("B", AlertType.kInfo);
        var c = makeAlert("C", AlertType.kInfo)) {
      a.set(true);
      assertState(AlertType.kInfo, List.of("A"));
      SimHooks.stepTiming(1);
      b.set(true);
      assertState(AlertType.kInfo, List.of("B", "A"));
      SimHooks.stepTiming(1);
      c.set(true);
      assertState(AlertType.kInfo, List.of("C", "B", "A"));

      SimHooks.stepTiming(1);
      c.set(false);
      assertState(AlertType.kInfo, List.of("B", "A"));

      SimHooks.stepTiming(1);
      c.set(true);
      assertState(AlertType.kInfo, List.of("C", "B", "A"));

      SimHooks.stepTiming(1);
      a.set(false);
      assertState(AlertType.kInfo, List.of("C", "B"));

      SimHooks.stepTiming(1);
      b.set(false);
      assertState(AlertType.kInfo, List.of("C"));

      SimHooks.stepTiming(1);
      b.set(true);
      assertState(AlertType.kInfo, List.of("B", "C"));

      SimHooks.stepTiming(1);
      a.set(true);
      assertState(AlertType.kInfo, List.of("A", "B", "C"));
    } finally {
      SimHooks.resumeTiming();
    }
  }
}
