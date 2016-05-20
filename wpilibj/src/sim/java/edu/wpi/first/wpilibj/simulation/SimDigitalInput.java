/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.SubscriberCallback;

import gazebo.msgs.GzBool.Bool;

public class SimDigitalInput {
  private boolean m_value;

  @SuppressWarnings("JavadocMethod")
  public SimDigitalInput(String topic) {
    MainNode.subscribe(topic, Msgs.Bool(),
        new SubscriberCallback<Bool>() {
          @Override
          public void callback(Bool msg) {
            m_value = msg.getData();
          }
        }
    );
  }

  public boolean get() {
    return m_value;
  }
}
