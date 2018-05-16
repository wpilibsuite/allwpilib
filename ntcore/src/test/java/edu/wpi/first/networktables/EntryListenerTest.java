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

public class EntryListenerTest extends TestCase {
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

  private void connect() {
    m_serverInst.startServer("connectionlistenertest.ini", "127.0.0.1", 10000);
    m_clientInst.startClient("127.0.0.1", 10000);

    // Use connection listener to ensure we've connected
    int poller = NetworkTablesJNI.createConnectionListenerPoller(m_clientInst.getHandle());
    NetworkTablesJNI.addPolledConnectionListener(poller, false);
    try {
      if (NetworkTablesJNI.pollConnectionListenerTimeout(m_clientInst, poller, 1.0).length == 0) {
        fail("client didn't connect to server");
      }
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("interrupted while waiting for server connection");
    }
  }

  /**
   * Test prefix with a new remote.
   */
  public void testPrefixNewRemote() {
    connect();
    List<EntryNotification> events = new ArrayList<>();
    final int handle = m_serverInst.addEntryListener("/foo", events::add,
        EntryListenerFlags.kNew);

    // Trigger an event
    m_clientInst.getEntry("/foo/bar").setDouble(1.0);
    m_clientInst.getEntry("/baz").setDouble(1.0);
    m_clientInst.flush();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for entries to update");
    }

    assertTrue(m_serverInst.waitForEntryListenerQueue(1.0));

    // Check the event
    assertEquals(events.size(), 1);
    assertEquals(events.get(0).listener, handle);
    assertEquals(events.get(0).getEntry(), m_serverInst.getEntry("/foo/bar"));
    assertEquals(events.get(0).name, "/foo/bar");
    assertEquals(events.get(0).value, NetworkTableValue.makeDouble(1.0));
    assertEquals(events.get(0).flags, EntryListenerFlags.kNew);
  }
}
