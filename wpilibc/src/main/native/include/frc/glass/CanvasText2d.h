// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <string_view>

#include <wpi/struct/Struct.h>

#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Represents the data for a text element on a Canvas2d. To draw on a Canvas2d,
 * use Canvas2d#DrawText.
 */
struct CanvasText2d {
  float x1;
  float y1;
  float fontSize;
  std::array<char, 64> text;
  frc::Color8Bit color;
  int opacity;
  float wrapWidth;
  int zOrder;

  CanvasText2d(float x1, float y1, float fontSize, std::string_view text,
               const frc::Color8Bit& color, int opacity, float wrapWidth,
               int zOrder)
      : x1{x1},
        y1{y1},
        fontSize{fontSize},
        color{color},
        opacity{std::clamp(opacity, 0, 255)},
        wrapWidth{wrapWidth},
        zOrder{zOrder} {
    std::copy_n(text.begin(), std::min<size_t>(text.size(), this->text.size()),
                this->text.begin());
  }

  CanvasText2d(float x1, float y1, float fontSize, std::array<char, 64> text,
               const frc::Color8Bit& color, int opacity, float wrapWidth,
               int zOrder)
      : x1{x1},
        y1{y1},
        fontSize{fontSize},
        text{text},
        color{color},
        opacity{std::clamp(opacity, 0, 255)},
        wrapWidth{wrapWidth},
        zOrder{zOrder} {}
};
}  // namespace frc

template <>
struct wpi::Struct<frc::CanvasText2d> {
  static constexpr std::string_view GetTypeString() {
    return "struct:CanvasText2d";
  }
  static constexpr size_t GetSize() { return 88; }
  static constexpr std::string_view GetSchema() {
    return "float x;float y;float fontSize;char text[64];uint8 color[4];float "
           "wrapWidth;int32 zOrder";
  }

  static frc::CanvasText2d Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const frc::CanvasText2d& value);
};

static_assert(wpi::StructSerializable<frc::CanvasText2d>);
