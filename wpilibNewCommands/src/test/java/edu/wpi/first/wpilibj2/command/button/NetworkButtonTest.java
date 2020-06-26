/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command.button;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.CommandTestBase;

import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

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
    var entry = NetworkTableInstance.getDefault()
        .getTable("TestTable")
        .getEntry("Test");

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
