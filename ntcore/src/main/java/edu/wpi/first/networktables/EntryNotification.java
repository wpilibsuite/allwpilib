/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * NetworkTables Entry notification.
 */
public final class EntryNotification {
  /**
   * Listener that was triggered.
   */
  @SuppressWarnings("MemberName")
  public final int listener;

  /**
   * Entry handle.
   */
  @SuppressWarnings("MemberName")
  public final int entry;

  /**
   * Entry name.
   */
  @SuppressWarnings("MemberName")
  public final String name;

  /**
   * The new value.
   */
  @SuppressWarnings("MemberName")
  public final NetworkTableValue value;

  /**
   * Update flags.  For example, {@link EntryListenerFlags#kNew} if the key did
   * not previously exist.
   */
  @SuppressWarnings("MemberName")
  public final int flags;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param entry Entry handle
   * @param name Entry name
   * @param value The new value
   * @param flags Update flags
   */
  public EntryNotification(NetworkTableInstance inst, int listener, int entry, String name,
                           NetworkTableValue value, int flags) {
    this.m_inst = inst;
    this.listener = listener;
    this.entry = entry;
    this.name = name;
    this.value = value;
    this.flags = flags;
  }

  /* Network table instance. */
  private final NetworkTableInstance m_inst;

  /* Cached entry object. */
  NetworkTableEntry m_entryObject;

  /**
   * Get the entry as an object.
   *
   * @return NetworkTableEntry for this entry.
   */
  public NetworkTableEntry getEntry() {
    if (m_entryObject == null) {
      m_entryObject = new NetworkTableEntry(m_inst, entry);
    }
    return m_entryObject;
  }
}
