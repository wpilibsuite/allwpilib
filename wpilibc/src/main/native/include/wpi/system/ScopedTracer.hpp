// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/system/Tracer.hpp"

namespace wpi::util {
class raw_ostream;
}  // namespace wpi::util

namespace wpi {
/**
 * A class for keeping track of how much time it takes for different
 * parts of code to execute. This class uses RAII, meaning you simply
 * need to create an instance at the top of the block you are timing. After the
 * block finishes execution (i.e. when the ScopedTracer instance gets
 * destroyed), the epoch is printed to the provided wpi::util::raw_ostream.
 */
class ScopedTracer {
 public:
  /**
   * Constructs a ScopedTracer instance.
   *
   * @param name The name of the epoch.
   * @param os A reference to the wpi::util::raw_ostream to print data to.
   */
  ScopedTracer(std::string_view name, wpi::util::raw_ostream& os);
  ~ScopedTracer();

  ScopedTracer(const ScopedTracer&) = delete;
  ScopedTracer& operator=(const ScopedTracer&) = delete;

 private:
  Tracer m_tracer;
  std::string m_name;
  wpi::util::raw_ostream& m_os;
};
}  // namespace wpi
