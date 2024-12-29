// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Pose3WithVariance.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<Pose3WithVariance> {
  static constexpr size_t poseSize = wpi::Struct<frc::Pose3d>::GetSize();

  static constexpr std::string_view GetTypeName() {
    return "Pose3WithVariance";
  }

  static constexpr size_t GetSize() { return poseSize + (6 * sizeof(double)); }

  static constexpr std::string_view GetSchema() {
    return "Pose3d pose; double covariance[6]";
  }

  static Pose3WithVariance Unpack(std::span<const uint8_t> data) {
    return Pose3WithVariance{
        wpi::UnpackStruct<frc::Pose3d, 0>(data),
        {
            wpi::UnpackStruct<double, poseSize + 0 * sizeof(double)>(data),
            wpi::UnpackStruct<double, poseSize + 1 * sizeof(double)>(data),
            wpi::UnpackStruct<double, poseSize + 2 * sizeof(double)>(data),
            wpi::UnpackStruct<double, poseSize + 3 * sizeof(double)>(data),
            wpi::UnpackStruct<double, poseSize + 4 * sizeof(double)>(data),
            wpi::UnpackStruct<double, poseSize + 5 * sizeof(double)>(data),
        }};
  }

  static void Pack(std::span<uint8_t> data, const Pose3WithVariance& value) {
    wpi::PackStruct<0>(data, value.pose);
    wpi::PackStruct<poseSize + sizeof(double) * 0>(data, value.covariance[0]);
    wpi::PackStruct<poseSize + sizeof(double) * 1>(data, value.covariance[1]);
    wpi::PackStruct<poseSize + sizeof(double) * 2>(data, value.covariance[2]);
    wpi::PackStruct<poseSize + sizeof(double) * 3>(data, value.covariance[3]);
    wpi::PackStruct<poseSize + sizeof(double) * 4>(data, value.covariance[4]);
    wpi::PackStruct<poseSize + sizeof(double) * 5>(data, value.covariance[5]);
  }

  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Pose3d>(fn);
  }
};

static_assert(wpi::StructSerializable<Pose3WithVariance>);
