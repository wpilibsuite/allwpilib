/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class ShuffleboardInstanceTest {
  private NetworkTableInstance m_ntInstance;
  private ShuffleboardInstance m_shuffleboardInstance;

  @BeforeEach
  void setupInstance() {
    m_ntInstance = NetworkTableInstance.create();
    m_shuffleboardInstance = new ShuffleboardInstance(m_ntInstance);
  }

  @AfterEach
  void tearDownInstance() {
    m_ntInstance.close();
  }

  @Test
  void testPathFluent() {
    NetworkTableEntry entry = m_shuffleboardInstance.getTab("Tab Title")
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
  void testNestedLayoutsFluent() {
    NetworkTableEntry entry = m_shuffleboardInstance.getTab("Tab")
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
  void testNestedLayoutsOop() {
    ShuffleboardTab tab = m_shuffleboardInstance.getTab("Tab");
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

  @Test
  void testLayoutTypeIsSet() {
    String layoutType = "Type";
    m_shuffleboardInstance.getTab("Tab")
                          .getLayout(layoutType, "Title");
    m_shuffleboardInstance.update();
    NetworkTableEntry entry = m_ntInstance.getEntry(
        "/Shuffleboard/.metadata/Tab/Title/PreferredComponent");
    assertEquals(layoutType, entry.getString("Not Set"), "Layout type not set");
  }

  @Test
  void testNestedActuatorWidgetsAreDisabled() {
    m_shuffleboardInstance.getTab("Tab")
                          .getLayout("Layout", "Title")
                          .add(new MockActuatorSendable("Actuator"));
    NetworkTableEntry controllableEntry =
        m_ntInstance.getEntry("/Shuffleboard/Tab/Title/Actuator/.controllable");

    m_shuffleboardInstance.update();

    // Note: we use the unsafe `getBoolean()` method because if the value is NOT a boolean, or if it
    // is not present, then something has clearly gone very, very wrong
    boolean controllable = controllableEntry.getValue().getBoolean();

    // Sanity check
    assertTrue(controllable, "The nested actuator widget should be enabled by default");
    m_shuffleboardInstance.disableActuatorWidgets();
    controllable = controllableEntry.getValue().getBoolean();
    assertFalse(controllable, "The nested actuator widget should have been disabled");
  }

}
