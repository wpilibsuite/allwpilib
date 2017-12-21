/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <condition_variable>

#include "priority_condition_variable.h"

namespace wpi {

#ifdef WPI_HAVE_PRIORITY_CONDITION_VARIABLE
using condition_variable = priority_condition_variable;
#else
using condition_variable = ::std::condition_variable;
#endif

}  // namespace wpi
