/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/SmallVector.h>

#include "frc/RobotBase.h"

namespace frc {
namespace filesystem {

/**
 * Obtains the current working path that the program was launched with.
 * This is analogous to the `pwd` command on unix.
 *
 * @param result The result of the current working path lookup.
 */
void GetLaunchDirectory(wpi::SmallVectorImpl<char>& result);

/**
 * Obtains the operating directory of the program. On the roboRIO, this
 * is /home/lvuser. In simulation, it is where the simulation was launched
 * from (`pwd`).
 *
 * @param result The result of the operating directory lookup.
 */
void GetOperatingDirectory(wpi::SmallVectorImpl<char>& result);

/**
 * Obtains the deploy directory of the program, which is the remote location
 * src/main/deploy is deployed to by default. On the roboRIO, this is
 * /home/lvuser/deploy. In simulation, it is where the simulation was launched
 * from, in the subdirectory "deploy" (`pwd`/deploy).
 *
 * @param result The result of the operating directory lookup
 */
void GetDeployDirectory(wpi::SmallVectorImpl<char>& result);

}  // namespace filesystem
}  // namespace frc
