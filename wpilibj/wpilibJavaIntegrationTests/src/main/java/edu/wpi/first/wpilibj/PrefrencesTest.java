/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.logging.Logger;

import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

/**
 * @author jonathanleitschuh
 *
 */
public class PrefrencesTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PrefrencesTest.class.getName());

  private NetworkTable prefTable;
  private Preferences pref;
  private long check;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }


  /**
   * @throws java.lang.Exception
   */
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
          .write("[NetworkTables Storage 3.0]\ndouble \"/Preferences/checkedValueInt\"=2\ndouble \"/Preferences/checkedValueDouble\"=.2\ndouble \"/Preferences/checkedValueFloat\"=3.14\ndouble \"/Preferences/checkedValueLong\"=172\nstring \"/Preferences/checkedValueString\"=\"hello \\nHow are you ?\"\nboolean \"/Preferences/checkedValueBoolean\"=false\n"
              .getBytes());

    } catch (IOException e) {
      e.printStackTrace();
    }
    NetworkTable.initialize();

    pref = Preferences.getInstance();
    prefTable = NetworkTable.getTable("Preferences");
    check = System.currentTimeMillis();
  }


  public void remove() {
    pref.remove("checkedValueLong");
    pref.remove("checkedValueDouble");
    pref.remove("checkedValueString");
    pref.remove("checkedValueInt");
    pref.remove("checkedValueFloat");
    pref.remove("checkedValueBoolean");
  }

  public void addCheckedValue() {
    pref.putLong("checkedValueLong", check);
    pref.putDouble("checkedValueDouble", 1);
    pref.putString("checkedValueString", "checked");
    pref.putInt("checkedValueInt", 1);
    pref.putFloat("checkedValueFloat", 1);
    pref.putBoolean("checkedValueBoolean", true);
  }

  @Test
  public void testAddRemoveSave() {
    assertEquals(pref.getLong("checkedValueLong", 0), 172L);
    assertEquals(pref.getDouble("checkedValueDouble", 0), .2, 0);
    assertEquals(pref.getString("checkedValueString", ""), "hello \nHow are you ?");
    assertEquals(pref.getInt("checkedValueInt", 0), 2);
    assertEquals(pref.getFloat("checkedValueFloat", 0), 3.14, .001);
    assertFalse(pref.getBoolean("checkedValueBoolean", true));
    remove();
    assertEquals(pref.getLong("checkedValueLong", 0), 0);
    assertEquals(pref.getDouble("checkedValueDouble", 0), 0, 0);
    assertEquals(pref.getString("checkedValueString", ""), "");
    assertEquals(pref.getInt("checkedValueInt", 0), 0);
    assertEquals(pref.getFloat("checkedValueFloat", 0), 0, 0);
    assertFalse(pref.getBoolean("checkedValueBoolean", false));
    addCheckedValue();
    pref.save();
    assertEquals(check, pref.getLong("checkedValueLong", 0));
    assertEquals(pref.getDouble("checkedValueDouble", 0), 1, 0);
    assertEquals(pref.getString("checkedValueString", ""), "checked");
    assertEquals(pref.getInt("checkedValueInt", 0), 1);
    assertEquals(pref.getFloat("checkedValueFloat", 0), 1, 0);
    assertTrue(pref.getBoolean("checkedValueBoolean", false));
  }

  @Test
  public void testPreferencesToNetworkTables() {
    String networkedNumber = "networkCheckedValue";
    int networkNumberValue = 100;
    pref.putInt(networkedNumber, networkNumberValue);
    assertEquals(networkNumberValue, (int)(prefTable.getNumber(networkedNumber)));
    pref.remove(networkedNumber);
  }

}
