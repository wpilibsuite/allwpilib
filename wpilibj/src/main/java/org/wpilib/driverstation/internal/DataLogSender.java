// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation.internal;

import org.wpilib.datalog.BooleanArrayLogEntry;
import org.wpilib.datalog.DataLog;
import org.wpilib.datalog.FloatArrayLogEntry;
import org.wpilib.datalog.IntegerArrayLogEntry;
import org.wpilib.datalog.StringLogEntry;
import org.wpilib.datalog.StructLogEntry;
import org.wpilib.driverstation.RobotState;
import org.wpilib.driverstation.internal.DriverStationBackend.HALJoystickAxes;
import org.wpilib.driverstation.internal.DriverStationBackend.HALJoystickButtons;
import org.wpilib.driverstation.internal.DriverStationBackend.HALJoystickPOVs;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;

class DataLogSender {
  private static class JoystickLogSender {
    JoystickLogSender(
        DataLog log,
        int stick,
        HALJoystickButtons[] joystickButtons,
        HALJoystickAxes[] joystickAxes,
        HALJoystickPOVs[] joystickPOVs,
        long timestamp) {
      m_stick = stick;

      m_logButtons = new BooleanArrayLogEntry(log, "DS:joystick" + stick + "/buttons", timestamp);
      m_logAxes = new FloatArrayLogEntry(log, "DS:joystick" + stick + "/axes", timestamp);
      m_logPOVs = new IntegerArrayLogEntry(log, "DS:joystick" + stick + "/povs", timestamp);

      appendButtons(joystickButtons[m_stick], timestamp);
      appendAxes(joystickAxes[m_stick], timestamp);
      appendPOVs(joystickPOVs[m_stick], timestamp);
    }

    public void send(
        HALJoystickButtons[] joystickButtons,
        HALJoystickAxes[] joystickAxes,
        HALJoystickPOVs[] joystickPOVs,
        long timestamp) {
      HALJoystickButtons buttons = joystickButtons[m_stick];
      if (buttons.m_available != m_prevButtons.m_available
          || buttons.m_buttons != m_prevButtons.m_buttons) {
        appendButtons(buttons, timestamp);
      }

      HALJoystickAxes axes = joystickAxes[m_stick];
      int available = axes.m_available;
      boolean needToLog = false;
      if (available != m_prevAxes.m_available) {
        needToLog = true;
      } else {
        for (int i = 0; i < m_prevAxes.m_axes.length; i++) {
          if (axes.m_axes[i] != m_prevAxes.m_axes[i]) {
            needToLog = true;
            break;
          }
        }
      }
      if (needToLog) {
        appendAxes(axes, timestamp);
      }

      HALJoystickPOVs povs = joystickPOVs[m_stick];
      available = joystickPOVs[m_stick].m_available;
      needToLog = false;
      if (available != m_prevPOVs.m_available) {
        needToLog = true;
      } else {
        for (int i = 0; i < m_prevPOVs.m_povs.length; i++) {
          if (povs.m_povs[i] != m_prevPOVs.m_povs[i]) {
            needToLog = true;
            break;
          }
        }
      }
      if (needToLog) {
        appendPOVs(povs, timestamp);
      }
    }

    void appendButtons(HALJoystickButtons buttons, long timestamp) {
      int count = DriverStationBackend.availableToCount(buttons.m_available);
      if (m_sizedButtons == null || m_sizedButtons.length != count) {
        m_sizedButtons = new boolean[count];
      }
      long buttonsValue = buttons.m_buttons;
      for (int i = 0; i < count; i++) {
        m_sizedButtons[i] = (buttonsValue & (1L << i)) != 0;
      }
      m_logButtons.append(m_sizedButtons, timestamp);
      m_prevButtons.m_available = buttons.m_available;
      m_prevButtons.m_buttons = buttons.m_buttons;
    }

