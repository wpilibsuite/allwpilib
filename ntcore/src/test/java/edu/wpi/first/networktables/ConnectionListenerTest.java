/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

public class ConnectionListenerTest extends TestCase {
  NetworkTableInstance m_serverInst;
  NetworkTableInstance m_clientInst;

  @Override
  protected void setUp() throws Exception {
    m_serverInst = NetworkTableInstance.create();
    m_serverInst.setNetworkIdentity("server");

    m_clientInst = NetworkTableInstance.create();
    m_clientInst.setNetworkIdentity("client");
  }

  @Override
  protected void tearDown() throws Exception {
    m_clientInst.close();
    m_serverInst.close();
  }

  /**
   * Connect to the server.
   */
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

  /**
   * Test the JNI.
   */
  public void ignoreJNI() {
    // set up the poller
    int poller = NetworkTablesJNI.createConnectionListenerPoller(m_serverInst.getHandle());
    assertTrue("bad poller handle", poller != 0);
    int handle = NetworkTablesJNI.addPolledConnectionListener(poller, false);
    assertTrue("bad listener handle", handle != 0);

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
    assertEquals(events.length, 1);
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
    assertEquals(events.length, 1);
    assertEquals(handle, events[0].listener);
    assertFalse(events[0].connected);

  }

  /**
   * Test threaded behavior.
   */
  public void ignoreThreaded() {
    m_serverInst.startServer("connectionlistenertest.ini", "127.0.0.1", 10000);
    List<ConnectionNotification> events = new ArrayList<>();
    final int handle = m_serverInst.addConnectionListener(events::add, false);

    // trigger a connect event
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
    assertTrue(m_serverInst.waitForConnectionListenerQueue(1.0));

    // get the event
    assertEquals(events.size(), 1);
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
    assertEquals(events.size(), 1);
    assertEquals(handle, events.get(0).listener);
    assertFalse(events.get(0).connected);
  }
}
