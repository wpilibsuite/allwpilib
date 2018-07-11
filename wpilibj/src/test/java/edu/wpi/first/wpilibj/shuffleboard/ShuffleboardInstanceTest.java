/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

@SuppressWarnings("MemberName")
public class ShuffleboardInstanceTest {
  private NetworkTableInstance ntInstance; // NOPMD
  private ShuffleboardInstance shuffleboardInstance;

  @BeforeEach
  public void setupInstance() {
    ntInstance = NetworkTableInstance.create();
    shuffleboardInstance = new ShuffleboardInstance(ntInstance);
  }

  @Test
  public void testPathFluent() {
    NetworkTableEntry entry = shuffleboardInstance.getTab("Tab Title")
                                                  .getLayout("List", "List Layout")
                                                  .add("Data", "string")
                                                  .withWidget("Text View")
                                                  .getEntry();

    assertAll(
        () -> assertEquals("string", entry.getString(null), "Wrong entry value"),
        () -> assertEquals("/Shuffleboard/Tab Title/List Layout/Data", entry.getName(),
                           "Entry path generated incorrectly"));
  }

  @Test
  public void testNestedLayoutsFluent() {
    NetworkTableEntry entry = shuffleboardInstance.getTab("Tab")
                                                  .getLayout("List", "First")
                                                  .getLayout("List", "Second")
                                                  .getLayout("List", "Third")
                                                  .getLayout("List", "Fourth")
                                                  .add("Value", "string")
                                                  .getEntry();

    assertAll(
        () -> assertEquals("string", entry.getString(null), "Wrong entry value"),
        () -> assertEquals("/Shuffleboard/Tab/First/Second/Third/Fourth/Value", entry.getName(),
                           "Entry path generated incorrectly"));
  }

  @Test
  public void testNestedLayoutsOop() {
    ShuffleboardTab tab = shuffleboardInstance.getTab("Tab");
    ShuffleboardLayout first = tab.getLayout("List", "First");
    ShuffleboardLayout second = first.getLayout("List", "Second");
    ShuffleboardLayout third = second.getLayout("List", "Third");
    ShuffleboardLayout fourth = third.getLayout("List", "Fourth");
    SimpleWidget widget = fourth.add("Value", "string");
    NetworkTableEntry entry = widget.getEntry();

    assertAll(
        () -> assertEquals("string", entry.getString(null)),
        () -> assertEquals("/Shuffleboard/Tab/First/Second/Third/Fourth/Value", entry.getName(),
                           "Entry path generated incorrectly"));
  }

}
