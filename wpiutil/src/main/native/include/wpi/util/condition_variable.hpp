// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <condition_variable>

#include "wpi/util/priority_mutex.hpp"

namespace wpi::util {

#if defined(__linux__) && defined(WPI_HAVE_PRIORITY_MUTEX)
using condition_variable = ::std::condition_variable_any;
#else
using condition_variable = ::std::condition_variable;
#endif

}  // namespace wpi::util
