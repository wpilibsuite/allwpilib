/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import org.junit.Ignore;
import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;

/**
 * Test that covers {@link SmartDashboard}.
 */
public class SmartDashboardTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(SmartDashboardTest.class.getName());
  private static final NetworkTable table = NetworkTable.getTable("SmartDashboard");

  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void testGetBadValue() {
    assertEquals("", SmartDashboard.getString("_404_STRING_KEY_SHOULD_NOT_BE_FOUND_", ""));
  }

  @Test
  public void testPutString() {
    String key = "testPutString";
    String value = "thisIsAValue";
    SmartDashboard.putString(key, value);
    assertEquals(value, SmartDashboard.getString(key, ""));
    assertEquals(value, table.getString(key));
  }

  @Test
  public void testPutNumber() {
    String key = "testPutNumber";
    int value = 2147483647;
    SmartDashboard.putNumber(key, value);
    assertEquals(value, SmartDashboard.getNumber(key, 0), 0.01);
    assertEquals(value, table.getNumber(key), 0.01);
  }

  @Test
  public void testPutBoolean() {
    String key = "testPutBoolean";
    boolean value = true;
    SmartDashboard.putBoolean(key, value);
    assertEquals(value, SmartDashboard.getBoolean(key, !value));
    assertEquals(value, table.getBoolean(key));
  }

  @Test
  public void testReplaceString() {
    String key = "testReplaceString";
    String valueOld = "oldValue";
    SmartDashboard.putString(key, valueOld);
    assertEquals(valueOld, SmartDashboard.getString(key, ""));
    assertEquals(valueOld, table.getString(key));

    String valueNew = "newValue";
    SmartDashboard.putString(key, valueNew);
    assertEquals(valueNew, SmartDashboard.getString(key, ""));
    assertEquals(valueNew, table.getString(key));
  }

  @Ignore
  @Test(expected = IllegalArgumentException.class)
  public void testPutStringNullKey() {
    SmartDashboard.putString(null, "This should not work");
  }

  @Ignore
  @Test(expected = IllegalArgumentException.class)
  public void testPutStringNullValue() {
    SmartDashboard.putString("KEY_SHOULD_NOT_BE_STORED", null);
  }
}
