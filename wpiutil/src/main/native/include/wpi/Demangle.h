// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_DEMANGLE_H_
#define WPIUTIL_WPI_DEMANGLE_H_

#include <string>

#include "wpi/Twine.h"

namespace wpi {

/**
 * Demangle a C++ symbol.
 *
 * @param mangledSymbol the mangled symbol.
 * @return The demangled symbol, or mangledSymbol if demangling fails.
 */
std::string Demangle(const Twine& mangledSymbol);

}  // namespace wpi

#endif  // WPIUTIL_WPI_DEMANGLE_H_
