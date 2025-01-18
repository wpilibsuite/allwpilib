// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TimeSyncTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
  }

  @AfterEach
  void tearDown() {
    m_inst.close();
  }

  @Test
  void testLocal() {
    var offset = m_inst.getServerTimeOffset();
    assertFalse(offset.isPresent());
  }

  @Test
  void testServer() {
    try (var poller = new NetworkTableListenerPoller(m_inst)) {
      poller.addTimeSyncListener(false);

      m_inst.startServer("timesynctest.json", "127.0.0.1", 10030);
      var offset = m_inst.getServerTimeOffset();
      assertTrue(offset.isPresent());
      assertEquals(0L, offset.getAsLong());

      NetworkTableEvent[] events = poller.readQueue();
      assertEquals(1, events.length);
      assertNotNull(events[0].timeSyncData);
      assertTrue(events[0].timeSyncData.valid);
      assertEquals(0L, events[0].timeSyncData.serverTimeOffset);
      assertEquals(0L, events[0].timeSyncData.rtt2);

      m_inst.stopServer();
      offset = m_inst.getServerTimeOffset();
      assertFalse(offset.isPresent());

      events = poller.readQueue();
      assertEquals(1, events.length);
      assertNotNull(events[0].timeSyncData);
      assertFalse(events[0].timeSyncData.valid);
    }
  }

  @Test
  void testClient() {
    m_inst.startClient("client");
    var offset = m_inst.getServerTimeOffset();
    assertFalse(offset.isPresent());

    m_inst.stopClient();
    offset = m_inst.getServerTimeOffset();
    assertFalse(offset.isPresent());
  }
}
