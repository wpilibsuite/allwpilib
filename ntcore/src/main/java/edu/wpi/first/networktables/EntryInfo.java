// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * NetworkTables Entry information.
 */
public final class EntryInfo {
  /** Entry handle. */
  @SuppressWarnings("MemberName")
  public final int entry;

  /** Entry name. */
  @SuppressWarnings("MemberName")
  public final String name;

  /** Entry type. */
  @SuppressWarnings("MemberName")
  public final NetworkTableType type;

  /** Entry flags. */
  @SuppressWarnings("MemberName")
  public final int flags;

  /** Timestamp of last change to entry (type or value). */
  @SuppressWarnings("MemberName")
  public final long last_change;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param entry Entry handle
   * @param name Name
   * @param type Type (integer version of {@link NetworkTableType})
   * @param flags Flags
   * @param lastChange Timestamp of last change
   */
  public EntryInfo(NetworkTableInstance inst, int entry, String name, int type, int flags,
                   long lastChange) {
    this.m_inst = inst;
    this.entry = entry;
    this.name = name;
    this.type = NetworkTableType.getFromInt(type);
    this.flags = flags;
    this.last_change = lastChange;
  }

  /* Network table instance. */
  private final NetworkTableInstance m_inst;

  /* Cached entry object. */
  private NetworkTableEntry m_entryObject;

  /**
   * Get the entry as an object.
   *
   * @return NetworkTableEntry for this entry.
   */
  NetworkTableEntry getEntry() {
    if (m_entryObject == null) {
      m_entryObject = new NetworkTableEntry(m_inst, entry);
    }
    return m_entryObject;
  }
}
