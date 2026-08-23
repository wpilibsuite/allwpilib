// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class CommandTest extends CommandTestBase {
  @Test
  void defaultSubsystemIsUngrouped() {
    assertEquals("Ungrouped", Commands.none().getSubsystem());
  }
}
