package edu.wpi.first.wpiutil.math;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

public class DrakeJNITest {

  @Test
  public void testLink() {
    assertDoesNotThrow(DrakeJNI::forceLoad);
  }
}
