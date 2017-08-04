/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import org.junit.Test;

public class JNITest {
    @Test
    public void jniNtcoreLinkTest() {
        // Test to verify that the JNI test link works correctly.
        edu.wpi.first.wpilibj.networktables.NetworkTablesJNI.flush();
    }

    @Test
    public void jniHalLinkTest() {
        // Test to verify that the JNI test link works correctly.
        HALUtil.getHALRuntimeType();
    }
}
