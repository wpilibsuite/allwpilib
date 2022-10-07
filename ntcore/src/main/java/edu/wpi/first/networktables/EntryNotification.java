// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables Entry notification. */
@SuppressWarnings("MemberName")
public final class EntryNotification {
  /** Listener that was triggered. */
  public final int listener;

  /** Entry handle. */
  public final int entry;

  /** Entry name. */
  public final String name;

  /** The new value. */
  public final NetworkTableValue value;

  /**
   * Update flags. For example, {@link EntryListenerFlags#kNew} if the key did not previously exist.
   */
  public final int flags;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param entry Entry handle
   * @param name Entry name
   * @param value The new value
   * @param flags Update flags
   */
  public EntryNotification(
      NetworkTableInstance inst,
      int listener,
      int entry,
      String name,
      NetworkTableValue value,
      int flags) {
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
