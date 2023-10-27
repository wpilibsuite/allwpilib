// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "units/angle.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {
/**
 * A change in distance along a 2D arc since the last pose update. We can use
 * ideas from differential calculus to create new Pose2ds from a Twist2d and
 * vice versa.
 *
 * A Twist can be used to represent a difference between two poses.
 */
struct WPILIB_DLLEXPORT Twist2d {
  /**
   * Linear "dx" component
   */
  units::meter_t dx = 0_m;

  /**
   * Linear "dy" component
   */
  units::meter_t dy = 0_m;

  /**
   * Angular "dtheta" component (radians)
   */
  units::radian_t dtheta = 0_rad;

  /**
   * Checks equality between this Twist2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Twist2d& other) const {
    return units::math::abs(dx - other.dx) < 1E-9_m &&
           units::math::abs(dy - other.dy) < 1E-9_m &&
           units::math::abs(dtheta - other.dtheta) < 1E-9_rad;
  }

  /**
   * Scale this by a given factor.
   *
   * @param factor The factor by which to scale.
   * @return The scaled Twist2d.
   */
  constexpr Twist2d operator*(double factor) const {
    return Twist2d{dx * factor, dy * factor, dtheta * factor};
  }
};
}  // namespace frc

template <>
struct wpi::Struct<frc::Twist2d> {
  static constexpr std::string_view kTypeString = "struct:Twist2d";
  static constexpr size_t kSize = 24;
  static constexpr std::string_view kSchema =
      "double dx;double dy;double dtheta";
  static frc::Twist2d Unpack(std::span<const uint8_t, 24> data) {
    return {units::meter_t{wpi::UnpackStruct<double, 0>(data)},
            units::meter_t{wpi::UnpackStruct<double, 8>(data)},
            units::radian_t{wpi::UnpackStruct<double, 16>(data)}};
  }
  static void Pack(std::span<uint8_t, 24> data, const frc::Twist2d& value) {
    wpi::PackStruct<0>(data, value.dx.value());
    wpi::PackStruct<8>(data, value.dy.value());
    wpi::PackStruct<16>(data, value.dtheta.value());
  }
};

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Twist2d> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Twist2d Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg, const frc::Twist2d& value);
};
