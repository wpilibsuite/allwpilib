// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/struct/Struct.h>

#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Represents the data for a line element on a Canvas2d. To draw on a Canvas2d,
 * use Canvas2d#DrawLine.
 */
struct CanvasLine2d {
  float x1;
  float y1;
  float x2;
  float y2;
  float weight;
  frc::Color8Bit color;
  int opacity;
  int zOrder;

  CanvasLine2d(float x1, float y1, float x2, float y2, float weight,
               const frc::Color8Bit& color, int opacity, int zOrder)
      : x1{x1},
        y1{y1},
        x2{x2},
        y2{y2},
        weight{weight},
        color{color},
        opacity{std::clamp(opacity, 0, 255)},
        zOrder{zOrder} {}
};
}  // namespace frc

template <>
struct wpi::Struct<frc::CanvasLine2d> {
  static constexpr std::string_view GetTypeString() {
    return "struct:CanvasLine2d";
  }
  static constexpr size_t GetSize() { return 28; }
  static constexpr std::string_view GetSchema() {
    return "float x1;float y1;float x2;float y2;float weight;uint8 "
           "color[4];int32 zOrder";
  }

  static frc::CanvasLine2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::CanvasLine2d& value);
};

static_assert(wpi::StructSerializable<frc::CanvasLine2d>);
