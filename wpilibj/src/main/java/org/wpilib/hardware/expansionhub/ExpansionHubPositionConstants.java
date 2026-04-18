// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.DoublePublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.system.SystemServer;

/** This class contains feedback and feedforward constants for an ExpansionHub motor. */
public class ExpansionHubPositionConstants {
  private final DoublePublisher m_pPublisher;
  private final DoublePublisher m_iPublisher;
  private final DoublePublisher m_dPublisher;

  private final DoublePublisher m_sPublisher;

  private final BooleanPublisher m_continuousPublisher;
  private final DoublePublisher m_continuousMinimumPublisher;
  private final DoublePublisher m_continuousMaximumPublisher;

  ExpansionHubPositionConstants(int hubNumber, int motorNumber) {
    NetworkTableInstance systemServer = SystemServer.getSystemServer();

    PubSubOption[] options =
        new PubSubOption[] {
          PubSubOption.SEND_ALL, PubSubOption.KEEP_DUPLICATES, PubSubOption.periodic(0.005)
        };

    m_pPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/position" + "/kp")
            .publish(options);

    m_iPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/position" + "/ki")
            .publish(options);

    m_dPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/position" + "/kd")
            .publish(options);

    m_sPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/position" + "/ks")
            .publish(options);

    m_continuousPublisher =
        systemServer
            .getBooleanTopic(
                "/rhsp/"
                    + hubNumber
                    + "/motor"
                    + motorNumber
                    + "/constants/position"
                    + "/continuous")
            .publish(options);

    m_continuousMinimumPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/"
                    + hubNumber
                    + "/motor"
                    + motorNumber
                    + "/constants/position"
                    + "/continuousMinimum")
            .publish(options);

    m_continuousMaximumPublisher =
        systemServer
            .getDoubleTopic(
                "/rhsp/"
                    + hubNumber
                    + "/motor"
                    + motorNumber
                    + "/constants/position"
                    + "/continuousMaximum")
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
  public ExpansionHubPositionConstants setPID(double p, double i, double d) {
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
   * @return This object, for method chaining.
   */
  public ExpansionHubPositionConstants setS(double s) {
    m_sPublisher.set(s);
    return this;
  }

  /**
   * Enables continuous input.
   *
   * <p>Rather then using the max and min input range as constraints, it considers them to be the
   * same point and automatically calculates the shortest route to the setpoint.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   * @return This object, for method chaining.
   */
  public ExpansionHubPositionConstants enableContinuousInput(
      double minimumInput, double maximumInput) {
    m_continuousMaximumPublisher.set(maximumInput);
    m_continuousMinimumPublisher.set(minimumInput);
    m_continuousPublisher.set(true);
    return this;
  }

  /**
   * Disable continuous input mode.
   *
   * @return This object, for method chaining.
   */
  public ExpansionHubPositionConstants disableContinuousInput() {
    m_continuousPublisher.set(false);
    return this;
  }

  void close() {
    m_pPublisher.close();
    m_iPublisher.close();
    m_dPublisher.close();
    m_sPublisher.close();
    m_continuousPublisher.close();
    m_continuousMinimumPublisher.close();
    m_continuousMaximumPublisher.close();
  }
}
