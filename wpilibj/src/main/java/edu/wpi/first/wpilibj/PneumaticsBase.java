// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

public interface PneumaticsBase extends AutoCloseable {
  /**
   * Sets solenoids on a pneumatics module.
   *
   * @param mask mask
   * @param values values
   */
  void setSolenoids(int mask, int values);

  /**
   * Gets solenoid values.
   *
   * @return values
   */
  int getSolenoids();

  /**
   * Get module number for this module.
   *
   * @return module number
   */
  int getModuleNumber();

  /**
   * Get the disabled solenoids.
   *
   * @return disabled list
   */
  int getSolenoidDisabledList();

  /**
   * Fire a single solenoid shot.
   *
   * @param index solenoid index
   */
  void fireOneShot(int index);

  /**
   * Set the duration for a single solenoid shot.
   *
   * @param index solenoid index
   * @param durMs shot duration
   */
  void setOneShotDuration(int index, int durMs);

  /**
   * Check if a solenoid channel is valid.
   *
   * @param channel Channel to check
   * @return True if channel exists
   */
  boolean checkSolenoidChannel(int channel);

  /**
   * Check to see if the masked solenoids can be reserved, and if not reserve them.
   *
   * @param mask The solenoid mask to reserve
   * @return 0 if successful, mask of solenoids that couldn't be allocated otherwise
   */
  int checkAndReserveSolenoids(int mask);

  /**
   * Unreserve the masked solenoids.
   *
   * @param mask The solenoid mask to unreserve
   */
  void unreserveSolenoids(int mask);

  /**
   * Duplicate a Pneumatics base module. Used for lifetime management. Not usually needed by teams.
   *
   * @return A copy of the PneumaticsBase object
   */
  PneumaticsBase duplicate();

  @Override
  void close();
}
