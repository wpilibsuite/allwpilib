package edu.wpi.first.wpilibj.networktables;

import org.junit.Test;

public class JNITest {
    @Test
    public void jniLinkTest() {
        // Test to verify that the JNI test link works correctly.
        edu.wpi.first.wpilibj.networktables.NetworkTablesJNI.flush();
    }
}
