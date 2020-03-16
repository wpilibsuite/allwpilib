package edu.wpi.first.wpiutil.math;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.io.IOException;

public class DrakeJNITest {

  @Test
  public void testLink() {
    try {
      DrakeJNI.forceLoad();
    } catch (IOException e) {
      e.printStackTrace();
      Assertions.fail();
    }
  }
}
