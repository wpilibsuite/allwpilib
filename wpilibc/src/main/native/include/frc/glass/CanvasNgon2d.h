// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/struct/Struct.h>

#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Represents the data for a ngon element on a Canvas2d. To draw on a Canvas2d,
 * use Canvas2d#DrawNgon.
 */
struct CanvasNgon2d {
  float x;
  float y;
  float radius;
  int numSides;
  float weight;
  bool fill;
  frc::Color8Bit color;
  int opacity;
  int zOrder;

  CanvasNgon2d(float x, float y, float radius, int numSides, float weight,
               bool fill, const frc::Color8Bit& color, int opacity, int zOrder)
      : x{x},
        y{y},
        radius{radius},
        numSides{numSides},
        weight{weight},
        fill{fill},
        color{color},
        opacity{std::clamp(opacity, 0, 255)},
        zOrder{zOrder} {}
};
}  // namespace frc

template <>
struct wpi::Struct<frc::CanvasNgon2d> {
  static constexpr std::string_view GetTypeString() {
    return "struct:CanvasNgon2d";
  }
  static constexpr size_t GetSize() { return 29; }
  static constexpr std::string_view GetSchema() {
    return "float x;float y;float radius;int32 numSides;float weight;bool "
           "fill;uint8 color[4];int32 zOrder";
  }

  static frc::CanvasNgon2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::CanvasNgon2d& value);
};

static_assert(wpi::StructSerializable<frc::CanvasNgon2d>);
