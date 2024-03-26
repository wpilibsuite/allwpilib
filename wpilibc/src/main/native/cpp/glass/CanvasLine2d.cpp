// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/glass/CanvasLine2d.h"

namespace {
constexpr size_t kX1Off = 0;
constexpr size_t kY1Off = kX1Off + 4;
constexpr size_t kX2Off = kY1Off + 4;
constexpr size_t kY2Off = kX2Off + 4;
constexpr size_t kWeightOff = kY2Off + 4;
constexpr size_t kColorOff = kWeightOff + 4;
constexpr size_t kZOrderOff = kColorOff + 4;
}  // namespace

using StructType = wpi::Struct<frc::CanvasLine2d>;

frc::CanvasLine2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::CanvasLine2d{
      wpi::UnpackStruct<float, kX1Off>(data),
      wpi::UnpackStruct<float, kY1Off>(data),
      wpi::UnpackStruct<float, kX2Off>(data),
      wpi::UnpackStruct<float, kY2Off>(data),
      wpi::UnpackStruct<float, kWeightOff>(data),
      frc::Color8Bit{wpi::UnpackStruct<uint8_t, kColorOff>(data),
                     wpi::UnpackStruct<uint8_t, kColorOff + 1>(data),
                     wpi::UnpackStruct<uint8_t, kColorOff + 2>(data)},
      wpi::UnpackStruct<uint8_t, kColorOff + 3>(data),
      wpi::UnpackStruct<int32_t, kZOrderOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::CanvasLine2d& value) {
  wpi::PackStruct<kX1Off>(data, value.x1);
  wpi::PackStruct<kY1Off>(data, value.y1);
  wpi::PackStruct<kX2Off>(data, value.x2);
  wpi::PackStruct<kY2Off>(data, value.y2);
  wpi::PackStruct<kWeightOff>(data, value.weight);
  wpi::PackStruct<kColorOff>(data, static_cast<uint8_t>(value.color.red));
  wpi::PackStruct<kColorOff + 1>(data, static_cast<uint8_t>(value.color.green));
  wpi::PackStruct<kColorOff + 2>(data, static_cast<uint8_t>(value.color.blue));
  wpi::PackStruct<kColorOff + 3>(data, static_cast<uint8_t>(value.opacity));
  wpi::PackStruct<kZOrderOff>(data, value.zOrder);
}
