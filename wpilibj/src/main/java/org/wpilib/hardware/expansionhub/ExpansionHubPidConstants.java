// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.DoublePublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.system.SystemServer;

/** This class contains PID constants for an ExpansionHub motor. */
public class ExpansionHubPidConstants {
  private final DoublePublisher m_pPublisher;
  private final DoublePublisher m_iPublisher;
  private final DoublePublisher m_dPublisher;
  private final DoublePublisher m_sPublisher;
  private final DoublePublisher m_vPublisher;
  private final DoublePublisher m_aPublisher;

  private final BooleanPublisher m_continuousPublisher;
  private final DoublePublisher m_continuousMinimumPublisher;
  private final DoublePublisher m_continuousMaximumPublisher;

  ExpansionHubPidConstants(int hubNumber, int motorNumber, boolean isVelocityPid) {
    NetworkTableInstance systemServer = SystemServer.getSystemServer();

    PubSubOption[] options =
        new PubSubOption[] {
          PubSubOption.sendAll(true),
          PubSubOption.keepDuplicates(true),
          PubSubOption.periodic(0.005)
        };

    String pidType = isVelocityPid ? "velocity" : "position";

    m_pPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/kp")
            .publish(options);

    m_iPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/ki")
            .publish(options);

    m_dPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/kd")
            .publish(options);

    m_aPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/ka")
            .publish(options);

    m_vPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/kv")
            .publish(options);

    m_sPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/ks")
            .publish(options);

    m_continuousPublisher =
        systemServer
            .getBooleanTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/pid/" + pidType + "/continuous")
            .publish(options);

    m_continuousMinimumPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/"
                    + hubNumber
                    + "/motor"
                    + motorNumber
                    + "/pid/"
                    + pidType
                    + "/continuousMinimum")
            .publish(options);

    m_continuousMaximumPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/"
                    + hubNumber
                    + "/motor"
                    + motorNumber
                    + "/pid/"
                    + pidType
                    + "/continousMaximum")
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
   */
  public void setPID(double p, double i, double d) {
    m_pPublisher.set(p);
    m_iPublisher.set(i);
    m_dPublisher.set(d);
  }

  /**
   * Sets the feed forward gains to the specified values.
   *
   * <p>The units should be radians for angular systems and meters for linear systems.
   *
   * <p>The PID control period is 10ms
   *
   * @param s The static gain in volts.
   * @param v The velocity gain in V/(units/s).
   * @param a The acceleration gain in V/(units/s²).
   */
  public void setFF(double s, double v, double a) {
    m_sPublisher.set(s);
    m_vPublisher.set(v);
    m_aPublisher.set(a);
  }

  /**
   * Enables continuous input.
   *
   * <p>Rather then using the max and min input range as constraints, it considers them to be the
   * same point and automatically calculates the shortest route to the setpoint.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  public void enableContinuousInput(double minimumInput, double maximumInput) {
    m_continuousMaximumPublisher.set(maximumInput);
    m_continuousMinimumPublisher.set(minimumInput);
    m_continuousPublisher.set(true);
  }

  /** Disable continous input mode. */
  public void disableContinuousInput() {
    m_continuousPublisher.set(false);
  }

  void close() {
    m_iPublisher.close();
    m_dPublisher.close();
    m_sPublisher.close();
    m_vPublisher.close();
    m_aPublisher.close();
    m_continuousPublisher.close();
    m_continuousMinimumPublisher.close();
    m_continuousMaximumPublisher.close();
  }
}
