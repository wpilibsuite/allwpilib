package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringArraySubscriber;
import edu.wpi.first.wpilibj.Alert.AlertType;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.Arrays;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInfo;

public class AlertTest {
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

  String getSubtableName(Alert.AlertType type) {
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

  StringArraySubscriber getSubscriberForType(Alert.AlertType type) {
    return NetworkTableInstance.getDefault()
        .getStringArrayTopic("/SmartDashboard/" + m_groupName + "/" + getSubtableName(type))
        .subscribe(new String[] {});
  }

  String[] getActiveAlerts(AlertType type) {
    try (var sub = getSubscriberForType(type)) {
      return sub.get();
    }
  }

  void update() {
    SmartDashboard.updateValues();
  }

  boolean isAlertActive(String text, Alert.AlertType type) {
    update();
    return Arrays.asList(getActiveAlerts(type)).contains(text);
  }

  Alert makeAlert(String text, Alert.AlertType type) {
    return new Alert(m_groupName, text, type);
  }

  @Test
  void setUnset() {
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
}
