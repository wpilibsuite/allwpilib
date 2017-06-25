/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * Tests the {@link Preferences}.
 */
public class PreferencesTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PreferencesTest.class.getName());

  private NetworkTable m_prefTable;
  private Preferences m_pref;
  private long m_check;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }


  @Before
  public void setUp() throws Exception {
    NetworkTable.shutdown();
    try {
      File file = new File("networktables.ini");
      file.mkdirs();
      if (file.exists()) {
        file.delete();
      }
      file.createNewFile();
      OutputStream output = new FileOutputStream(file);
      output
          .write(("[NetworkTables Storage 3.0]\ndouble \"/Preferences/checkedValueInt\"=2\ndouble "
              + "\"/Preferences/checkedValueDouble\"=.2\ndouble "
              + "\"/Preferences/checkedValueFloat\"=3.14\ndouble "
              + "\"/Preferences/checkedValueLong\"=172\nstring "
              + "\"/Preferences/checkedValueString\"=\"hello \\nHow are you ?\"\nboolean "
              + "\"/Preferences/checkedValueBoolean\"=false\n")
              .getBytes());

    } catch (IOException ex) {
      ex.printStackTrace();
    }
    NetworkTable.initialize();

    m_pref = Preferences.getInstance();
    m_prefTable = NetworkTable.getTable("Preferences");
    m_check = System.currentTimeMillis();
  }


  protected void remove() {
    m_pref.remove("checkedValueLong");
    m_pref.remove("checkedValueDouble");
    m_pref.remove("checkedValueString");
    m_pref.remove("checkedValueInt");
    m_pref.remove("checkedValueFloat");
    m_pref.remove("checkedValueBoolean");
  }

  protected void addCheckedValue() {
    m_pref.putLong("checkedValueLong", m_check);
    m_pref.putDouble("checkedValueDouble", 1);
    m_pref.putString("checkedValueString", "checked");
    m_pref.putInt("checkedValueInt", 1);
    m_pref.putFloat("checkedValueFloat", 1);
    m_pref.putBoolean("checkedValueBoolean", true);
  }

  @Test
  public void testAddRemoveSave() {
    assertEquals(m_pref.getLong("checkedValueLong", 0), 172L);
    assertEquals(m_pref.getDouble("checkedValueDouble", 0), .2, 0);
    assertEquals(m_pref.getString("checkedValueString", ""), "hello \nHow are you ?");
    assertEquals(m_pref.getInt("checkedValueInt", 0), 2);
    assertEquals(m_pref.getFloat("checkedValueFloat", 0), 3.14, .001);
    assertFalse(m_pref.getBoolean("checkedValueBoolean", true));
    remove();
    assertEquals(m_pref.getLong("checkedValueLong", 0), 0);
    assertEquals(m_pref.getDouble("checkedValueDouble", 0), 0, 0);
    assertEquals(m_pref.getString("checkedValueString", ""), "");
    assertEquals(m_pref.getInt("checkedValueInt", 0), 0);
    assertEquals(m_pref.getFloat("checkedValueFloat", 0), 0, 0);
    assertFalse(m_pref.getBoolean("checkedValueBoolean", false));
    addCheckedValue();
    assertEquals(m_check, m_pref.getLong("checkedValueLong", 0));
    assertEquals(m_pref.getDouble("checkedValueDouble", 0), 1, 0);
    assertEquals(m_pref.getString("checkedValueString", ""), "checked");
    assertEquals(m_pref.getInt("checkedValueInt", 0), 1);
    assertEquals(m_pref.getFloat("checkedValueFloat", 0), 1, 0);
    assertTrue(m_pref.getBoolean("checkedValueBoolean", false));
  }

  @Test
  public void testPreferencesToNetworkTables() {
    String networkedNumber = "networkCheckedValue";
    int networkNumberValue = 100;
    m_pref.putInt(networkedNumber, networkNumberValue);
    assertEquals(networkNumberValue, (int) (m_prefTable.getNumber(networkedNumber)));
    m_pref.remove(networkedNumber);
  }

}
