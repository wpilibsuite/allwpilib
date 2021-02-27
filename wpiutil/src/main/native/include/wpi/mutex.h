// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <mutex>

#include "priority_mutex.h"

namespace wpi {

#ifdef WPI_HAVE_PRIORITY_MUTEX
using mutex = priority_mutex;
using recursive_mutex = priority_recursive_mutex;
#else
using mutex = ::std::mutex;
using recursive_mutex = ::std::recursive_mutex;
#endif

}  // namespace wpi
