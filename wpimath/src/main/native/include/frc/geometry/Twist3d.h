// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "frc/geometry/Rotation3d.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {
/**
 * A change in distance along a 3D arc since the last pose update. We can use
 * ideas from differential calculus to create new Pose3ds from a Twist3d and
 * vice versa.
 *
 * A Twist can be used to represent a difference between two poses.
 */
struct WPILIB_DLLEXPORT Twist3d {
  /**
   * Linear "dx" component
   */
  units::meter_t dx = 0_m;

  /**
   * Linear "dy" component
   */
  units::meter_t dy = 0_m;

  /**
   * Linear "dz" component
   */
  units::meter_t dz = 0_m;

  /**
   * Rotation vector x component.
   */
  units::radian_t rx = 0_rad;

  /**
   * Rotation vector y component.
   */
  units::radian_t ry = 0_rad;

  /**
   * Rotation vector z component.
   */
  units::radian_t rz = 0_rad;

  /**
   * Checks equality between this Twist3d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Twist3d& other) const {
    return units::math::abs(dx - other.dx) < 1E-9_m &&
           units::math::abs(dy - other.dy) < 1E-9_m &&
           units::math::abs(dz - other.dz) < 1E-9_m &&
           units::math::abs(rx - other.rx) < 1E-9_rad &&
           units::math::abs(ry - other.ry) < 1E-9_rad &&
           units::math::abs(rz - other.rz) < 1E-9_rad;
  }

  /**
   * Scale this by a given factor.
   *
   * @param factor The factor by which to scale.
   * @return The scaled Twist3d.
   */
  constexpr Twist3d operator*(double factor) const {
    return Twist3d{dx * factor, dy * factor, dz * factor,
                   rx * factor, ry * factor, rz * factor};
  }
};
}  // namespace frc

template <>
struct wpi::Struct<frc::Twist3d> {
  static constexpr std::string_view kTypeString = "struct:Twist3d";
  static constexpr size_t kSize = 48;
  static constexpr std::string_view kSchema =
      "double dx;double dy;double dz;double rx;double ry;double rz";
  static frc::Twist3d Unpack(std::span<const uint8_t, 48> data) {
    return {units::meter_t{wpi::UnpackStruct<double, 0>(data)},
            units::meter_t{wpi::UnpackStruct<double, 8>(data)},
            units::meter_t{wpi::UnpackStruct<double, 16>(data)},
            units::radian_t{wpi::UnpackStruct<double, 24>(data)},
            units::radian_t{wpi::UnpackStruct<double, 32>(data)},
            units::radian_t{wpi::UnpackStruct<double, 40>(data)}};
  }
  static void Pack(std::span<uint8_t, 48> data, const frc::Twist3d& value) {
    wpi::PackStruct<0>(data, value.dx.value());
    wpi::PackStruct<8>(data, value.dy.value());
    wpi::PackStruct<16>(data, value.dz.value());
    wpi::PackStruct<24>(data, value.rx.value());
    wpi::PackStruct<32>(data, value.ry.value());
    wpi::PackStruct<40>(data, value.rz.value());
  }
};

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Twist3d> {
  static constexpr std::string_view kTypeString = "proto:Twist3d";
  static google::protobuf::Message* New(google::protobuf::Arena* arena);
  static frc::Twist3d Unpack(const google::protobuf::Message& msg);
  static void Pack(google::protobuf::Message* msg, const frc::Twist3d& value);
};
