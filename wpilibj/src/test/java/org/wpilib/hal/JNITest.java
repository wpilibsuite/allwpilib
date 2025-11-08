// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.HALUtil;
import edu.wpi.first.networktables.NetworkTablesJNI;
import org.junit.jupiter.api.Test;

class JNITest {
  @Test
  void jniNtcoreLinkTest() {
    // Test to verify that the JNI test link works correctly.
    NetworkTablesJNI.flush(NetworkTablesJNI.getDefaultInstance());
  }

  @Test
  void jniHalLinkTest() {
    HAL.initialize(500, 0);
    // Test to verify that the JNI test link works correctly.
    HALUtil.getHALRuntimeType();
  }
}
