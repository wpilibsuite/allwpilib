// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;

import org.junit.jupiter.api.Test;

class JNITest {
  @Test
  void jniLinkTest() {
    // Test to verify that the JNI test link works correctly.
    int inst = NetworkTablesJNI.getDefaultInstance();
    NetworkTablesJNI.flush(inst);
  }

  @Test
  void stringArrayAtomicCreatesTimestampedValue() {
    NetworkTableInstance inst = NetworkTableInstance.create();
    try (StringArrayEntry entry =
        inst.getStringArrayTopic("stringArray").getEntry(new String[] {})) {
      entry.set(new String[] {"hello", "there"});

      TimestampedStringArray value = entry.getAtomic();

      assertArrayEquals(new String[] {"hello", "there"}, value.value);
    } finally {
      inst.close();
    }
  }
}
