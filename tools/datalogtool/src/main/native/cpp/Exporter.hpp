// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

namespace wpi::glass {
class Storage;
}  // namespace wpi::glass

/**
 * Adds a datalog to the input files, unless its filename stem is already
 * loaded. File errors are displayed in the input files list.
 *
 * @param filename Datalog filename.
 */
void AddInputFile(std::string_view filename);

void DisplayInputFiles();
void DisplayEntries();
void DisplayOutput(wpi::glass::Storage& storage);

extern bool gShutdown;
