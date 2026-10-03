// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>

namespace wpi::util {
class raw_ostream;
}  // namespace wpi::util

namespace dlt {

/**
 * Runs the datalogtool command-line interface.
 *
 * @param args Arguments excluding the executable name.
 * @param out Standard output, for help and machine-readable listings.
 * @param err Standard error, for progress and diagnostics.
 * @return 0 on success, 1 on an operation failure, or 2 on invalid arguments.
 */
int RunCli(std::span<const std::string> args, wpi::util::raw_ostream& out,
           wpi::util::raw_ostream& err);

}  // namespace dlt
