// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/struct/Struct.h>

#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Represents a quad element on a Canvas2d. To draw on a Canvas2d, use
 * Canvas2d#DrawQuad} or Canvas2d#DrawRect.
 */
struct CanvasQuad2d {
  float x1;
  float y1;
  float x2;
  float y2;
  float x3;
  float y3;
  float x4;
  float y4;
  float weight;
  bool fill;
  frc::Color8Bit color;
  int opacity;
  int zOrder;

  CanvasQuad2d(float x1, float y1, float x2, float y2, float x3, float y3,
               float x4, float y4, float weight, bool fill,
               const frc::Color8Bit& color, int opacity, int zOrder)
      : x1{x1},
        y1{y1},
        x2{x2},
        y2{y2},
        x3{x3},
        y3{y3},
        x4{x4},
        y4{y4},
        weight{weight},
        fill{fill},
        color{color},
        opacity{std::clamp(opacity, 0, 255)},
        zOrder{zOrder} {}
};
}  // namespace frc

template <>
struct wpi::Struct<frc::CanvasQuad2d> {
  static constexpr std::string_view GetTypeString() {
    return "struct:CanvasQuad2d";
  }
  static constexpr size_t GetSize() { return 45; }
  static constexpr std::string_view GetSchema() {
    return "float x1;float y1;float x2;float y2;float x3;float y3;float "
           "x4;float y4;float weight;bool fill;uint8 color[4];int32 zOrder";
  }

  static frc::CanvasQuad2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::CanvasQuad2d& value);
};

static_assert(wpi::StructSerializable<frc::CanvasQuad2d>);
