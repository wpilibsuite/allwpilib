package edu.wpi.first.networktables;

import org.junit.Test;

public class JNITest {
  @Test
  public void jniLinkTest() {
    // Test to verify that the JNI test link works correctly.
    int inst = NetworkTablesJNI.getDefaultInstance();
    NetworkTablesJNI.flush(inst);
  }
}
