/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_NETWORKTABLES_TABLELISTENER_H_
#define NTCORE_NETWORKTABLES_TABLELISTENER_H_

#include <functional>
#include <memory>

#include <wpi/StringRef.h>

namespace nt {

class NetworkTable;

using wpi::StringRef;

/**
 * A listener that listens to new sub-tables in a NetworkTable.
 *
 * Called when a new table is created.
 *
 * @param parent the parent of the table
 * @param name the name of the new table
 * @param table the new table
 *
 * @ingroup ntcore_cpp_api
 */
typedef std::function<void(NetworkTable* parent, StringRef name,
                           std::shared_ptr<NetworkTable> table)>
    TableListener;

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_TABLELISTENER_H_
