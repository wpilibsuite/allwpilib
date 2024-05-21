// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/glass/CanvasCircle2d.h"

namespace {
constexpr size_t kXOff = 0;
constexpr size_t kYOff = kXOff + 4;
constexpr size_t kRadiusOff = kYOff + 4;
constexpr size_t kWeightOff = kRadiusOff + 4;
constexpr size_t kFillOff = kWeightOff + 4;
constexpr size_t kColorOff = kFillOff + 1;
constexpr size_t kZOrderOff = kColorOff + 4;
}  // namespace

using StructType = wpi::Struct<frc::CanvasCircle2d>;

frc::CanvasCircle2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::CanvasCircle2d{
      wpi::UnpackStruct<float, kXOff>(data),
      wpi::UnpackStruct<float, kYOff>(data),
      wpi::UnpackStruct<float, kRadiusOff>(data),
      wpi::UnpackStruct<float, kWeightOff>(data),
      wpi::UnpackStruct<bool, kFillOff>(data),
      frc::Color8Bit{wpi::UnpackStruct<uint8_t, kColorOff>(data),
                     wpi::UnpackStruct<uint8_t, kColorOff + 1>(data),
                     wpi::UnpackStruct<uint8_t, kColorOff + 2>(data)},
      wpi::UnpackStruct<uint8_t, kColorOff + 3>(data),
      wpi::UnpackStruct<int32_t, kZOrderOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::CanvasCircle2d& value) {
  wpi::PackStruct<kXOff>(data, value.x);
  wpi::PackStruct<kYOff>(data, value.y);
  wpi::PackStruct<kRadiusOff>(data, value.radius);
  wpi::PackStruct<kWeightOff>(data, value.weight);
  wpi::PackStruct<kFillOff>(data, value.fill);
  wpi::PackStruct<kColorOff>(data, static_cast<uint8_t>(value.color.red));
  wpi::PackStruct<kColorOff + 1>(data, static_cast<uint8_t>(value.color.green));
  wpi::PackStruct<kColorOff + 2>(data, static_cast<uint8_t>(value.color.blue));
  wpi::PackStruct<kColorOff + 3>(data, static_cast<uint8_t>(value.opacity));
  wpi::PackStruct<kZOrderOff>(data, value.zOrder);
}
