// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/glass/CanvasText2d.h"

#include <wpi/struct/Struct.h>

namespace {
constexpr size_t kXOff = 0;
constexpr size_t kYOff = kXOff + 4;
constexpr size_t kFontSizeOff = kYOff + 4;
constexpr size_t kTextOff = kFontSizeOff + 4;
constexpr size_t kColorOff = kTextOff + 64;
constexpr size_t kWrapWidthOff = kColorOff + 4;
constexpr size_t kZOrderOff = kWrapWidthOff + 4;
}  // namespace

using StructType = wpi::Struct<frc::CanvasText2d>;

frc::CanvasText2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::CanvasText2d{
      wpi::UnpackStruct<float, kXOff>(data),
      wpi::UnpackStruct<float, kYOff>(data),
      wpi::UnpackStruct<float, kFontSizeOff>(data),
      wpi::UnpackStruct<std::array<char, 64>, kTextOff>(data),
      frc::Color8Bit{wpi::UnpackStruct<uint8_t, kColorOff>(data),
                     wpi::UnpackStruct<uint8_t, kColorOff + 1>(data),
                     wpi::UnpackStruct<uint8_t, kColorOff + 2>(data)},
      wpi::UnpackStruct<uint8_t, kColorOff + 3>(data),
      wpi::UnpackStruct<float, kWrapWidthOff>(data),
      wpi::UnpackStruct<int32_t, kZOrderOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::CanvasText2d& value) {
  wpi::PackStruct<kXOff>(data, value.x1);
  wpi::PackStruct<kYOff>(data, value.y1);
  wpi::PackStruct<kFontSizeOff>(data, value.fontSize);
  wpi::PackStruct<kTextOff>(data, value.text);
  wpi::PackStruct<kColorOff>(data, static_cast<uint8_t>(value.color.red));
  wpi::PackStruct<kColorOff + 1>(data, static_cast<uint8_t>(value.color.green));
  wpi::PackStruct<kColorOff + 2>(data, static_cast<uint8_t>(value.color.blue));
  wpi::PackStruct<kColorOff + 3>(data, static_cast<uint8_t>(value.opacity));
  wpi::PackStruct<kWrapWidthOff>(data, value.wrapWidth);
  wpi::PackStruct<kZOrderOff>(data, value.zOrder);
}
