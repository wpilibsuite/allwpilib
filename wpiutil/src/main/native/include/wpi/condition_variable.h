/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <condition_variable>

#include "wpi/priority_mutex.h"

namespace wpi {

#if defined(__linux__) && defined(WPI_HAVE_PRIORITY_MUTEX)
using condition_variable = ::std::condition_variable_any;
#else
using condition_variable = ::std::condition_variable;
#endif

}  // namespace wpi
