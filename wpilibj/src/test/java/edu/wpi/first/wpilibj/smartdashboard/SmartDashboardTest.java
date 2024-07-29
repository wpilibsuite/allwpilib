// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.UtilityClassTest;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class SmartDashboardTest extends UtilityClassTest<SmartDashboard> {
  private NetworkTableInstance m_inst;
  private NetworkTable m_table;

  SmartDashboardTest() {
    super(SmartDashboard.class);
  }

  @BeforeEach
  void beforeEach() {
    m_inst = NetworkTableInstance.create();
    m_table = m_inst.getTable("SmartDashboard");
    SmartDashboard.setNetworkTableInstance(m_inst);
  }

  @AfterEach
  void afterEach() {
    m_inst.close();
  }

  @Test
  void getBadValueTest() {
    assertEquals("Expected", SmartDashboard.getString("KEY_SHOULD_NOT_BE_FOUND", "Expected"));
  }

  @Test
  void putStringTest() {
    final String key = "putString";
    final String value = "thisIsAValue";

    SmartDashboard.putString(key, value);

    assertEquals(value, m_table.getEntry(key).getString(""));
  }

  @Test
  void getStringTest() {
    final String key = "getString";
    final String value = "thisIsAValue";

    m_table.getEntry(key).setString(value);

    assertEquals(value, SmartDashboard.getString(key, ""));
  }

  @Test
  void putNumberTest() {
    final String key = "PutNumber";
    final int value = 2147483647;

    SmartDashboard.putNumber(key, value);

    assertEquals(value, m_table.getEntry(key).getNumber(0).intValue());
  }

  @Test
  void getNumberTest() {
    final String key = "GetNumber";
    final int value = 2147483647;

    m_table.getEntry(key).setNumber(value);

    assertEquals(value, SmartDashboard.getNumber(key, 0), 0.01);
  }

  @Test
  void putBooleanTest() {
    final String key = "PutBoolean";
    final boolean value = true;

    SmartDashboard.putBoolean(key, value);

    assertEquals(value, m_table.getEntry(key).getBoolean(!value));
  }

  @Test
  void getBooleanTest() {
    final String key = "GetBoolean";
    final boolean value = true;

    m_table.getEntry(key).setBoolean(value);

    assertEquals(value, SmartDashboard.getBoolean(key, !value));
  }

  @Test
  void testReplaceString() {
    final String key = "testReplaceString";
    final String valueNew = "newValue";

    m_table.getEntry(key).setString("oldValue");
    SmartDashboard.putString(key, valueNew);

    assertEquals(valueNew, m_table.getEntry(key).getString(""));
  }

  @Test
  void putStringNullKeyTest() {
    assertThrows(
        NullPointerException.class, () -> SmartDashboard.putString(null, "This should not work"));
  }

  @Test
  void putStringNullValueTest() {
    assertThrows(
        NullPointerException.class,
        () -> SmartDashboard.putString("KEY_SHOULD_NOT_BE_STORED", null));
  }
}
