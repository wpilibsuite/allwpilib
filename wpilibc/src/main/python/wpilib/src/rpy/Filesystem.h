
#pragma once

#include "wpi/util/fs.hpp"

namespace robotpy::filesystem {

/**
 * Obtains the operating directory of the program. On the robot, this
 * is /home/systemcore/py. In simulation, it is the location of robot.py
 *
 * @return The result of the operating directory lookup.
 */
std::string GetOperatingDirectory();

/**
 * Obtains the deploy directory of the program, which is the remote location
 * the deploy directory is deployed to by default. On the robot, this is
 * /home/systemcore/py/deploy. In simulation, it is where the simulation was
 * launched from, in the subdirectory "deploy" (`dirname(robot.py)`/deploy).
 *
 * @return The result of the operating directory lookup
 */
std::string GetDeployDirectory();

// intended to be used by C++ bindings, returns same as GetOperatingDirectory
fs::path GetOperatingDirectoryFs();
// intended to be used by C++ bindings, returns same as GetDeployDirectory
fs::path GetDeployDirectoryFs();

}  // namespace robotpy::filesystem

#include "Filesystem.inc"
