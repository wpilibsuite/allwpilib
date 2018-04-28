/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
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
  public final int listener;

  /**
   * Entry handle.
   */
  public final int entry;

  /**
   * Entry name.
   */
  public final String name;

  /**
   * The new value.
   */
  public final NetworkTableValue value;

  /**
   * Update flags.  For example, {@link EntryListenerFlags#kNew} if the key did
   * not previously exist.
   */
  public final int flags;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param entry Entry handle
   * @param name Entry name
   * @param value The new value
   * @param flags Update flags
   */
  public EntryNotification(NetworkTableInstance inst, int listener, int entry, String name, NetworkTableValue value, int flags) {
    this.inst = inst;
    this.listener = listener;
    this.entry = entry;
    this.name = name;
    this.value = value;
    this.flags = flags;
  }

  /* Network table instance. */
  private final NetworkTableInstance inst;

  /* Cached entry object. */
  NetworkTableEntry entryObject;

  /**
   * Get the entry as an object.
   * @return NetworkTableEntry for this entry.
   */
  public NetworkTableEntry getEntry() {
    if (entryObject == null) {
      entryObject = new NetworkTableEntry(inst, entry);
    }
    return entryObject;
  }
}
