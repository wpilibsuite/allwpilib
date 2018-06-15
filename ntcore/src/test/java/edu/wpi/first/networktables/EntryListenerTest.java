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

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

class EntryListenerTest {
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

  @SuppressWarnings("PMD.AvoidUsingHardCodedIP")
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
  @Test
  void testPrefixNewRemote() {
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
    assertAll("Event",
        () -> assertEquals(1, events.size()),
        () -> assertEquals(handle, events.get(0).listener),
        () -> assertEquals(m_serverInst.getEntry("/foo/bar"), events.get(0).getEntry()),
        () -> assertEquals("/foo/bar", events.get(0).name),
        () -> assertEquals(NetworkTableValue.makeDouble(1.0), events.get(0).value),
        () -> assertEquals(EntryListenerFlags.kNew, events.get(0).flags)
    );
  }
}
