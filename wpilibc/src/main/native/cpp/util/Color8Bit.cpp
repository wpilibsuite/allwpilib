/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/util/Color8Bit.h"

using namespace frc;

constexpr Color8Bit::Color8Bit(Color color) : red(color.red * 255), green(color.green * 255), blue(color.blue * 255) {}
