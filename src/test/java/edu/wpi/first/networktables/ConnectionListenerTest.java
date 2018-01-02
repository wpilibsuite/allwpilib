/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.ArrayList;
import java.util.List;
import junit.framework.TestCase;

public class ConnectionListenerTest extends TestCase {
  NetworkTableInstance serverInst;
  NetworkTableInstance clientInst;

  @Override
  protected void setUp() throws Exception {
    serverInst = NetworkTableInstance.create();
    serverInst.setNetworkIdentity("server");

    clientInst = NetworkTableInstance.create();
    clientInst.setNetworkIdentity("client");
  }

  @Override
  protected void tearDown() throws Exception {
    clientInst.free();
    serverInst.free();
  }

  private void connect() {
    serverInst.startServer("connectionlistenertest.ini", "127.0.0.1", 10000);
    clientInst.startClient("127.0.0.1", 10000);

    // wait for client to report it's started, then wait another 0.1 sec
    try {
      while ((clientInst.getNetworkMode() & NetworkTableInstance.kNetModeStarting) != 0) {
        Thread.sleep(100);
      }
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to start");
    }
  }

  public void testJNI() {
    // set up the poller
    int poller = NetworkTablesJNI.createConnectionListenerPoller(serverInst.getHandle());
    assertTrue("bad poller handle", poller != 0);
    int handle = NetworkTablesJNI.addPolledConnectionListener(poller, false);
    assertTrue("bad listener handle", handle != 0);

    // trigger a connect event
    connect();

    // get the event
    assertTrue(serverInst.waitForConnectionListenerQueue(1.0));
    ConnectionNotification[] events = null;
    try {
      events = NetworkTablesJNI.pollConnectionListenerTimeout(serverInst, poller, 0.0);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("unexpected interrupted exception" + ex);
    }

    assertNotNull(events);
    assertEquals(events.length, 1);
    assertEquals(handle, events[0].listener);
    assertTrue(events[0].connected);

    // trigger a disconnect event
    clientInst.stopClient();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to stop");
    }

    // get the event
    assertTrue(serverInst.waitForConnectionListenerQueue(1.0));
    try {
      events = NetworkTablesJNI.pollConnectionListenerTimeout(serverInst, poller, 0.0);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("unexpected interrupted exception" + ex);
    }

    assertNotNull(events);
    assertEquals(events.length, 1);
    assertEquals(handle, events[0].listener);
    assertFalse(events[0].connected);

  }

  public void testThreaded() {
    serverInst.startServer("connectionlistenertest.ini", "127.0.0.1", 10000);
    List<ConnectionNotification> events = new ArrayList<>();
    int handle = serverInst.addConnectionListener((event) -> events.add(event), false);

    // trigger a connect event
    clientInst.startClient("127.0.0.1", 10000);

    // wait for client to report it's started, then wait another 0.1 sec
    try {
      while ((clientInst.getNetworkMode() & NetworkTableInstance.kNetModeStarting) != 0) {
        Thread.sleep(100);
      }
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to start");
    }
    assertTrue(serverInst.waitForConnectionListenerQueue(1.0));

    // get the event
    assertEquals(events.size(), 1);
    assertEquals(handle, events.get(0).listener);
    assertTrue(events.get(0).connected);
    events.clear();

    // trigger a disconnect event
    clientInst.stopClient();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for client to stop");
    }

    // get the event
    assertTrue(serverInst.waitForConnectionListenerQueue(1.0));
    assertEquals(events.size(), 1);
    assertEquals(handle, events.get(0).listener);
    assertFalse(events.get(0).connected);
  }
}
