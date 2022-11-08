// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;
import static org.junit.jupiter.api.parallel.ExecutionMode.SAME_THREAD;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.Topic;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.junit.jupiter.api.parallel.Execution;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

@Execution(SAME_THREAD)
class PreferencesTest {
  private NetworkTableInstance m_inst;
  private NetworkTable m_table;

  private static final String kFilename = "networktables.json";

  @BeforeEach
  void setup(@TempDir Path tempDir) {
    m_inst = NetworkTableInstance.create();
    m_table = m_inst.getTable("Preferences");
    Preferences.setNetworkTableInstance(m_inst);

    Path filepath = tempDir.resolve(kFilename);
    try (InputStream is = getClass().getResource("PreferencesTestDefault.json").openStream()) {
      Files.copy(is, filepath);
    } catch (IOException ex) {
      fail(ex);
    }

    m_inst.startServer(filepath.toString(), "", 0, 0);
    try {
      int count = 0;
      while (m_inst.getNetworkMode().contains(NetworkTableInstance.NetworkMode.kStarting)) {
        Thread.sleep(100);
        count++;
        if (count > 30) {
          throw new InterruptedException();
        }
      }
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for server to start");
    }
  }

  @AfterEach
  void cleanup() {
    m_inst.close();
  }

  @Test
  void removeAllTest() {
    Preferences.removeAll();

    List<String> keys = new ArrayList<>();
    boolean anyPersistent = false;
    for (Topic topic : m_table.getTopics()) {
      if (topic.isPersistent()) {
        anyPersistent = true;
        keys.add(topic.getName());
      }
    }

    assertFalse(
        anyPersistent,
        "Preferences was not empty!  Preferences in table: " + Arrays.toString(keys.toArray()));
  }

  @ParameterizedTest
  @MethodSource("defaultKeyProvider")
  void defaultKeysTest(String key) {
    assertTrue(Preferences.containsKey(key));
  }

  @ParameterizedTest
  @MethodSource("defaultKeyProvider")
  void defaultKeysAllTest(String key) {
    assertTrue(Preferences.getKeys().contains(key));
  }

  @Test
  void defaultValueTest() {
    assertAll(
        () -> assertEquals(172L, Preferences.getLong("checkedValueLong", 0)),
        () -> assertEquals(0.2, Preferences.getDouble("checkedValueDouble", 0), 1e-6),
        () -> assertEquals("Hello. How are you?", Preferences.getString("checkedValueString", "")),
        () -> assertEquals(2, Preferences.getInt("checkedValueInt", 0)),
        () -> assertEquals(3.4, Preferences.getFloat("checkedValueFloat", 0), 1e-6),
        () -> assertFalse(Preferences.getBoolean("checkedValueBoolean", true)));
  }

  @Nested
  class PutGetTests {
    @Test
    void intTest() {
      final String key = "test";
      final int value = 123;

      Preferences.setInt(key, value);

      assertAll(
          () -> assertEquals(value, Preferences.getInt(key, -1)),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).intValue()));
    }

    @Test
    void longTest() {
      final String key = "test";
      final long value = 190L;

      Preferences.setLong(key, value);

      assertAll(
          () -> assertEquals(value, Preferences.getLong(key, -1)),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).longValue()));
    }

    @Test
    void floatTest() {
      final String key = "test";
      final float value = 9.42f;

      Preferences.setFloat(key, value);

      assertAll(
          () -> assertEquals(value, Preferences.getFloat(key, -1), 1e-6),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).floatValue(), 1e-6));
    }

    @Test
    void doubleTest() {
      final String key = "test";
      final double value = 6.28;

      Preferences.setDouble(key, value);

      assertAll(
          () -> assertEquals(value, Preferences.getDouble(key, -1), 1e-6),
          () -> assertEquals(value, m_table.getEntry(key).getNumber(-1).doubleValue(), 1e-6));
    }

    @Test
    void stringTest() {
      final String key = "test";
      final String value = "value";

      Preferences.setString(key, value);

      assertAll(
          () -> assertEquals(value, Preferences.getString(key, "")),
          () -> assertEquals(value, m_table.getEntry(key).getString("")));
    }

    @Test
    void booleanTest() {
      final String key = "test";
      final boolean value = true;

      Preferences.setBoolean(key, value);

      assertAll(
          () -> assertEquals(value, Preferences.getBoolean(key, false)),
          () -> assertEquals(value, m_table.getEntry(key).getBoolean(false)));
    }
  }

  static Stream<String> defaultKeyProvider() {
    return Stream.of(
        "checkedValueLong",
        "checkedValueDouble",
        "checkedValueString",
        "checkedValueInt",
        "checkedValueFloat",
        "checkedValueBoolean");
  }
}
