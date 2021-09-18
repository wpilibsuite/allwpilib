// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

/** WPILib FileSystem namespace */
namespace frc::filesystem {

/**
 * Obtains the current working path that the program was launched with.
 * This is analogous to the `pwd` command on unix.
 *
 * @return The result of the current working path lookup.
 */
std::string GetLaunchDirectory();

/**
 * Obtains the operating directory of the program. On the roboRIO, this
 * is /home/lvuser. In simulation, it is where the simulation was launched
 * from (`pwd`).
 *
 * @return The result of the operating directory lookup.
 */
std::string GetOperatingDirectory();

/**
 * Obtains the deploy directory of the program, which is the remote location
 * src/main/deploy is deployed to by default. On the roboRIO, this is
 * /home/lvuser/deploy. In simulation, it is where the simulation was launched
 * from, in the subdirectory "src/main/deploy" (`pwd`/src/main/deploy).
 *
 * @return The result of the operating directory lookup
 */
std::string GetDeployDirectory();

}  // namespace frc::filesystem
