/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.ArrayList;
import java.util.List;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.condition.DisabledOnOs;
import org.junit.jupiter.api.condition.OS;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNotSame;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

class ConnectionListenerTest {
  private NetworkTableInstance m_serverInst;
  private NetworkTableInstance m_clientInst;

  @BeforeEach
  void setUp() {
    m_serverInst = NetworkTableInstance.create();
    m_serverInst.setNetworkIdentity("server");

    m_clientInst = NetworkTableInstance.create();
    m_clientInst.setNetworkIdentity("client");
  }

  @AfterEach
  void tearDown() {
    m_clientInst.close();
    m_serverInst.close();
  }

  /**
   * Connect to the server.
   */
  @SuppressWarnings("PMD.AvoidUsingHardCodedIP")
  private void connect() {
    m_serverInst.startServer("connectionlistenertest.ini", "127.0.0.1", 10000);
    m_clientInst.startClient("127.0.0.1", 10000);

    // wait for client to report it's started, then wait another 0.1 sec
    try {
      while ((m_clientInst.getNetworkMode() & NetworkTableInstance.kNetModeStarting) != 0) {
        Thread.sleep(100);
      }
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to start");
    }
  }

  @Test
  @DisabledOnOs(OS.WINDOWS)
  void testJNI() {
    // set up the poller
    int poller = NetworkTablesJNI.createConnectionListenerPoller(m_serverInst.getHandle());
    assertNotSame(poller, 0, "bad poller handle");
    int handle = NetworkTablesJNI.addPolledConnectionListener(poller, false);
    assertNotSame(handle, 0, "bad listener handle");

    // trigger a connect event
    connect();

    // get the event
    assertTrue(m_serverInst.waitForConnectionListenerQueue(1.0));
    ConnectionNotification[] events = null;
    try {
      events = NetworkTablesJNI.pollConnectionListenerTimeout(m_serverInst, poller, 0.0);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("unexpected interrupted exception" + ex);
    }

    assertNotNull(events);
    assertEquals(1, events.length);
    assertEquals(handle, events[0].listener);
    assertTrue(events[0].connected);

    // trigger a disconnect event
    m_clientInst.stopClient();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to stop");
    }

    // get the event
    assertTrue(m_serverInst.waitForConnectionListenerQueue(1.0));
    try {
      events = NetworkTablesJNI.pollConnectionListenerTimeout(m_serverInst, poller, 0.0);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("unexpected interrupted exception" + ex);
    }

    assertNotNull(events);
    assertEquals(1, events.length);
    assertEquals(handle, events[0].listener);
    assertFalse(events[0].connected);

  }

  @ParameterizedTest
  @DisabledOnOs(OS.WINDOWS)
  @SuppressWarnings("PMD.AvoidUsingHardCodedIP")
  @ValueSource(strings = { "127.0.0.1", "127.0.0.1 ", " 127.0.0.1 " })
  void testThreaded(String address) {
    m_serverInst.startServer("connectionlistenertest.ini", address, 10000);
    List<ConnectionNotification> events = new ArrayList<>();
    final int handle = m_serverInst.addConnectionListener(events::add, false);

    // trigger a connect event
    m_clientInst.startClient(address, 10000);

    // wait for client to report it's started, then wait another 0.1 sec
    try {
      while ((m_clientInst.getNetworkMode() & NetworkTableInstance.kNetModeStarting) != 0) {
        Thread.sleep(100);
      }
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to start");
    }
    assertTrue(m_serverInst.waitForConnectionListenerQueue(1.0));

    // get the event
    assertEquals(1, events.size());
    assertEquals(handle, events.get(0).listener);
    assertTrue(events.get(0).connected);
    events.clear();

    // trigger a disconnect event
    m_clientInst.stopClient();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to stop");
    }

    // get the event
    assertTrue(m_serverInst.waitForConnectionListenerQueue(1.0));
    assertEquals(1, events.size());
    assertEquals(handle, events.get(0).listener);
    assertFalse(events.get(0).connected);
  }
}
