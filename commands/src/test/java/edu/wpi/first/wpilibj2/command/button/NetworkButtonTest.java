// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.CommandTestBase;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

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
    verify(command, never()).schedule();
    pub.set(true);
    scheduler.run();
    scheduler.run();
    verify(command).schedule();
  }
}
