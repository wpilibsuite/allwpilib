// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "frc/EigenCore.h"
#include "wpimath.npb.h"

template <int Size, int Options, int MaxRows, int MaxCols>
struct wpi::Protobuf<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>> {
  static const pb_msgdesc_t* Message() {
    return get_wpi_proto_ProtobufVector_msg();
  }

  static std::optional<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>> Unpack(
      wpi::ProtoInputStream& stream) {
    using UnpackType =
        std::conditional_t<Size * sizeof(double) < 256,  // NOLINT
                           wpi::UnpackCallback<double, Size>,
                           wpi::StdVectorUnpackCallback<double, Size>>;
    UnpackType rows;
    rows.Vec().reserve(Size);
    rows.SetLimits(wpi::DecodeLimits::Fail);
    _wpi_proto_ProtobufVector msg{
        .rows = rows.Callback(),
    };
    if (!stream.DecodeNoInit(msg)) {
      return {};
    }

    auto items = rows.Items();

    if (items.size() != Size) {
      return {};
    }

    frc::Matrixd<Size, 1, Options, MaxRows, MaxCols> mat;
    for (int i = 0; i < Size; i++) {
      mat(i) = items[i];
    }

    return mat;
  }

  static bool Pack(
      wpi::ProtoOutputStream& stream,
      const frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value) {
    std::span<const double> rowsSpan{value.data(), static_cast<size_t>(Size)};
    wpi::PackCallback<double> rows{rowsSpan};
    wpi_proto_ProtobufVector msg{
        .rows = rows.Callback(),
    };
    return stream.Encode(msg);
  }
};
