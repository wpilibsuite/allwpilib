package edu.wpi.first.wpiutil;

import org.junit.jupiter.api.Test;

public class JNITest {
  @Test
  void jniLinkTest() {
    // Test to verify that the JNI test link works correctly.
    WPIUtilJNI.removePortForwarder(1234);
  }
}
