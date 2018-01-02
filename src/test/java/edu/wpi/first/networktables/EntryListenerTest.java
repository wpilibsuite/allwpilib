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

public class EntryListenerTest extends TestCase {
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

    // Use connection listener to ensure we've connected
    int poller = NetworkTablesJNI.createConnectionListenerPoller(clientInst.getHandle());
    NetworkTablesJNI.addPolledConnectionListener(poller, false);
    try {
      if (NetworkTablesJNI.pollConnectionListenerTimeout(clientInst, poller, 1.0).length == 0) {
        fail("client didn't connect to server");
      }
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("interrupted while waiting for server connection");
    }
  }

  public void testPrefixNewRemote() {
    connect();
    List<EntryNotification> events = new ArrayList<>();
    int handle = serverInst.addEntryListener("/foo", (event) -> events.add(event),
        EntryListenerFlags.kNew);

    // Trigger an event
    clientInst.getEntry("/foo/bar").setDouble(1.0);
    clientInst.getEntry("/baz").setDouble(1.0);
    clientInst.flush();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for entries to update");
    }

    assertTrue(serverInst.waitForEntryListenerQueue(1.0));

    // Check the event
    assertEquals(events.size(), 1);
    assertEquals(events.get(0).listener, handle);
    assertEquals(events.get(0).getEntry(), serverInst.getEntry("/foo/bar"));
    assertEquals(events.get(0).name, "/foo/bar");
    assertEquals(events.get(0).value, NetworkTableValue.makeDouble(1.0));
    assertEquals(events.get(0).flags, EntryListenerFlags.kNew);
  }
}
