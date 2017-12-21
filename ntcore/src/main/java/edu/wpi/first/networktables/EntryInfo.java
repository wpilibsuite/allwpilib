/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * NetworkTables Entry information.
 */
public final class EntryInfo {
  /** Entry handle. */
  public final int entry;

  /** Entry name. */
  public final String name;

  /** Entry type. */
  public final NetworkTableType type;

  /** Entry flags. */
  public final int flags;

  /** Timestamp of last change to entry (type or value). */
  public final long last_change;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param inst Instance
   * @param entry Entry handle
   * @param name Name
   * @param type Type (integer version of {@link NetworkTableType})
   * @param flags Flags
   * @param last_change Timestamp of last change
   */
  public EntryInfo(NetworkTableInstance inst, int entry, String name, int type, int flags, long last_change) {
    this.inst = inst;
    this.entry = entry;
    this.name = name;
    this.type = NetworkTableType.getFromInt(type);
    this.flags = flags;
    this.last_change = last_change;
  }

  /* Network table instance. */
  private final NetworkTableInstance inst;

  /* Cached entry object. */
  private NetworkTableEntry entryObject;

  /**
   * Get the entry as an object.
   * @return NetworkTableEntry for this entry.
   */
  NetworkTableEntry getEntry() {
    if (entryObject == null) {
      entryObject = new NetworkTableEntry(inst, entry);
    }
    return entryObject;
  }
}
