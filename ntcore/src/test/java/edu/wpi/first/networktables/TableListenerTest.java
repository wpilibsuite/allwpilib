// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.EnumSet;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TableListenerTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
  }

  @AfterEach
  void tearDown() {
    m_inst.close();
  }

  private void publishTopics() {
    m_inst.getDoubleTopic("/foo/foovalue").publish();
    m_inst.getDoubleTopic("/foo/bar/barvalue").publish();
    m_inst.getDoubleTopic("/baz/bazvalue").publish();
  }

  @Test
  void testAddListener() {
    NetworkTable table = m_inst.getTable("/foo");
    AtomicInteger count = new AtomicInteger();
    table.addListener(
        EnumSet.of(NetworkTableEvent.Kind.kTopic),
        (atable, key, event) -> {
          count.incrementAndGet();
          assertEquals(atable, table);
          assertEquals(key, "foovalue");
        });
    publishTopics();
    assertTrue(m_inst.waitForListenerQueue(1.0));
    assertEquals(count.get(), 1);
  }

  @Test
  void testAddSubTableListener() {
    NetworkTable table = m_inst.getTable("/foo");
    AtomicInteger count = new AtomicInteger();
    table.addSubTableListener(
        (atable, key, event) -> {
          count.incrementAndGet();
          assertEquals(atable, table);
          assertEquals(key, "bar");
        });
    publishTopics();
    assertTrue(m_inst.waitForListenerQueue(1.0));
    assertEquals(count.get(), 1);
  }
}
