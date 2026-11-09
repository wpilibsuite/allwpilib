// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation.internal;

import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.driverstation.RobotState;
import org.wpilib.util.Alert;
import org.wpilib.util.AlertException;

final class JoystickAlerts {
  enum JoystickResourceAlert {
    BUTTON("ButtonUnavailable", "Button"),
    AXIS("AxisUnavailable", "axis"),
    POV("POVUnavailable", "POV");

    final String idSuffix;
    final String label;

    JoystickResourceAlert(String idSuffix, String label) {
      this.idSuffix = idSuffix;
      this.label = label;
    }

    boolean isAvailable(int stick, int resource) {
      return switch (this) {
        case BUTTON ->
            (DriverStationBackend.getStickButtonsAvailable(stick) & (1L << resource)) != 0;
        case AXIS -> (DriverStationBackend.getStickAxesAvailable(stick) & (1 << resource)) != 0;
        case POV -> (DriverStationBackend.getStickPOVsAvailable(stick) & (1 << resource)) != 0;
      };
    }

    String getText(int stick, int resource) {
      return "Joystick " + label + " " + resource + " on port " + stick + " not available";
    }
  }

  private static final ReentrantLock m_joystickAlertMutex = new ReentrantLock();
  private static boolean m_silenceJoystickAlerts;
  private static final JoystickAlerts[] m_joystickAlerts =
      new JoystickAlerts[DriverStationBackend.JOYSTICK_PORTS];
  private static final JoystickResourceAlert[] JOYSTICK_RESOURCE_ALERTS =
      JoystickResourceAlert.values();

  private static final class JoystickAlertState {
    Alert alert;
    boolean active;
  }

  private final int m_stick;
  private final String m_connectionText;
  private final JoystickAlertState m_connectionAlert = new JoystickAlertState();
  private final JoystickAlertState[] m_resourceAlerts =
      new JoystickAlertState[JOYSTICK_RESOURCE_ALERTS.length];
  private final JoystickAlertState m_touchpadFingerAlert = new JoystickAlertState();
  private final int[] m_resourceAlertValues = new int[JOYSTICK_RESOURCE_ALERTS.length];
  private int m_touchpadFingerAlertTouchpad;
  private int m_touchpadFingerAlertFinger;
  private boolean m_initialized;

  JoystickAlerts(int stick) {
    this.m_stick = stick;
    m_connectionText =
        "Joystick on port " + stick + " not available, check if controller is plugged in";
    for (int i = 0; i < m_resourceAlerts.length; i++) {
      m_resourceAlerts[i] = new JoystickAlertState();
    }
    initializeAlerts();
  }

  boolean initializeAlerts() {
    closeAlerts();

    Alert newConnectionAlert = null;
    Alert[] newResourceAlerts = new Alert[JOYSTICK_RESOURCE_ALERTS.length];
    Alert newTouchpadFingerAlert = null;

    try {
      newConnectionAlert =
          new Alert(
              "DriverStation",
              "joystick" + m_stick + "Disconnected",
              m_connectionText,
              Alert.Level.HIGH);
      for (JoystickResourceAlert resourceAlert : JOYSTICK_RESOURCE_ALERTS) {
        newResourceAlerts[resourceAlert.ordinal()] =
            new Alert(
                "DriverStation",
                "joystick" + m_stick + resourceAlert.idSuffix,
                "",
                Alert.Level.MEDIUM);
      }
      newTouchpadFingerAlert =
          new Alert(
              "DriverStation",
              "joystick" + m_stick + "TouchpadFingerUnavailable",
              "",
              Alert.Level.MEDIUM);
    } catch (AlertException ex) {
      closeAlert(newConnectionAlert);
      for (Alert alert : newResourceAlerts) {
        closeAlert(alert);
      }
      closeAlert(newTouchpadFingerAlert);
      m_initialized = false;
      return false;
    }

    m_connectionAlert.alert = newConnectionAlert;
    for (JoystickResourceAlert resourceAlert : JOYSTICK_RESOURCE_ALERTS) {
      m_resourceAlerts[resourceAlert.ordinal()].alert = newResourceAlerts[resourceAlert.ordinal()];
    }
    m_touchpadFingerAlert.alert = newTouchpadFingerAlert;
    m_initialized = true;
    return true;
  }

  void closeAlerts() {
    closeAlert(m_connectionAlert);
    for (JoystickAlertState alertState : m_resourceAlerts) {
      closeAlert(alertState);
    }
    closeAlert(m_touchpadFingerAlert);
    m_initialized = false;
  }

  static void closeAlert(JoystickAlertState alertState) {
    closeAlert(alertState.alert);
    alertState.alert = null;
    alertState.active = false;
  }

  static void closeAlert(Alert alert) {
    if (alert == null) {
      return;
    }

    try {
      alert.close();
    } catch (AlertException ex) {
      // Ignore stale handles while cleaning up a failed initialization attempt.
    }
  }

  boolean prepareForReport() {
    if (DriverStationBackend.isJoystickConnected(m_stick)) {
      setAlert(m_connectionAlert, false, m_connectionText, true);
      return true;
    }

    clearResourceAlerts();
    setAlert(m_connectionAlert, true, m_connectionText, true);
    return false;
  }

