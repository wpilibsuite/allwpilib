/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.networktables.NetworkTablesJNI;
import org.junit.Test;

public class JNITest {
  @Test
  public void jniNtcoreLinkTest() {
    // Test to verify that the JNI test link works correctly.
    NetworkTablesJNI.flush(NetworkTablesJNI.getDefaultInstance());
  }

  @Test
  public void jniHalLinkTest() {
    HAL.initialize(500, 0);
    // Test to verify that the JNI test link works correctly.
    HALUtil.getHALRuntimeType();
  }
}
