/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import com.google.protobuf.Message;

import org.gazebosim.transport.Node;
import org.gazebosim.transport.Publisher;
import org.gazebosim.transport.Subscriber;
import org.gazebosim.transport.SubscriberCallback;

import java.io.IOException;
import java.util.logging.Logger;

public class MainNode {

  private MainNode() {
  }

  private static final Logger LOG = Logger.getLogger("Simulation MainNode");
  private static Node m_mainNode;

  @SuppressWarnings("JavadocMethod")
  public static synchronized void openGazeboConnection() throws IOException, InterruptedException {
    if (m_mainNode != null) {
      LOG.warning("MainNode.openGazeboConnection() was already called!");
      return;
    }
    m_mainNode = new Node("frc");
    m_mainNode.waitForConnection();
  }

  @SuppressWarnings("JavadocMethod")
  public static <T extends Message> Publisher<T> advertise(String topic, T defaultMessage) {
    if (m_mainNode == null) {
      throw new IllegalStateException("MainNode.openGazeboConnection() should have already been "
          + "called by RobotBase.main()!");
    }
    return m_mainNode.advertise(topic, defaultMessage);
  }

  @SuppressWarnings("JavadocMethod")
  public static <T extends Message> Subscriber<T> subscribe(String topic, T defaultMessage,
                                                            SubscriberCallback<T> cb) {
    if (m_mainNode == null) {
      throw new IllegalStateException("MainNode.openGazeboConnection() should have already been "
          + "called by RobotBase.main()!");
    }
    return m_mainNode.subscribe(topic, defaultMessage, cb);
  }
}
