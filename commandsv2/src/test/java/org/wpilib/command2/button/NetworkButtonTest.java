// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2.button;

import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.CommandTestBase;
import org.wpilib.networktables.NetworkTableInstance;

class NetworkButtonTest extends CommandTestBase {
  NetworkTableInstance m_inst;

  @BeforeEach
  void setup() {
    m_inst = NetworkTableInstance.create();
    m_inst.startLocal();
  }

  @AfterEach
  void teardown() {
    m_inst.close();
  }

  @Test
  void setNetworkButtonTest() {
    var scheduler = CommandScheduler.getInstance();
    var commandHolder = new MockCommandHolder(true);
    var command = commandHolder.getMock();
    var pub = m_inst.getTable("TestTable").getBooleanTopic("Test").publish();

    var button = new NetworkButton(m_inst, "TestTable", "Test");
    pub.set(false);
    button.onTrue(command);
    scheduler.run();
    verify(command, never()).initialize();
    pub.set(true);
    scheduler.run();
    scheduler.run();
    verify(command).initialize();
  }
}
