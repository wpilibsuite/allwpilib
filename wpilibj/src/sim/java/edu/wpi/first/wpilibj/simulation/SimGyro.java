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

public class SimGyro {
  private double m_position;
  private double m_velocity;
  private Publisher<GzString.String> m_commandPub;

  @SuppressWarnings("JavadocMethod")
  public SimGyro(String topic) {
    m_commandPub = MainNode.advertise(topic + "/control", Msgs.String());
    m_commandPub.setLatchMode(true);

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
  }

  public void reset() {
    sendCommand("reset");
  }

  private void sendCommand(String cmd) {
    m_commandPub.publish(Msgs.String(cmd));
  }

  public double getAngle() {
    return m_position;
  }

  public double getVelocity() {
    return m_velocity;
  }
}
