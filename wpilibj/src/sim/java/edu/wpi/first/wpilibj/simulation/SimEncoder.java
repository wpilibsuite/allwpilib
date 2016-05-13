/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.Publisher;
import org.gazebosim.transport.SubscriberCallback;

import gazebo.msgs.GzFloat64.Float64;
import gazebo.msgs.GzString;

public class SimEncoder {
  private double m_position;
  private double m_velocity;
  private Publisher<GzString.String> m_commandPub;

  @SuppressWarnings("JavadocMethod")
  public SimEncoder(String topic) {
    m_commandPub = MainNode.advertise(topic + "/control", Msgs.String());

    MainNode.subscribe(topic + "/position", Msgs.Float64(),
        new SubscriberCallback<Float64>() {
          @Override
          public void callback(Float64 msg) {
            m_position = msg.getData();
          }
        }
    );

    MainNode.subscribe(topic + "/velocity", Msgs.Float64(),
        new SubscriberCallback<Float64>() {
          @Override
          public void callback(Float64 msg) {
            m_velocity = msg.getData();
          }
        }
    );

    try {
      if (m_commandPub.waitForConnection(5000)) { // Wait up to five seconds.
        System.out.println("Initialized " + topic);
      } else {
        System.err.println("Failed to initialize " + topic + ": does the encoder exist?");
      }
    } catch (InterruptedException ex) {
      ex.printStackTrace(); // TODO: Better way to handle this?
      Thread.currentThread().interrupt();
    }
  }

  public void reset() {
    sendCommand("reset");
  }

  public void start() {
    sendCommand("start");
  }

  public void stop() {
    sendCommand("stop");
  }

  private void sendCommand(String cmd) {
    m_commandPub.publish(Msgs.String(cmd));
  }

  public double getM_position() {
    return m_position;
  }

  public double getM_velocity() {
    return m_velocity;
  }
}
