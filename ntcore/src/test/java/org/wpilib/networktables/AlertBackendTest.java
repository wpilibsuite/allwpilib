// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.util.Alert;
import org.wpilib.util.AlertDataJNI;
import org.wpilib.util.AlertException;

class AlertBackendTest {
  // The production contract allows only one installation in this process.
  @Test
  void startupAndLifetime() {
    try (var instance = NetworkTableInstance.create();
        var other = NetworkTableInstance.create()) {
      AlertBackend.install(instance, "coprocessor//alerts/");
      var textTopic = instance.getStringTopic("/coprocessor/alerts/group/1/id/text");
      var activeTopic = instance.getIntegerTopic("/coprocessor/alerts/group/1/id/active");
      try (var text = textTopic.subscribe("");
          var active =
              activeTopic.subscribe(-1, PubSubOption.KEEP_DUPLICATES, PubSubOption.SEND_ALL);
          var alert = new Alert("group", "id", "initial", Alert.Level.MEDIUM)) {
        assertFalse(alert.get());
        assertEquals("initial", text.get());
        assertEquals(0, active.get());
        assertFalse(other.getTopic(textTopic.getName()).exists());
        assertThrows(
            AlertException.class, () -> new Alert("group", "id", "duplicate", Alert.Level.MEDIUM));
        alert.setText("inactive edit");
        assertEquals("inactive edit", text.get());
        alert.set(true);
        long activation = active.get();
        assertTrue(activation > 0);
        alert.set(true);
        alert.setText("active edit");
        assertEquals(activation, active.get());
        assertEquals("active edit", text.get());
        assertEquals(activation, AlertDataJNI.getAlerts()[0].activeStartTime);
        alert.set(false);
        assertEquals(0, active.get());
        var queue = active.readQueue();
        assertEquals(3, queue.length);
        assertEquals(activation, queue[1].value);
        assertEquals(0, queue[2].value);
        AlertDataJNI.resetData();
        try (var current = new Alert("group", "id", "new", Alert.Level.MEDIUM)) {
          assertThrows(AlertException.class, () -> alert.setText("stale"));
          assertEquals("new", current.getText());
        }
      }
      assertFalse(textTopic.exists());
      try (var shutdown = new Alert("shutdown", "id", "text", Alert.Level.LOW)) {
        instance.close();
        assertThrows(AlertException.class, () -> shutdown.set(true));
        assertThrows(AlertException.class, () -> new Alert("new", "text", Alert.Level.LOW));
      }
    }
  }
}
