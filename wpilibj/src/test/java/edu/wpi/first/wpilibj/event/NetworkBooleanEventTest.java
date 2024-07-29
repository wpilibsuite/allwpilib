// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.networktables.NetworkTableInstance;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class NetworkBooleanEventTest {
  NetworkTableInstance m_inst;

  @BeforeEach
  void setup() {
    m_inst = NetworkTableInstance.create();
    m_inst.startLocal();
  }

  @AfterEach
  void teardown() {
    m_inst.close();
  }

  @Test
  void testNetworkBooleanEvent() {
    var loop = new EventLoop();
    var counter = new AtomicInteger(0);

    var pub = m_inst.getTable("TestTable").getBooleanTopic("Test").publish();

    new NetworkBooleanEvent(loop, m_inst, "TestTable", "Test").ifHigh(counter::incrementAndGet);
    pub.set(false);
    loop.poll();
    assertEquals(0, counter.get());
    pub.set(true);
    loop.poll();
    assertEquals(1, counter.get());
    pub.set(false);
    loop.poll();
    assertEquals(1, counter.get());
  }
}
