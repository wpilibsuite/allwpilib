// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/CoordinateAxis.h"

using namespace frc;

CoordinateAxis::CoordinateAxis(double x, double y, double z) : m_axis{x, y, z} {
  m_axis /= m_axis.norm();
}

CoordinateAxis CoordinateAxis::N() {
  return CoordinateAxis{1.0, 0.0, 0.0};
}

CoordinateAxis CoordinateAxis::S() {
  return CoordinateAxis{-1.0, 0.0, 0.0};
}

CoordinateAxis CoordinateAxis::E() {
  return CoordinateAxis{0.0, -1.0, 0.0};
}

CoordinateAxis CoordinateAxis::W() {
  return CoordinateAxis{0.0, 1.0, 0.0};
}

CoordinateAxis CoordinateAxis::U() {
  return CoordinateAxis{0.0, 0.0, 1.0};
}

CoordinateAxis CoordinateAxis::D() {
  return CoordinateAxis{0.0, 0.0, -1.0};
}