  void refresh() {
    if (!DriverStationBackend.isJoystickConnected(m_stick)) {
      clearResourceAlerts();
      return;
    }

    if (m_connectionAlert.active) {
      setAlert(m_connectionAlert, false, m_connectionText, true);
    }

    for (JoystickResourceAlert resourceAlert : JOYSTICK_RESOURCE_ALERTS) {
      refreshResourceAlert(resourceAlert);
    }

    if (m_touchpadFingerAlert.active
        && DriverStationBackend.getStickTouchpadFingerAvailable(
            m_stick, m_touchpadFingerAlertTouchpad, m_touchpadFingerAlertFinger)) {
      setAlert(m_touchpadFingerAlert, false);
    }
  }

  void clearAllAlerts() {
    setAlert(m_connectionAlert, false, m_connectionText, true);
    clearResourceAlerts();
  }

  void clearResourceAlerts() {
    for (JoystickAlertState alertState : m_resourceAlerts) {
      setAlert(alertState, false);
    }
    setAlert(m_touchpadFingerAlert, false);
  }

  void refreshResourceAlert(JoystickResourceAlert resourceAlert) {
    int alertIndex = resourceAlert.ordinal();
    JoystickAlertState alertState = m_resourceAlerts[alertIndex];
    if (alertState.active
        && resourceAlert.isAvailable(m_stick, m_resourceAlertValues[alertIndex])) {
      setAlert(alertState, false);
    }
  }

  void reportResourceWarning(JoystickResourceAlert resourceAlert, int resource) {
    int alertIndex = resourceAlert.ordinal();
    m_resourceAlertValues[alertIndex] = resource;
    setAlert(m_resourceAlerts[alertIndex], true, resourceAlert.getText(m_stick, resource), false);
  }

  void reportTouchpadFingerWarning(int touchpad, int finger) {
    String text =
        "Joystick touchpad finger "
            + finger
            + " on touchpad "
            + touchpad
            + " on port "
            + m_stick
            + " not available";
    m_touchpadFingerAlertTouchpad = touchpad;
    m_touchpadFingerAlertFinger = finger;
    setAlert(m_touchpadFingerAlert, true, text, false);
  }

  boolean applyAlert(
      JoystickAlertState alertState, boolean active, String text, boolean verifyActive) {
    if (alertState.alert == null) {
      return false;
    }

    try {
      if (active && !text.isEmpty()) {
        alertState.alert.setText(text);
      }
      alertState.alert.set(active);
      return !active || !verifyActive || alertState.alert.get();
    } catch (AlertException ex) {
      return false;
    }
  }

  boolean setAlert(JoystickAlertState alertState, boolean active) {
    return setAlert(alertState, active, "", false);
  }

  boolean setAlert(
      JoystickAlertState alertState, boolean active, String text, boolean verifyActive) {
    if (!active && !alertState.active) {
      return true;
    }

    if (active && !m_initialized && !initializeAlerts()) {
      return false;
    }

    if (!applyAlert(alertState, active, text, verifyActive)) {
      if (!active) {
        alertState.active = false;
        return true;
      }
      if (!initializeAlerts()) {
        return false;
      }
      if (!applyAlert(alertState, active, text, verifyActive)) {
        return false;
      }
    }
    alertState.active = active;
    return true;
  }

  static void silenceJoystickConnectionAlert(boolean silence) {
    m_joystickAlertMutex.lock();
    try {
      m_silenceJoystickAlerts = silence;
      if (silence && !RobotState.isFMSAttached()) {
        clearJoystickAlertsLocked();
      }
    } finally {
      m_joystickAlertMutex.unlock();
    }
  }

  static boolean isJoystickConnectionAlertSilenced() {
    m_joystickAlertMutex.lock();
    try {
      return !RobotState.isFMSAttached() && m_silenceJoystickAlerts;
    } finally {
      m_joystickAlertMutex.unlock();
    }
  }

  static JoystickAlerts getJoystickAlertsLocked(int stick) {
    JoystickAlerts alerts = m_joystickAlerts[stick];
    if (alerts == null) {
      alerts = new JoystickAlerts(stick);
      m_joystickAlerts[stick] = alerts;
    }
    return alerts;
  }

  static void reportJoystickResourceWarning(
      int stick, JoystickResourceAlert resourceAlert, int resource) {
    m_joystickAlertMutex.lock();
    try {
      if (!RobotState.isFMSAttached() && m_silenceJoystickAlerts) {
        return;
      }

      JoystickAlerts alerts = getJoystickAlertsLocked(stick);
      if (!alerts.prepareForReport()) {
        return;
      }

      alerts.reportResourceWarning(resourceAlert, resource);
    } finally {
      m_joystickAlertMutex.unlock();
    }
  }

  static void reportJoystickTouchpadFingerWarning(int stick, int touchpad, int finger) {
    m_joystickAlertMutex.lock();
    try {
      if (!RobotState.isFMSAttached() && m_silenceJoystickAlerts) {
        return;
      }

      JoystickAlerts alerts = getJoystickAlertsLocked(stick);
      if (!alerts.prepareForReport()) {
        return;
      }

      alerts.reportTouchpadFingerWarning(touchpad, finger);
    } finally {
      m_joystickAlertMutex.unlock();
    }
  }

  static void clearJoystickAlerts() {
    m_joystickAlertMutex.lock();
    try {
      clearJoystickAlertsLocked();
    } finally {
      m_joystickAlertMutex.unlock();
    }
  }

  static void clearJoystickAlertsLocked() {
    if (!RobotState.isFMSAttached() && m_silenceJoystickAlerts) {
      for (JoystickAlerts alerts : m_joystickAlerts) {
        if (alerts != null) {
          alerts.clearAllAlerts();
        }
      }
      return;
    }

    for (JoystickAlerts alerts : m_joystickAlerts) {
      if (alerts != null) {
        alerts.refresh();
      }
    }
  }
}
