// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/controller/LinearQuadraticRegulator.h"

namespace wpimath {

template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    LinearQuadraticRegulator<1, 1>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    LinearQuadraticRegulator<2, 1>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    LinearQuadraticRegulator<2, 2>;

}  // namespace wpimath
