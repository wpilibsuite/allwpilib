// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

/**
 * Interface for sendable serialization.
 *
 * <p>Idiomatically, classes that support sendable serialization should provide a static final
 * member named "sendable" that provides an instance of an implementation of this interface.
 *
 * @param <T> object type
 */
public interface Sendable<T> {
  /**
   * Gets the Class object for the stored value.
   *
   * @return Class
   */
  Class<T> getTypeClass();

  /**
   * Gets the dashboard type string.
   *
   * @return dashboard type string
   */
  String getTypeString();

  /**
   * Gets the sendable set for an object instance.
   *
   * @return sendable set
   */
  SendableSet getSendableSet(T obj);

  /**
   * Sets up a sendable for a particular object instance and table.
   *
   * @param obj object
   * @param table sendable table
   */
  void initSendable(T obj, SendableTable table);

  /**
   * Closes a sendable for a particular object instance.
   *
   * @param obj object
   */
  default void closeSendable(T obj) {
  }
}
