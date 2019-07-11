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

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.fail;

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
  @SuppressWarnings("PMD.AvoidUsingHardCodedIP")
  void addMessageTest() {
    List<LogMessage> msgs = new ArrayList<>();
    m_clientInst.addLogger(msgs::add, LogMessage.kInfo, 100);

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

    assertFalse(msgs.isEmpty());
  }
}
