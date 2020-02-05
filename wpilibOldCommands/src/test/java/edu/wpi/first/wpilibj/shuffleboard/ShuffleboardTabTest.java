/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.HashMap;
import java.util.Map;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.command.InstantCommand;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

@SuppressWarnings({"PMD.TooManyMethods"})
public class ShuffleboardTabTest {
  private NetworkTableInstance m_ntInstance;
  private ShuffleboardTab m_tab;
  private ShuffleboardInstance m_instance;

  @BeforeEach
  void setup() {
    m_ntInstance = NetworkTableInstance.create();
    m_instance = new ShuffleboardInstance(m_ntInstance);
    m_tab = m_instance.getTab("Tab");
  }

  @AfterEach
  void tearDown() {
    m_ntInstance.close();
  }

  @Test
  void testAddDouble() {
    NetworkTableEntry entry = m_tab.add("Double", 1.0).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Double", entry.getName()),
        () -> assertEquals(1.0, entry.getValue().getDouble()));
  }

  @Test
  void testAddInteger() {
    NetworkTableEntry entry = m_tab.add("Int", 1).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Int", entry.getName()),
        () -> assertEquals(1.0, entry.getValue().getDouble()));
  }

  @Test
  void testAddLong() {
    NetworkTableEntry entry = m_tab.add("Long", 1L).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Long", entry.getName()),
        () -> assertEquals(1.0, entry.getValue().getDouble()));
  }


  @Test
  void testAddBoolean() {
    NetworkTableEntry entry = m_tab.add("Bool", false).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Bool", entry.getName()),
        () -> assertFalse(entry.getValue().getBoolean()));
  }

  @Test
  void testAddString() {
    NetworkTableEntry entry = m_tab.add("String", "foobar").getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/String", entry.getName()),
        () -> assertEquals("foobar", entry.getValue().getString()));
  }

  @Test
  void testAddNamedSendableWithProperties() {
    Sendable sendable = new InstantCommand("Command");
    String widgetType = "Command Widget";
    m_tab.add(sendable)
       .withWidget(widgetType)
       .withProperties(mapOf("foo", 1234, "bar", "baz"));

    m_instance.update();
    String meta = "/Shuffleboard/.metadata/Tab/Command";

    assertAll(
        () -> assertEquals(1234,
                           m_ntInstance.getEntry(meta + "/Properties/foo").getDouble(-1),
                           "Property 'foo' not set correctly"),
        () -> assertEquals("baz",
                           m_ntInstance.getEntry(meta + "/Properties/bar").getString(null),
                           "Property 'bar' not set correctly"),
        () -> assertEquals(widgetType,
                           m_ntInstance.getEntry(meta + "/PreferredComponent").getString(null),
                           "Preferred component not set correctly"));
  }

  @Test
  void testAddNumberArray() {
    NetworkTableEntry entry = m_tab.add("DoubleArray", new double[]{1, 2, 3}).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/DoubleArray", entry.getName()),
        () -> assertArrayEquals(new double[]{1, 2, 3}, entry.getValue().getDoubleArray()));
  }

  @Test
  void testAddBooleanArray() {
    NetworkTableEntry entry = m_tab.add("BoolArray", new boolean[]{true, false}).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/BoolArray", entry.getName()),
        () -> assertArrayEquals(new boolean[]{true, false}, entry.getValue().getBooleanArray()));
  }

  @Test
  void testAddStringArray() {
    NetworkTableEntry entry = m_tab.add("StringArray", new String[]{"foo", "bar"}).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/StringArray", entry.getName()),
        () -> assertArrayEquals(new String[]{"foo", "bar"}, entry.getValue().getStringArray()));
  }

  /**
   * Stub for Java 9 {@code Map.of()}.
   */
  @SuppressWarnings({"unchecked", "PMD"})
  private static <K, V> Map<K, V> mapOf(Object... entries) {
    Map<K, V> map = new HashMap<>();
    for (int i = 0; i < entries.length; i += 2) {
      map.put((K) entries[i], (V) entries[i + 1]);
    }
    return map;
  }

}
