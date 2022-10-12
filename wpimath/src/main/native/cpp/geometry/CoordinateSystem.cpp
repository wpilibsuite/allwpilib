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

  // The change of basis matrix should be a pure rotation. The Rotation3d
  // constructor will verify this by checking for special orthogonality.
  m_rotation = Rotation3d{R};
}

const CoordinateSystem& CoordinateSystem::NWU() {
  static const CoordinateSystem instance{
      CoordinateAxis::N(), CoordinateAxis::W(), CoordinateAxis::U()};
  return instance;
}

const CoordinateSystem& CoordinateSystem::EDN() {
  static const CoordinateSystem instance{
      CoordinateAxis::E(), CoordinateAxis::D(), CoordinateAxis::N()};
  return instance;
}

const CoordinateSystem& CoordinateSystem::NED() {
  static const CoordinateSystem instance{
      CoordinateAxis::N(), CoordinateAxis::E(), CoordinateAxis::D()};
  return instance;
}

Translation3d CoordinateSystem::Convert(const Translation3d& translation,
                                        const CoordinateSystem& from,
                                        const CoordinateSystem& to) {
  return translation.RotateBy(from.m_rotation - to.m_rotation);
}

Rotation3d CoordinateSystem::Convert(const Rotation3d& rotation,
                                     const CoordinateSystem& from,
                                     const CoordinateSystem& to) {
  return rotation.RotateBy(from.m_rotation - to.m_rotation);
}

Pose3d CoordinateSystem::Convert(const Pose3d& pose,
                                 const CoordinateSystem& from,
                                 const CoordinateSystem& to) {
  return Pose3d{Convert(pose.Translation(), from, to),
                Convert(pose.Rotation(), from, to)};
}

Transform3d CoordinateSystem::Convert(const Transform3d& transform,
                                      const CoordinateSystem& from,
                                      const CoordinateSystem& to) {
  return Transform3d{Convert(transform.Translation(), from, to),
                     Convert(transform.Rotation(), from, to)};
}
