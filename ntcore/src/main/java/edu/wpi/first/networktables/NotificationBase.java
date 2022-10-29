// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** Base class for NetworkTables notifications. */
@SuppressWarnings("MemberName")
public class NotificationBase {
  /**
   * Handle of listener that was triggered. The value returned when adding the listener can be used
   * to map this to a specific added listener.
   */
  public final int listener;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param listener Listener handle
   */
  public NotificationBase(NetworkTableInstance inst, int listener) {
    this.m_inst = inst;
    this.listener = listener;
  }

  /* Network table instance. */
  protected final NetworkTableInstance m_inst;

  /**
   * Gets the instance associated with this notification.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }
}
