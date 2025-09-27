// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.cleanup;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;

class CleanupPoolTest {
  static class AutoCloseableObject implements AutoCloseable {
    public boolean m_closed;

    @Override
    public void close() {
      m_closed = true;
    }
  }

  static class AutoCloseableObjectWithCallback implements AutoCloseable {
    private final Runnable m_cb;

    AutoCloseableObjectWithCallback(Runnable cb) {
      m_cb = cb;
    }

    @Override
    public void close() {
      m_cb.run();
    }
  }

  static class FailingAutoCloseableObject implements AutoCloseable {
    public static final String message = "This is an expected failure";

    @Override
    public void close() {
      throw new RuntimeException(message);
    }
  }

  @Test
  void cleanupStackWorks() {
    List<AutoCloseableObject> objects = new ArrayList<>();
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());

    try (CleanupPool pool = new CleanupPool()) {
      for (AutoCloseableObject autoCloseableObject : objects) {
        pool.register(autoCloseableObject);
      }
    }

    for (AutoCloseableObject autoCloseableObject : objects) {
      assertTrue(autoCloseableObject.m_closed);
    }
  }

  @Test
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  void cleanupStackWithExceptionNotInCloseWorks() {
    List<AutoCloseableObject> objects = new ArrayList<>();
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());

    String message = "This is a known failure";

    try (CleanupPool pool = new CleanupPool()) {
      for (AutoCloseableObject autoCloseableObject : objects) {
        pool.register(autoCloseableObject);
      }
      throw new Exception(message);
    } catch (Exception e) {
      assertEquals(message, e.getMessage());
    }

    for (AutoCloseableObject autoCloseableObject : objects) {
      assertTrue(autoCloseableObject.m_closed);
    }
  }

  @Test
  void cleanupStackWithExceptionInCloseWorks() {
    List<AutoCloseableObject> objects = new ArrayList<>();
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());

    try (CleanupPool pool = new CleanupPool()) {
      for (AutoCloseableObject autoCloseableObject : objects) {
        pool.register(new FailingAutoCloseableObject());
        pool.register(autoCloseableObject);
      }
    }

    for (AutoCloseableObject autoCloseableObject : objects) {
      assertTrue(autoCloseableObject.m_closed);
    }
  }

  @Test
  void cleanupStackRemovalWorks() {
    List<AutoCloseableObject> objects = new ArrayList<>();
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());
    objects.add(new AutoCloseableObject());

    try (CleanupPool pool = new CleanupPool()) {
      for (AutoCloseableObject autoCloseableObject : objects) {
        pool.register(autoCloseableObject);
      }

      pool.remove(objects.get(0));
    }

    int idx = 0;
    for (AutoCloseableObject autoCloseableObject : objects) {
      if (idx == 0) {
        assertFalse(autoCloseableObject.m_closed);
      } else {
        assertTrue(autoCloseableObject.m_closed);
      }
      idx++;
    }
  }

  @Test
  void cleanupStackIsLifo() {
    List<AutoCloseableObjectWithCallback> objects = new ArrayList<>();
    List<Integer> order = new ArrayList<>();
    objects.add(new AutoCloseableObjectWithCallback(() -> order.add(0)));
    objects.add(new AutoCloseableObjectWithCallback(() -> order.add(1)));
    objects.add(new AutoCloseableObjectWithCallback(() -> order.add(2)));

    try (CleanupPool pool = new CleanupPool()) {
      for (AutoCloseable autoCloseableObject : objects) {
        pool.register(autoCloseableObject);
      }
    }

    assertEquals(order.size(), 3);
    assertEquals(order.get(0), 2);
    assertEquals(order.get(1), 1);
    assertEquals(order.get(2), 0);
  }
}
