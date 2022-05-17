// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/CoordinateSystem.h"

#include <cmath>
#include <stdexcept>
#include <utility>

#include "Eigen/QR"

using namespace frc;

CoordinateSystem::CoordinateSystem(const CoordinateAxis& positiveX,
                                   const CoordinateAxis& positiveY,
                                   const CoordinateAxis& positiveZ) {
  // Construct a change of basis matrix from the source coordinate system to the
  // NWU coordinate system. Each column vector in the change of basis matrix is
  // one of the old basis vectors mapped to its representation in the new basis.
  Matrixd<3, 3> R;
  R.block<3, 1>(0, 0) = positiveX.m_axis;
  R.block<3, 1>(0, 1) = positiveY.m_axis;
  R.block<3, 1>(0, 2) = positiveZ.m_axis;

  // Require that the change of basis matrix is special orthogonal. This is true
  // if the axes used are orthogonal and normalized. The Axis class already
  // normalizes itself, so we just need to check for orthogonality.
  if (R * R.transpose() != Matrixd<3, 3>::Identity()) {
    throw std::domain_error("Coordinate system isn't special orthogonal");
  }

  // Turn change of basis matrix into a quaternion since it's a pure rotation
  // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
  double trace = R(0, 0) + R(1, 1) + R(2, 2);
  double w;
  double x;
  double y;
  double z;

  if (trace > 0.0) {
    double s = 0.5 / std::sqrt(trace + 1.0);
    w = 0.25 / s;
    x = (R(2, 1) - R(1, 2)) * s;
    y = (R(0, 2) - R(2, 0)) * s;
    z = (R(1, 0) - R(0, 1)) * s;
  } else {
    if (R(0, 0) > R(1, 1) && R(0, 0) > R(2, 2)) {
      double s = 2.0 * std::sqrt(1.0 + R(0, 0) - R(1, 1) - R(2, 2));
      w = (R(2, 1) - R(1, 2)) / s;
      x = 0.25 * s;
      y = (R(0, 1) + R(1, 0)) / s;
      z = (R(0, 2) + R(2, 0)) / s;
    } else if (R(1, 1) > R(2, 2)) {
      double s = 2.0 * std::sqrt(1.0 + R(1, 1) - R(0, 0) - R(2, 2));
      w = (R(0, 2) - R(2, 0)) / s;
      x = (R(0, 1) + R(1, 0)) / s;
      y = 0.25 * s;
      z = (R(1, 2) + R(2, 1)) / s;
    } else {
      double s = 2.0 * std::sqrt(1.0 + R(2, 2) - R(0, 0) - R(1, 1));
      w = (R(1, 0) - R(0, 1)) / s;
      x = (R(0, 2) + R(2, 0)) / s;
      y = (R(1, 2) + R(2, 1)) / s;
      z = 0.25 * s;
    }
  }

  m_rotation = Rotation3d{Quaternion{w, x, y, z}};
}

CoordinateSystem CoordinateSystem::NWU() {
  static CoordinateSystem instance{CoordinateAxis::N(), CoordinateAxis::W(),
                                   CoordinateAxis::U()};
  return instance;
}

CoordinateSystem CoordinateSystem::EDN() {
  static CoordinateSystem instance{CoordinateAxis::E(), CoordinateAxis::D(),
                                   CoordinateAxis::N()};
  return instance;
}

CoordinateSystem CoordinateSystem::NED() {
  static CoordinateSystem instance{CoordinateAxis::N(), CoordinateAxis::E(),
                                   CoordinateAxis::D()};
  return instance;
}

Pose3d CoordinateSystem::Convert(const Pose3d& pose,
                                 const CoordinateSystem& from,
                                 const CoordinateSystem& to) {
  return pose.RelativeTo(
      Pose3d{Translation3d{}, to.m_rotation - from.m_rotation});
}
