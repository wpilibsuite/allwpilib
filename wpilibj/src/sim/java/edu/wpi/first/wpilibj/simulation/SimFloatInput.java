/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;


import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.SubscriberCallback;

import gazebo.msgs.GzFloat64.Float64;

public class SimFloatInput {
  private double m_value;

  @SuppressWarnings("JavadocMethod")
  public SimFloatInput(String topic) {
    MainNode.subscribe(topic, Msgs.Float64(),
        new SubscriberCallback<Float64>() {
          @Override
          public void callback(Float64 msg) {
            m_value = msg.getData();
          }
        }
    );
  }

  public double get() {
    return m_value;
  }
}
