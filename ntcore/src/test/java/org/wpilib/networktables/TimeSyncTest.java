// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
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

      m_inst.startServer("timesynctest.json", "127.0.0.1", "", 10030);
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

  @Test
  void testServerClientProtocolVersion() throws InterruptedException {
    try (var client = NetworkTableInstance.create()) {
      m_inst.startServer("", " 127.0.0.1 ", "", 10032);
      client.setServer("127.0.0.1", 10032);
      client.startClient("timesync-test");
      for (int i = 0;
          i < 100 && (m_inst.getConnections().length == 0 || !client.isConnected());
          i++) {
        Thread.sleep(50);
      }
      var serverConnections = m_inst.getConnections();
      var clientConnections = client.getConnections();
      assertEquals(1, serverConnections.length);
      assertEquals(1, clientConnections.length);
      assertEquals(0x0402, serverConnections[0].protocolVersion);
      assertEquals(0x0402, clientConnections[0].protocolVersion);
      for (int i = 0; i < 100 && client.getServerTimeOffset().isEmpty(); i++) {
        Thread.sleep(50);
      }
      assertTrue(client.getServerTimeOffset().isPresent());
    }
  }

  @Test
  void testServerWireTimestampsNanoseconds() throws IOException {
    m_inst.startServer("", "127.0.0.1", "", 10035);
    try (var socket = new DatagramSocket()) {
      socket.setSoTimeout(2000);
      long clientTimeNs = 123_456_789;
      var ping = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN);
      ping.put((byte) 1).put((byte) 1).putLong(clientTimeNs);
      long before = NetworkTablesJNI.now();
      socket.send(new DatagramPacket(ping.array(), 10, InetAddress.getByName("127.0.0.1"), 10035));
      var response = new DatagramPacket(new byte[18], 18);
      socket.receive(response);
      long after = NetworkTablesJNI.now();
      assertEquals(18, response.getLength());
      var pong = ByteBuffer.wrap(response.getData()).order(ByteOrder.LITTLE_ENDIAN);
      assertEquals(1, pong.get());
      assertEquals(2, pong.get());
      assertEquals(clientTimeNs, pong.getLong());
      long serverTimeNs = pong.getLong();
      assertTrue(serverTimeNs >= before);
      assertTrue(serverTimeNs <= after);
    }
  }
}
