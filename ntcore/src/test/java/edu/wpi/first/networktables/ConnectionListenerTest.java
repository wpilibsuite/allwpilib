// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNotSame;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.util.WPIUtilJNI;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class ConnectionListenerTest {
  private NetworkTableInstance m_serverInst;
  private NetworkTableInstance m_clientInst;

  @BeforeEach
  void setUp() {
    m_serverInst = NetworkTableInstance.create();
    m_clientInst = NetworkTableInstance.create();
  }

  @AfterEach
  void tearDown() {
    m_clientInst.close();
    m_serverInst.close();
  }

  /** Connect to the server. */
  private void connect(int port) {
    m_serverInst.startServer("connectionlistenertest.json", "127.0.0.1", 0, port);
    m_clientInst.startClient4("client");
    m_clientInst.setServer("127.0.0.1", port);

    // wait for client to report it's connected, then wait another 0.1 sec
    try {
      int count = 0;
      while (!m_clientInst.isConnected()) {
        Thread.sleep(100);
        count++;
        if (count > 30) {
          throw new InterruptedException();
        }
      }
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to start");
    }
  }

  @Test
  void testJNI() {
    // set up the poller
    int poller = NetworkTablesJNI.createListenerPoller(m_serverInst.getHandle());
    assertNotSame(poller, 0, "bad poller handle");
    int handle =
        NetworkTablesJNI.addListener(
            poller, m_serverInst.getHandle(), EnumSet.of(NetworkTableEvent.Kind.kConnection));
    assertNotSame(handle, 0, "bad listener handle");

    // trigger a connect event
    connect(10020);

    // get the event
    try {
      assertFalse(WPIUtilJNI.waitForObjectTimeout(handle, 1.0));
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for queue");
    }
    NetworkTableEvent[] events = NetworkTablesJNI.readListenerQueue(m_serverInst, poller);

    assertNotNull(events);
    assertEquals(1, events.length);
    assertEquals(handle, events[0].listener);
    assertNotNull(events[0].connInfo);
    assertTrue(events[0].is(NetworkTableEvent.Kind.kConnected));

    // trigger a disconnect event
    m_clientInst.stopClient();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to stop");
    }

    // get the event
    try {
      assertFalse(WPIUtilJNI.waitForObjectTimeout(handle, 1.0));
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for queue");
    }
    events = NetworkTablesJNI.readListenerQueue(m_serverInst, poller);

    assertNotNull(events);
    assertEquals(1, events.length);
    assertEquals(handle, events[0].listener);
    assertTrue(events[0].is(NetworkTableEvent.Kind.kDisconnected));
  }

  private static int threadedPort = 10001;

  @ParameterizedTest
  @ValueSource(strings = {"127.0.0.1", "127.0.0.1 ", " 127.0.0.1 "})
  void testThreaded(String address) {
    m_serverInst.startServer("connectionlistenertest.json", address, 0, threadedPort);
    List<NetworkTableEvent> events = new ArrayList<>();
    final int handle =
        m_serverInst.addConnectionListener(
            false,
            e -> {
              synchronized (events) {
                events.add(e);
              }
            });

    // trigger a connect event
    m_clientInst.startClient4("client");
    m_clientInst.setServer(address, threadedPort);
    threadedPort++;

    // wait for client to report it's connected, then wait another 0.1 sec
    try {
      int count = 0;
      while (!m_clientInst.isConnected()) {
        Thread.sleep(100);
        count++;
        if (count > 30) {
          throw new InterruptedException();
        }
      }
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to start");
    }
    try {
      assertFalse(WPIUtilJNI.waitForObjectTimeout(handle, 1.0));
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for queue");
    }

    // wait for thread
    m_serverInst.waitForListenerQueue(1.0);

    // get the event
    synchronized (events) {
      assertEquals(1, events.size());
      assertEquals(handle, events.get(0).listener);
      assertNotNull(events.get(0).connInfo);
      assertTrue(events.get(0).is(NetworkTableEvent.Kind.kConnected));
      events.clear();
    }

    // trigger a disconnect event
    m_clientInst.stopClient();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to stop");
    }

    // wait for thread
    m_serverInst.waitForListenerQueue(1.0);

    // get the event
    try {
      assertFalse(WPIUtilJNI.waitForObjectTimeout(handle, 1.0));
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for queue");
    }
    synchronized (events) {
      assertEquals(1, events.size());
      assertEquals(handle, events.get(0).listener);
      assertNotNull(events.get(0).connInfo);
      assertTrue(events.get(0).is(NetworkTableEvent.Kind.kDisconnected));
    }
  }
}
