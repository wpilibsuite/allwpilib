// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import org.wpilib.networktables.DoublePublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.system.SystemServer;

/** This class contains feedback and feedforward constants for an ExpansionHub motor. */
public class ExpansionHubVelocityConstants {
  private final DoublePublisher m_pPublisher;
  private final DoublePublisher m_iPublisher;
  private final DoublePublisher m_dPublisher;

  private final DoublePublisher m_sPublisher;
  private final DoublePublisher m_vPublisher;
  private final DoublePublisher m_aPublisher;

  ExpansionHubVelocityConstants(int hubNumber, int motorNumber) {
    NetworkTableInstance systemServer = SystemServer.getSystemServer();

    PubSubOption[] options =
        new PubSubOption[] {
          PubSubOption.SEND_ALL, PubSubOption.KEEP_DUPLICATES, PubSubOption.periodic(0.005)
        };

    m_pPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity" + "/kp")
            .publish(options);

    m_iPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity" + "/ki")
            .publish(options);

    m_dPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity" + "/kd")
            .publish(options);

    m_sPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity" + "/ks")
            .publish(options);

    m_vPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity" + "/kv")
            .publish(options);

    m_aPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity" + "/ka")
            .publish(options);
  }

  /**
   * Sets the PID Controller gain parameters.
   *
   * <p>Set the proportional, integral, and differential coefficients.
   *
   * @param p The proportional coefficient.
   * @param i The integral coefficient.
   * @param d The derivative coefficient.
   * @return This object, for method chaining.
   */
  public ExpansionHubVelocityConstants setPID(double p, double i, double d) {
    m_pPublisher.set(p);
    m_iPublisher.set(i);
    m_dPublisher.set(d);
    return this;
  }

  /**
   * Sets the feed forward gains to the specified values.
   *
   * <p>The units should be radians for angular systems and meters for linear systems.
   *
   * <p>The motor control period is 10ms
   *
   * @param s The static gain in volts.
   * @param v The velocity gain in volts per unit per second.
   * @param a The acceleration gain in volts per unit per second squared.
   * @return This object, for method chaining.
   */
  public ExpansionHubVelocityConstants setFF(double s, double v, double a) {
    m_sPublisher.set(s);
    m_vPublisher.set(v);
    m_aPublisher.set(a);
    return this;
  }

  void close() {
    m_pPublisher.close();
    m_iPublisher.close();
    m_dPublisher.close();
    m_sPublisher.close();
    m_vPublisher.close();
    m_aPublisher.close();
  }
}
