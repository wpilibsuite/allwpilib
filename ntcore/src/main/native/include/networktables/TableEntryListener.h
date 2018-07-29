/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_NETWORKTABLES_TABLEENTRYLISTENER_H_
#define NTCORE_NETWORKTABLES_TABLEENTRYLISTENER_H_

#include <functional>
#include <memory>

#include <wpi/StringRef.h>

namespace nt {

class NetworkTable;
class NetworkTableEntry;
class Value;

using wpi::StringRef;

/**
 * A listener that listens to changes in values in a NetworkTable.
 *
 * Called when a key-value pair is changed in a NetworkTable.
 *
 * @param table the table the key-value pair exists in
 * @param key the key associated with the value that changed
 * @param entry the entry associated with the value that changed
 * @param value the new value
 * @param flags update flags; for example, EntryListenerFlags.kNew if the key
 * did not previously exist
 *
 * @ingroup ntcore_cpp_api
 */
typedef std::function<void(NetworkTable* table, StringRef name,
                           NetworkTableEntry entry,
                           std::shared_ptr<Value> value, int flags)>
    TableEntryListener;

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_TABLEENTRYLISTENER_H_
