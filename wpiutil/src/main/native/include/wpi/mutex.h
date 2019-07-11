/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
