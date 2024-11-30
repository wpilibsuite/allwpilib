// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

import java.util.List;
import org.junit.jupiter.api.Test;

class LazyBackendTest {
  @Test
  void lazyOfLazyReturnsSelf() {
    var lazy = new LazyBackend(new NullBackend());
    assertSame(lazy, lazy.lazy());
  }

  @Test
  void lazyInt() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    {
      // First time logging to "int" should go through
      lazy.log("int", 0);
      assertEquals(List.of(new TestBackend.LogEntry<>("int", 0)), backend.getEntries());
    }

    {
      // Logging the current value shouldn't go through
      lazy.log("int", 0);
      assertEquals(List.of(new TestBackend.LogEntry<>("int", 0)), backend.getEntries());
    }

    {
      // Logging a new value should go through
      lazy.log("int", 1);
      assertEquals(
          List.of(new TestBackend.LogEntry<>("int", 0), new TestBackend.LogEntry<>("int", 1)),
          backend.getEntries());
    }

    {
      // Logging a previous value should go through
      lazy.log("int", 0);
      assertEquals(
          List.of(
              new TestBackend.LogEntry<>("int", 0),
              new TestBackend.LogEntry<>("int", 1),
              new TestBackend.LogEntry<>("int", 0)),
          backend.getEntries());
    }
  }
}
