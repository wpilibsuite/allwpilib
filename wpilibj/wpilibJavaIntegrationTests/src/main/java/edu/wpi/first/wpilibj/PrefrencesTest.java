/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.command.AbstractCommandTest;
import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 * @author jonathanleitschuh
 *
 */
public class PrefrencesTest extends AbstractCommandTest {
	
	private NetworkTable prefTable;
	private Preferences pref;
	private long check;

	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		try {
			File file = new File("wpilib-preferences.ini");
			file.mkdirs();
			if (file.exists()) {
				file.delete();
			}
			file.createNewFile();
			OutputStream output = new FileOutputStream(file);
			output.write("checkedValueInt = 2\ncheckedValueDouble = .2\ncheckedValueFloat = 3.14\ncheckedValueLong = 172\ncheckedValueString =\"hello \nHow are you ?\"\ncheckedValueBoolean = false"
					.getBytes());

		} catch (IOException e) {
			e.printStackTrace();
		}

		pref = Preferences.getInstance();
		prefTable = NetworkTable.getTable("Preferences");
		check = System.currentTimeMillis();
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
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
		assertEquals(pref.getString("checkedValueString", ""),
				"hello \nHow are you ?");
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
		assertEquals(pref.getString("checkedValueString", ""),
				"checked");
		assertEquals(pref.getInt("checkedValueInt", 0), 1);
		assertEquals(pref.getFloat("checkedValueFloat", 0), 1, 0);
		assertTrue(pref.getBoolean("checkedValueBoolean", false));
	}
	
	@Test
	public void testPreferencesToNetworkTables(){
		String networkedNumber = "networkCheckedValue";
		int networkNumberValue = 100;
		pref.putInt(networkedNumber, networkNumberValue);
		assertEquals((new Integer(networkNumberValue).toString()), prefTable.getString(networkedNumber));
		pref.remove(networkedNumber);
	}

}