    void appendAxes(HALJoystickAxes axes, long timestamp) {
      int count = DriverStationBackend.availableToCount(axes.m_available);
      if (m_sizedAxes == null || m_sizedAxes.length != count) {
        m_sizedAxes = new float[count];
      }
      System.arraycopy(axes.m_axes, 0, m_sizedAxes, 0, count);
      m_logAxes.append(m_sizedAxes, timestamp);
      m_prevAxes.m_available = axes.m_available;
      System.arraycopy(axes.m_axes, 0, m_prevAxes.m_axes, 0, count);
    }

    void appendPOVs(HALJoystickPOVs povs, long timestamp) {
      int count = DriverStationBackend.availableToCount(povs.m_available);
      if (m_sizedPOVs == null || m_sizedPOVs.length != count) {
        m_sizedPOVs = new long[count];
      }
      for (int i = 0; i < count; i++) {
        m_sizedPOVs[i] = povs.m_povs[i];
      }
      m_logPOVs.append(m_sizedPOVs, timestamp);
      m_prevPOVs.m_available = povs.m_available;
      System.arraycopy(povs.m_povs, 0, m_prevPOVs.m_povs, 0, count);
    }

    final int m_stick;
    boolean[] m_sizedButtons;
    float[] m_sizedAxes;
    long[] m_sizedPOVs;
    final HALJoystickButtons m_prevButtons = new HALJoystickButtons();
    final HALJoystickAxes m_prevAxes = new HALJoystickAxes(DriverStationJNI.MAX_JOYSTICK_AXES);
    final HALJoystickPOVs m_prevPOVs = new HALJoystickPOVs(DriverStationJNI.MAX_JOYSTICK_POVS);
    final BooleanArrayLogEntry m_logButtons;
    final FloatArrayLogEntry m_logAxes;
    final IntegerArrayLogEntry m_logPOVs;
  }

  DataLogSender(
      DataLog log,
      boolean logJoysticks,
      ControlWord controlWord,
      HALJoystickButtons[] joystickButtons,
      HALJoystickAxes[] joystickAxes,
      HALJoystickPOVs[] joystickPOVs,
      long timestamp) {
    m_logControlWord = StructLogEntry.create(log, "DS:controlWord", ControlWord.struct, timestamp);

    // append initial control word value
    m_logControlWord.append(controlWord, timestamp);
    m_oldControlWord.update(controlWord);

    // append initial opmode value
    m_logOpMode = new StringLogEntry(log, "DS:opMode", timestamp);
    m_logOpMode.append(RobotState.getOpMode(), timestamp);

    if (logJoysticks) {
      m_joysticks = new JoystickLogSender[DriverStationBackend.JOYSTICK_PORTS];
      for (int i = 0; i < DriverStationBackend.JOYSTICK_PORTS; i++) {
        m_joysticks[i] =
            new JoystickLogSender(log, i, joystickButtons, joystickAxes, joystickPOVs, timestamp);
      }
    } else {
      m_joysticks = new JoystickLogSender[0];
    }
  }

  public void send(
      ControlWord controlWord,
      HALJoystickButtons[] joystickButtons,
      HALJoystickAxes[] joystickAxes,
      HALJoystickPOVs[] joystickPOVs,
      long timestamp) {
    // append control word value changes
    if (!controlWord.equals(m_oldControlWord)) {
      // append opmode value changes
      long opModeId = controlWord.getOpModeId();
      if (opModeId != m_oldControlWord.getOpModeId()) {
        m_logOpMode.append(RobotState.getOpMode(), timestamp);
      }

      m_logControlWord.append(controlWord, timestamp);
      m_oldControlWord.update(controlWord);
    }

    // append joystick value changes
    for (JoystickLogSender joystick : m_joysticks) {
      joystick.send(joystickButtons, joystickAxes, joystickPOVs, timestamp);
    }
  }

  final ControlWord m_oldControlWord = new ControlWord();
  final StructLogEntry<ControlWord> m_logControlWord;
  final StringLogEntry m_logOpMode;

  final JoystickLogSender[] m_joysticks;
}
