/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Allows multiple {@link SpeedController} objects to be linked together.
 */
public class SpeedControllerGroup<T extends SpeedController> implements SpeedController {

  private boolean m_isInverted = false;
  private final List<T> m_speedControllers;

  /**
   * Create a new SpeedControllerGroup with no SpeedControllers.
   */
  public SpeedControllerGroup() {
    m_speedControllers = new ArrayList<>();
  }

  /**
   * Create a new SpeedControllerGroup with the provided SpeedControllers.
   *
   * @param speedControllers The SpeedControllers to add
   */
  public SpeedControllerGroup(Collection<T> speedControllers) {
    m_speedControllers = new ArrayList<>(speedControllers);
  }

  /**
   * Adds the provided SpeedController to the SpeedControllerGroup.
   *
   * @param speedController The SpeedController to add
   */
  public void add(T speedController) {
    m_speedControllers.add(speedController);
  }

  /**
   * Adds the provided SpeedControllers to the SpeedControllerGroup.
   *
   * @param speedControllers The SpeedControllers to add
   */
  public void addAll(Collection<T> speedControllers) {
    m_speedControllers.addAll(speedControllers);
  }

  /**
   * Removed the provided SpeedController to the SpeedControllerGroup.
   *
   * @param speedController The SpeedController to remove
   */
  public void remove(T speedController) {
    m_speedControllers.remove(speedController);
  }

  /**
   * Gets a list of SpeedControllers in this SpeedControllerGroup.
   *
   * @return A list of SpeedControllers in this SpeedControllerGroup
   */
  public List<T> getSpeedControllers() {
    return m_speedControllers;
  }

  @Override
  public double get() {
    if (!m_speedControllers.isEmpty()) {
      return m_speedControllers.get(0).get();
    }
    return 0.0;
  }

  @Override
  public void set(double speed) {
    m_speedControllers.forEach(s -> s.set(m_isInverted ? -speed : speed));
  }

  @Override
  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  @Override
  public boolean getInverted() {
    return m_isInverted;
  }

  @Override
  public void disable() {
    m_speedControllers.forEach(SpeedController::disable);
  }

  @Override
  public void stopMotor() {
    m_speedControllers.forEach(SpeedController::stopMotor);
  }

  @Override
  public void pidWrite(double output) {
    m_speedControllers.forEach(s -> s.pidWrite(output));
  }
}
