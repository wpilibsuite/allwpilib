// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "TagDetection.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<TagDetection> {
  static constexpr std::string_view GetTypeName() { return "TagDetection"; }

  static constexpr size_t GetSize() { return ((8 * 2) * 4 + 4); }

  static constexpr std::string_view GetSchema() {
    return "uint32 id;double cx1;double cy1;double cx2;double cy2;double "
           "cx3;double cy3;double cx4;double cy4";
  }

  static TagDetection Unpack(std::span<const uint8_t> data) {
    return TagDetection{wpi::UnpackStruct<int32_t, 0>(data),
                        {
                            {
                                wpi::UnpackStruct<double, 4 + 8 * 0>(data),
                                wpi::UnpackStruct<double, 4 + 8 * 1>(data),
                            },
                            {
                                wpi::UnpackStruct<double, 4 + 8 * 2>(data),
                                wpi::UnpackStruct<double, 4 + 8 * 3>(data),
                            },
                            {
                                wpi::UnpackStruct<double, 4 + 8 * 4>(data),
                                wpi::UnpackStruct<double, 4 + 8 * 5>(data),
                            },
                            {
                                wpi::UnpackStruct<double, 4 + 8 * 6>(data),
                                wpi::UnpackStruct<double, 4 + 8 * 7>(data),
                            },
                        }};
  }

  static void Pack(std::span<uint8_t> data, const TagDetection& value) {
    wpi::PackStruct<0>(data, value.id);
    wpi::PackStruct<4 + 8 * 0>(data, value.corners[0].x);
    wpi::PackStruct<4 + 8 * 1>(data, value.corners[0].y);
    wpi::PackStruct<4 + 8 * 2>(data, value.corners[1].x);
    wpi::PackStruct<4 + 8 * 3>(data, value.corners[1].y);
    wpi::PackStruct<4 + 8 * 4>(data, value.corners[2].x);
    wpi::PackStruct<4 + 8 * 5>(data, value.corners[2].y);
    wpi::PackStruct<4 + 8 * 6>(data, value.corners[3].x);
    wpi::PackStruct<4 + 8 * 7>(data, value.corners[3].y);
  }
};

static_assert(wpi::StructSerializable<TagDetection>);
