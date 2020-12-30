// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_ENTRYLISTENERFLAGS_H_
#define NTCORE_NETWORKTABLES_ENTRYLISTENERFLAGS_H_

#include "ntcore_c.h"

/** Entry listener flags */
namespace nt::EntryListenerFlags {

/**
 * Flag values for use with entry listeners.
 *
 * The flags are a bitmask and must be OR'ed together to indicate the
 * combination of events desired to be received.
 *
 * The constants kNew, kDelete, kUpdate, and kFlags represent different events
 * that can occur to entries.
 *
 * By default, notifications are only generated for remote changes occurring
 * after the listener is created.  The constants kImmediate and kLocal are
 * modifiers that cause notifications to be generated at other times.
 *
 * @ingroup ntcore_cpp_api
 */
enum {
  /**
   * Initial listener addition.
   * Set this flag to receive immediate notification of entries matching the
   * flag criteria (generally only useful when combined with kNew).
   */
  kImmediate = NT_NOTIFY_IMMEDIATE,

  /**
   * Changed locally.
   * Set this flag to receive notification of both local changes and changes
   * coming from remote nodes.  By default, notifications are only generated
   * for remote changes.  Must be combined with some combination of kNew,
   * kDelete, kUpdate, and kFlags to receive notifications of those respective
   * events.
   */
  kLocal = NT_NOTIFY_LOCAL,

  /**
   * Newly created entry.
   * Set this flag to receive a notification when an entry is created.
   */
  kNew = NT_NOTIFY_NEW,

  /**
   * Entry was deleted.
   * Set this flag to receive a notification when an entry is deleted.
   */
  kDelete = NT_NOTIFY_DELETE,

  /**
   * Entry's value changed.
   * Set this flag to receive a notification when an entry's value (or type)
   * changes.
   */
  kUpdate = NT_NOTIFY_UPDATE,

  /**
   * Entry's flags changed.
   * Set this flag to receive a notification when an entry's flags value
   * changes.
   */
  kFlags = NT_NOTIFY_FLAGS
};

}  // namespace nt::EntryListenerFlags

#endif  // NTCORE_NETWORKTABLES_ENTRYLISTENERFLAGS_H_
