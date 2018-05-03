/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

// clang-format off
#ifdef _MSC_VER
#pragma message "warning: llvm/STLExtras.h is deprecated; include wpi/STLExtras.h instead"
#else
#warning "llvm/STLExtras.h is deprecated; include wpi/STLExtras.h instead"
#endif
// clang-format on

#include "wpi/STLExtras.h"

namespace llvm = wpi;
