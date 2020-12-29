// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Set;
import java.util.stream.Stream;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

class PreferencesTest {
  private final Preferences m_prefs = Preferences.getInstance();
  private final NetworkTable m_table = NetworkTableInstance.getDefault().getTable("Preferences");

  private static final String kFilename = "networktables.ini";

  @BeforeAll
  static void setupAll() {
    NetworkTableInstance.getDefault().stopServer();
  }

  @BeforeEach
  void setup(@TempDir Path tempDir) {
    m_table.getKeys().forEach(m_table::delete);

    Path filepath = tempDir.resolve(kFilename);
    try (InputStream is = getClass().getResource("PreferencesTestDefault.ini").openStream()) {
      Files.copy(is, filepath);
    } catch (IOException ex) {
      fail(ex);
    }

    NetworkTableInstance.getDefault().startServer(filepath.toString());
  }

  @AfterEach
  void cleanup() {
    NetworkTableInstance.getDefault().stopServer();
  }

  @AfterAll
  static void cleanupAll() {
    NetworkTableInstance.getDefault().startServer();
  }

  @Test
  void removeAllTest() {
    m_prefs.removeAll();

    Set<String> keys = m_table.getKeys();
    keys.remove(".type");

    assertTrue(keys.isEmpty(), "Preferences was not empty!  Preferences in table: "
        + Arrays.toString(keys.toArray()));
  }

  @ParameterizedTest
  @MethodSource("defaultKeyProvider")
  void defaultKeysTest(String key) {
    assertTrue(m_prefs.containsKey(key));
  }

  @ParameterizedTest
  @MethodSource("defaultKeyProvider")
  void defaultKeysAllTest(String key) {
    assertTrue(m_prefs.getKeys().contains(key));
  }

  @Test
  void defaultValueTest() {
    assertAll(
        () -> assertEquals(172L, m_prefs.getLong("checkedValueLong", 0)),
        () -> assertEquals(0.2, m_prefs.getDouble("checkedValueDouble", 0), 1e-6),
        () -> assertEquals("Hello. How are you?", m_prefs.getString("checkedValueString", "")),
        () -> assertEquals(2, m_prefs.getInt("checkedValueInt", 0)),
        () -> assertEquals(3.14, m_prefs.getFloat("checkedValueFloat", 0), 1e-6),
        () -> assertFalse(m_prefs.getBoolean("checkedValueBoolean", true))
    );
  }

  @Test
  void backupTest() {
    m_prefs.removeAll();

    assertAll(
        () -> assertEquals(0, m_prefs.getLong("checkedValueLong", 0)),
        () -> assertEquals(0, m_prefs.getDouble("checkedValueDouble", 0), 1e-6),
        () -> assertEquals("", m_prefs.getString("checkedValueString", "")),
        () -> assertEquals(0, m_prefs.getInt("checkedValueInt", 0)),
        () -> assertEquals(0, m_prefs.getFloat("checkedValueFloat", 0), 1e-6),
        () -> assertTrue(m_prefs.getBoolean("checkedValueBoolean", true))
    );
  }

  @Nested
  class PutGetTests {
    @Test
    void intTest() {
      final String key = "test";
      final int value = 123;

      m_prefs.putInt(key, value);

      assertAll(
          () -> assertEquals(value, m_prefs.getInt(key, -1)),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).intValue())
      );
    }

    @Test
    void longTest() {
      final String key = "test";
      final long value = 190L;

      m_prefs.putLong(key, value);

      assertAll(
          () -> assertEquals(value, m_prefs.getLong(key, -1)),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).longValue())
      );
    }

    @Test
    void floatTest() {
      final String key = "test";
      final float value = 9.42f;

      m_prefs.putFloat(key, value);

      assertAll(
          () -> assertEquals(value, m_prefs.getFloat(key, -1), 1e-6),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).floatValue(), 1e-6)
      );
    }

    @Test
    void doubleTest() {
      final String key = "test";
      final double value = 6.28;

      m_prefs.putDouble(key, value);

      assertAll(
          () -> assertEquals(value, m_prefs.getDouble(key, -1), 1e-6),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).doubleValue(), 1e-6)
      );
    }

    @Test
    void stringTest() {
      final String key = "test";
      final String value = "value";

      m_prefs.putString(key, value);

      assertAll(
          () -> assertEquals(value, m_prefs.getString(key, "")),
          () -> assertEquals(value, m_table.getEntry(key).getString(""))
      );
    }

    @Test
    void booleanTest() {
      final String key = "test";
      final boolean value = true;

      m_prefs.putBoolean(key, value);

      assertAll(
          () -> assertEquals(value, m_prefs.getBoolean(key, false)),
          () -> assertEquals(value, m_table.getEntry(key).getBoolean(false))
      );
    }
  }

  static Stream<String> defaultKeyProvider() {
    return Stream.of(
        "checkedValueLong",
        "checkedValueDouble",
        "checkedValueString",
        "checkedValueInt",
        "checkedValueFloat",
        "checkedValueBoolean"
    );
  }
}
