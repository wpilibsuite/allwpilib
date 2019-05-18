/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.concurrent.atomic.AtomicInteger;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class SuppliedValueWidgetTest {
  private NetworkTableInstance m_ntInstance;
  private ShuffleboardInstance m_instance;

  @BeforeEach
  void setup() {
    m_ntInstance = NetworkTableInstance.create();
    m_instance = new ShuffleboardInstance(m_ntInstance);
  }

  @Test
  void testAddString() {
    AtomicInteger count = new AtomicInteger(0);
    m_instance.getTab("Tab")
        .addString("Title", () -> Integer.toString(count.incrementAndGet()));
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertEquals("1", entry.getString(null));

    m_instance.update();
    assertEquals("2", entry.getString(null));
  }

  @Test
  void testAddDouble() {
    AtomicInteger num = new AtomicInteger(0);
    m_instance.getTab("Tab")
        .addNumber("Title", num::incrementAndGet);
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertEquals(1, entry.getDouble(0));

    m_instance.update();
    assertEquals(2, entry.getDouble(0));
  }

  @Test
  void testAddBoolean() {
    boolean[] bool = {false};
    m_instance.getTab("Tab")
        .addBoolean("Title", () -> bool[0] = !bool[0]);
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertTrue(entry.getBoolean(false));

    m_instance.update();
    assertFalse(entry.getBoolean(true));
  }

  @Test
  void testAddStringArray() {
    String[] arr = {"foo", "bar"};
    m_instance.getTab("Tab")
        .addStringArray("Title", () -> arr);
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertArrayEquals(arr, entry.getStringArray(new String[0]));
  }

  @Test
  void testAddDoubleArray() {
    double[] arr = {0, 1};
    m_instance.getTab("Tab")
        .addDoubleArray("Title", () -> arr);
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertArrayEquals(arr, entry.getDoubleArray(new double[0]));
  }

  @Test
  void testAddBooleanArray() {
    boolean[] arr = {true, false};
    m_instance.getTab("Tab")
        .addBooleanArray("Title", () -> arr);
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertArrayEquals(arr, entry.getBooleanArray(new boolean[0]));
  }

  @Test
  void testAddRawBytes() {
    byte[] arr = {0, 1, 2, 3};
    m_instance.getTab("Tab")
        .addRaw("Title", () -> arr);
    NetworkTableEntry entry = m_ntInstance.getEntry("/Shuffleboard/Tab/Title");

    m_instance.update();
    assertArrayEquals(arr, entry.getRaw(new byte[0]));
  }

}
