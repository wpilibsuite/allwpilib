// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TunableTest {
  MockTunableBackend m_mock;

  @BeforeEach
  public void init() {
    m_mock = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  public void shutdown() {
    TunableRegistry.reset();
  }

  @Test
  void testIntTunable() {
    TunableInt tunable = TunableInt.create();
    tunable.set(42);
    int val = tunable.get();
    assertEquals(val, 42);
    Tunables.publish("test", tunable);
    m_mock.setInt("/test", 84);
    TunableRegistry.update();
    val = tunable.get();
    assertEquals(val, 84);
  }
}
