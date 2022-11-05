// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class LoggerTest {
  private NetworkTableInstance m_clientInst;

  @BeforeEach
  protected void setUp() {
    m_clientInst = NetworkTableInstance.create();
  }

  @AfterEach
  protected void tearDown() {
    m_clientInst.close();
  }

  @Test
  void addMessageTest() {
    List<NetworkTableEvent> msgs = new ArrayList<>();
    m_clientInst.addLogger(LogMessage.kInfo, 100, msgs::add);

    m_clientInst.startClient4("client");
    m_clientInst.setServer("127.0.0.1", 10000);

    // wait for client to report it's started, then wait another 0.1 sec
    try {
      int count = 0;
      while (!m_clientInst.getNetworkMode().contains(NetworkTableInstance.NetworkMode.kClient4)) {
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

    assertFalse(msgs.isEmpty());
  }
}
