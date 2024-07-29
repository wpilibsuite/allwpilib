// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_DEMANGLE_H_
#define WPIUTIL_WPI_DEMANGLE_H_

#include <string>
#include <string_view>
#include <typeinfo>

namespace wpi {

/**
 * Demangle a C++ symbol.
 *
 * @param mangledSymbol the mangled symbol.
 * @return The demangled symbol, or mangledSymbol if demangling fails.
 */
std::string Demangle(std::string_view mangledSymbol);

/**
 * Returns the type name of an object
 * @param type The object
 */
template <typename T>
std::string GetTypeName(const T& type) {
  return Demangle(typeid(type).name());
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_DEMANGLE_H_
