// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.util.WPIUtilJNI;
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
    m_serverInst.setNetworkIdentity("server");

    m_clientInst = NetworkTableInstance.create();
    m_clientInst.setNetworkIdentity("client");
  }

  @AfterEach
  void tearDown() {
    m_clientInst.close();
    m_serverInst.close();
  }

  private void connect() {
    m_serverInst.startServer("topiclistenertest.json", "127.0.0.1", 0, 10010);
    m_clientInst.startClient4();
    m_clientInst.setServer("127.0.0.1", 10010);

    // Use connection listener to ensure we've connected
    int poller = NetworkTablesJNI.createConnectionListenerPoller(m_clientInst.getHandle());
    NetworkTablesJNI.addPolledConnectionListener(poller, false);
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
    final int poller = NetworkTablesJNI.createTopicListenerPoller(m_serverInst.getHandle());
    final int handle =
        NetworkTablesJNI.addPolledTopicListener(
            poller, new String[] {"/foo"}, TopicListenerFlags.kPublish);

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
    TopicNotification[] events = NetworkTablesJNI.readTopicListenerQueue(m_serverInst, poller);

    // Check the event
    assertEquals(1, events.length);
    assertEquals(handle, events[0].listener);
    assertEquals(m_serverInst.getTopic("/foo/bar"), events[0].info.getTopic());
    assertEquals("/foo/bar", events[0].info.name);
    assertEquals(TopicListenerFlags.kPublish, events[0].flags);
  }
}
