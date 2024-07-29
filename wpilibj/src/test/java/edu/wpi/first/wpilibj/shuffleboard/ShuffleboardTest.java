// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import static org.junit.jupiter.api.Assertions.assertSame;

import edu.wpi.first.wpilibj.UtilityClassTest;
import org.junit.jupiter.api.Test;

class ShuffleboardTest extends UtilityClassTest<Shuffleboard> {
  ShuffleboardTest() {
    super(Shuffleboard.class);
  }

  // Most relevant tests are in ShuffleboardTabTest

  @Test
  void testTabObjectsCached() {
    ShuffleboardTab tab1 = Shuffleboard.getTab("testTabObjectsCached");
    ShuffleboardTab tab2 = Shuffleboard.getTab("testTabObjectsCached");
    assertSame(tab1, tab2, "Tab objects were not cached");
  }
}
