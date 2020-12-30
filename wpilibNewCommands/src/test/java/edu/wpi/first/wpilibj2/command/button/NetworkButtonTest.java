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
  @BeforeEach
  void setup() {
    NetworkTableInstance.getDefault().startLocal();
  }

  @AfterEach
  void teardown() {
    NetworkTableInstance.getDefault().deleteAllEntries();
    NetworkTableInstance.getDefault().stopLocal();
  }

  @Test
  void setNetworkButtonTest() {
    var scheduler = CommandScheduler.getInstance();
    var commandHolder = new MockCommandHolder(true);
    var command = commandHolder.getMock();
    var entry = NetworkTableInstance.getDefault().getTable("TestTable").getEntry("Test");

    var button = new NetworkButton("TestTable", "Test");
    entry.setBoolean(false);
    button.whenPressed(command);
    scheduler.run();
    verify(command, never()).schedule(true);
    entry.setBoolean(true);
    scheduler.run();
    scheduler.run();
    verify(command).schedule(true);
  }
}
