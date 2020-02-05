/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.stream.DoubleStream;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.UtilityClassTest;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

public class ShuffleboardTest extends UtilityClassTest<Shuffleboard> {
  public ShuffleboardTest() {
    super(Shuffleboard.class);
  }

  // Most relevant tests are in ShuffleboardTabTest

  @Test
  void testTabObjectsCached() {
    ShuffleboardTab tab1 = Shuffleboard.getTab("testTabObjectsCached");
    ShuffleboardTab tab2 = Shuffleboard.getTab("testTabObjectsCached");
    assertSame(tab1, tab2, "Tab objects were not cached");
  }

  @Test
  void testAddCanBeRepeated() {
    String tabName = "Tab";
    String title = "Title";
    SimpleWidget widget = Shuffleboard.getTab(tabName).add(title, 0);
    assertAll(DoubleStream.iterate(1, d -> d <= 10, d -> d + 1)
        .mapToObj(d -> () -> {
          Shuffleboard.getTab(tabName).add(title, d);
          assertEquals(d, widget.getEntry().getNumber(0), "Entry was not set");
        }));
  }
}
