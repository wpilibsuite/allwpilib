// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables entry base implementation. */
public abstract class EntryBase implements Subscriber, Publisher {
  /**
   * Constructor.
   *
   * @param handle handle
   */
  public EntryBase(int handle) {
    m_handle = handle;
  }

  @Override
  public boolean isValid() {
    return m_handle != 0;
  }

  @Override
  public int getHandle() {
    return m_handle;
  }

  @Override
  public void close() {
    NetworkTablesJNI.release(m_handle);
  }

  @Override
  public boolean exists() {
    return NetworkTablesJNI.getTopicExists(m_handle);
  }

  @Override
  public long getLastChange() {
    return NetworkTablesJNI.getEntryLastChange(m_handle);
  }

  /** NetworkTables handle. */
  protected int m_handle;
}
