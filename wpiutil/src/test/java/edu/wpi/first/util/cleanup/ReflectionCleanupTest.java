// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.cleanup;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class ReflectionCleanupTest {
  static class CleanupClass implements AutoCloseable {
    public boolean m_closed;

    @Override
    public void close() {
      m_closed = true;
    }
  }

  @SuppressWarnings("PMD.TestClassWithoutTestCases")
  static class CleanupTest implements ReflectionCleanup {
    public CleanupClass m_class1 = new CleanupClass();
    public CleanupClass m_class2 = new CleanupClass();
    public Object m_nonCleanupObject = new Object();
    public Object m_nullCleanupObject;

    @Override
    public void close() {
      reflectionCleanup(CleanupTest.class);
    }
  }

  static class CleanupTest2 extends CleanupTest {
    @SkipCleanup public CleanupClass m_class3 = new CleanupClass();
    public CleanupClass m_class4 = new CleanupClass();

    @Override
    public void close() {
      reflectionCleanup(CleanupTest2.class);
    }
  }

  @Test
  void cleanupClosesAllFields() {
    CleanupTest test = new CleanupTest();
    test.close();
    assertTrue(test.m_class1.m_closed);
    assertTrue(test.m_class2.m_closed);
  }

  @Test
  void cleanupOnlyClosesExplicitClassAndSkipWorks() {
    CleanupTest2 test = new CleanupTest2();
    test.close();
    assertFalse(test.m_class1.m_closed);
    assertFalse(test.m_class2.m_closed);
    assertFalse(test.m_class3.m_closed);
    assertTrue(test.m_class4.m_closed);
  }
}
