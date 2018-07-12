package edu.wpi.first.wpilibj.shuffleboard;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.UtilityClassTest;

import static org.junit.jupiter.api.Assertions.assertSame;

public class ShuffleboardTest extends UtilityClassTest {

  public ShuffleboardTest() {
    super(Shuffleboard.class);
  }

  // Most relevant tests are in ShuffleboardTabTest

  @Test
  public void testTabObjectsCached() {
    ShuffleboardTab tab1 = Shuffleboard.getTab("testTabObjectsCached");
    ShuffleboardTab tab2 = Shuffleboard.getTab("testTabObjectsCached");
    assertSame(tab1, tab2, "Tab objects were not cached");
  }

}
