// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "frc/EigenCore.h"
#include "wpimath/protobuf/wpimath.npb.h"

template <int Size, int Options, int MaxRows, int MaxCols>
struct wpi::Protobuf<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>> {
  using MessageStruct = wpi_proto_ProtobufVector;
  using InputStream =
      wpi::ProtoInputStream<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>>;
  using OutputStream =
      wpi::ProtoOutputStream<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>>;

  static std::optional<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>> Unpack(
      InputStream& stream) {
    constexpr bool isSmall = Size * sizeof(double) < 256;
    using UnpackType =
        std::conditional_t<isSmall, wpi::UnpackCallback<double, Size>,
                           wpi::StdVectorUnpackCallback<double, Size>>;
    UnpackType rows;
    rows.Vec().reserve(Size);
    rows.SetLimits(wpi::DecodeLimits::Fail);
    wpi_proto_ProtobufVector msg{
        .rows = rows.Callback(),
    };
    if (!stream.Decode(msg)) {
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
      OutputStream& stream,
      const frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value) {
    std::span<const double> rowsSpan{value.data(), static_cast<size_t>(Size)};
    wpi::PackCallback<double> rows{rowsSpan};
    wpi_proto_ProtobufVector msg{
        .rows = rows.Callback(),
    };
    return stream.Encode(msg);
  }
};
