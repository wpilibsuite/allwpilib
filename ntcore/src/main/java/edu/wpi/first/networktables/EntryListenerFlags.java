/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * Flag values for use with entry listeners.
 *
 * <p>The flags are a bitmask and must be OR'ed together to indicate the
 * combination of events desired to be received.
 *
 * <p>The constants kNew, kDelete, kUpdate, and kFlags represent different events
 * that can occur to entries.
 *
 * <p>By default, notifications are only generated for remote changes occurring
 * after the listener is created.  The constants kImmediate and kLocal are
 * modifiers that cause notifications to be generated at other times.
 */
public interface EntryListenerFlags {
  /**
   * Initial listener addition.
   *
   * <p>Set this flag to receive immediate notification of entries matching the
   * flag criteria (generally only useful when combined with kNew).
   */
  int kImmediate = 0x01;

  /**
   * Changed locally.
   *
   * <p>Set this flag to receive notification of both local changes and changes
   * coming from remote nodes.  By default, notifications are only generated
   * for remote changes.  Must be combined with some combination of kNew,
   * kDelete, kUpdate, and kFlags to receive notifications of those respective
   * events.
   */
  int kLocal = 0x02;

  /**
   * Newly created entry.
   *
   * <p>Set this flag to receive a notification when an entry is created.
   */
  int kNew = 0x04;

  /**
   * Entry was deleted.
   *
   * <p>Set this flag to receive a notification when an entry is deleted.
   */
  int kDelete = 0x08;

  /**
   * Entry's value changed.
   *
   * <p>Set this flag to receive a notification when an entry's value (or type)
   * changes.
   */
  int kUpdate = 0x10;

  /**
   * Entry's flags changed.
   *
   * <p>Set this flag to receive a notification when an entry's flags value
   * changes.
   */
  int kFlags = 0x20;
}
