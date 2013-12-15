/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.Preferences;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * 
 * @author Fred
 */
public class PreferencesTest extends TestClass {

	private Preferences pref;
	private long check;

	public String getName() {
		return "Preferences test";
	}

	public String[] getTags() {
		return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
				RunTests.Tags.Type.PREFERENCES };
	}

	public void setup() {
		try {
			File file = new File("file:///wpilib-preferences.ini");
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
		check = System.currentTimeMillis();
	}

	public void teardown() {
	}

	{
		new Test("Check preferences file operations") {

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

			public void run() {
				assertEquals(pref.getLong("checkedValueLong", 0), 172);
				assertEquals(pref.getDouble("checkedValueDouble", 0), .2);
				assertEquals(pref.getString("checkedValueString", ""),
						"hello \nHow are you ?");
				assertEquals(pref.getInt("checkedValueInt", 0), 2);
				assertEquals(pref.getFloat("checkedValueFloat", 0), 3.14, .001);
				assertFalse(pref.getBoolean("checkedValueBoolean", true));
				remove();
				assertEquals(pref.getLong("checkedValueLong", 0), 0);
				assertEquals(pref.getDouble("checkedValueDouble", 0), 0);
				assertEquals(pref.getString("checkedValueString", ""), "");
				assertEquals(pref.getInt("checkedValueInt", 0), 0);
				assertEquals(pref.getFloat("checkedValueFloat", 0), 0);
				assertFalse(pref.getBoolean("checkedValueBoolean", false));
				addCheckedValue();
				pref.save();
				assertEquals(check, pref.getLong("checkedValueLong", 0));
				assertEquals(pref.getDouble("checkedValueDouble", 0), 1);
				assertEquals(pref.getString("checkedValueString", ""),
						"checked");
				assertEquals(pref.getInt("checkedValueInt", 0), 1);
				assertEquals(pref.getFloat("checkedValueFloat", 0), 1);
				assertTrue(pref.getBoolean("checkedValueBoolean", false));
			}
		};
	}
}
