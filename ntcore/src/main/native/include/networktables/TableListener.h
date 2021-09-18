// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_TABLELISTENER_H_
#define NTCORE_NETWORKTABLES_TABLELISTENER_H_

#include <functional>
#include <memory>
#include <string_view>

namespace nt {

class NetworkTable;

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
using TableListener =
    std::function<void(NetworkTable* parent, std::string_view name,
                       std::shared_ptr<NetworkTable> table)>;

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_TABLELISTENER_H_
