// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.util.WPIUtilJNI;
import java.util.EnumSet;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

class TopicListenerTest {
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

  private void connect() {
    m_serverInst.startServer("topiclistenertest.json", "127.0.0.1", 0, 10010);
    m_clientInst.startClient4("client");
    m_clientInst.setServer("127.0.0.1", 10010);

    // Use connection listener to ensure we've connected
    int poller = NetworkTablesJNI.createListenerPoller(m_clientInst.getHandle());
    NetworkTablesJNI.addListener(
        poller, m_clientInst.getHandle(), EnumSet.of(NetworkTableEvent.Kind.kConnected));
    try {
      if (WPIUtilJNI.waitForObjectTimeout(poller, 1.0)) {
        fail("client didn't connect to server");
      }
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("interrupted while waiting for server connection");
    }
  }

  /** Test prefix with a new remote. */
  @Disabled("unreliable in CI")
  @Test
  void testPrefixNewRemote() {
    connect();
    final int poller = NetworkTablesJNI.createListenerPoller(m_serverInst.getHandle());
    final int handle =
        NetworkTablesJNI.addListener(
            poller, new String[] {"/foo"}, EnumSet.of(NetworkTableEvent.Kind.kPublish));

    // Trigger an event
    m_clientInst.getEntry("/foo/bar").setDouble(1.0);
    m_clientInst.getEntry("/baz").setDouble(1.0);
    m_clientInst.flush();
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      fail("interrupted while waiting for entries to update");
    }

    try {
      if (WPIUtilJNI.waitForObjectTimeout(poller, 1.0)) {
        fail("never got signaled");
      }
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      fail("interrupted while waiting for signal");
    }
    NetworkTableEvent[] events = NetworkTablesJNI.readListenerQueue(m_serverInst, poller);

    // Check the event
    assertEquals(1, events.length);
    assertEquals(handle, events[0].listener);
    assertNotNull(events[0].topicInfo);
    assertEquals(m_serverInst.getTopic("/foo/bar"), events[0].topicInfo.getTopic());
    assertEquals("/foo/bar", events[0].topicInfo.name);
    assertTrue(events[0].is(NetworkTableEvent.Kind.kPublish));
  }
}
