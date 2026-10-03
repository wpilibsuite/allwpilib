// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue;

import org.wpilib.framework.RobotBase;
import org.wpilib.framework.TimedRobot;

/**
 * Service for logging telemetry via Epilogue, starting from a root robot object. Service
 * implementations are automatically generated at compile-time: every class that extends {@code
 * RobotBase} gets its own service class. These are queried by {@link Epilogue#update(RobotBase)}
 * and {@link Epilogue#bind(TimedRobot)}.
 */
public interface EpilogueService<R extends RobotBase> {
  /**
   * Checks if this service supports the given root robot object.
   *
   * @param root The root robot object
   * @return True if this service specifically supports the given root robot object, false
   *     otherwise. This returns true if and only if the root robot is of the exact type {@code R};
   *     subclasses of {@code R} are not supported.
   */
  boolean supportsExactly(RobotBase root);

  /**
   * Updates telemetry for the given root robot object.
   *
   * @param root the root robot object
   */
  void update(R root);

  /**
   * A specialization of {@link EpilogueService} that can be bound to a {@link TimedRobot}.
   *
   * @param <R> The specific type of {@link TimedRobot} that this service supports.
   */
  interface Bindable<R extends TimedRobot> extends EpilogueService<R> {
    void bind(R root);
  }
}
