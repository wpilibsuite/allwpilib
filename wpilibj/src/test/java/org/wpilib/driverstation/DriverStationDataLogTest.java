// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.io.ByteArrayOutputStream;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.Isolated;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;
import org.wpilib.datalog.DataLogReader;
import org.wpilib.datalog.DataLogWriter;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.simulation.DriverStationSim;

@Isolated("Uses the process-wide DriverStation data log sender")
class DriverStationDataLogTest {
  private Field m_senderField;
  private Object m_previousSender;
  private ByteArrayOutputStream m_data;
  private DataLogWriter m_log;

  @BeforeEach
  @SuppressWarnings("PMD.AvoidAccessibilityAlteration")
  void setUp() throws ReflectiveOperationException {
    HAL.initialize();
    // There is no public stopDataLog(); detach the sender before closing its log.
    m_senderField = DriverStationBackend.class.getDeclaredField("m_dataLogSender");
    m_senderField.setAccessible(true);
    m_previousSender = m_senderField.get(null);
    m_senderField.set(null, null);

    RobotState.clearUserProgramStarted();
    RobotState.clearOpModes();
    DriverStationSim.resetData();
    DriverStationSim.notifyNewData();
    m_data = new ByteArrayOutputStream();
    m_log = new DataLogWriter(m_data);
  }

  @AfterEach
  void tearDown() throws ReflectiveOperationException {
    m_senderField.set(null, null);
    try {
      m_log.close();
      RobotState.clearUserProgramStarted();
      RobotState.clearOpModes();
      DriverStationSim.resetData();
      DriverStationSim.notifyNewData();
    } finally {
      m_senderField.set(null, m_previousSender);
    }
  }

  @ParameterizedTest
  @ValueSource(booleans = {true, false})
  void logsSelectedOpModeBeforeUserProgramStarts(boolean selectBeforeLogging) {
    long id = RobotState.addOpMode(RobotMode.AUTONOMOUS, "Selected auto");
    if (selectBeforeLogging) {
      selectOpMode(id);
    }

    DriverStation.startDataLog(m_log, false);
    if (!selectBeforeLogging) {
      selectOpMode(id);
    }
    assertEquals("", RobotState.getOpMode());

    RobotState.observeUserProgramStarting();
    DriverStationSim.notifyNewData();
    DriverStationSim.notifyNewData();
    assertEquals(id, RobotState.getOpModeId());
    assertEquals("Selected auto", RobotState.getOpMode());

    m_log.flush();
    Map<Integer, String> entries = new HashMap<>();
    List<String> opModes = new ArrayList<>();
    new DataLogReader(ByteBuffer.wrap(m_data.toByteArray()))
        .forEach(
            record -> {
              if (record.isStart()) {
                var start = record.getStartData();
                entries.put(start.entry, start.name);
              } else if ("DS:opMode".equals(entries.get(record.getEntry()))) {
                opModes.add(record.getString());
              }
            });
    assertEquals("Selected auto", opModes.getLast());
  }

  private static void selectOpMode(long id) {
    DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
    DriverStationSim.setOpMode(id);
    // Also fill the previous-packet cache used by logging before the refactor in
    // https://github.com/wpilibsuite/allwpilib/pull/9148.
    DriverStationSim.notifyNewData();
    DriverStationSim.notifyNewData();
  }
}
