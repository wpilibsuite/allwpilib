// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * A listener that listens to changes in values in a {@link NetworkTable}.
 */
@FunctionalInterface
public interface TableEntryListener extends EntryListenerFlags {
  /**
   * Called when a key-value pair is changed in a {@link NetworkTable}.
   *
   * @param table the table the key-value pair exists in
   * @param key the key associated with the value that changed
   * @param entry the entry associated with the value that changed
   * @param value the new value
   * @param flags update flags; for example, EntryListenerFlags.kNew if the key
   *     did not previously exist in the table
   */
  void valueChanged(NetworkTable table, String key, NetworkTableEntry entry,
                    NetworkTableValue value, int flags);
}
