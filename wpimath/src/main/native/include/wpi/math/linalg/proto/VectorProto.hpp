// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/wpimath.npb.h"

template <int Size, int Options, int MaxRows, int MaxCols>
struct wpi::util::Protobuf<wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>> {
  using MessageStruct = wpi_proto_ProtobufVector;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>>;

  static std::optional<wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>> Unpack(
      InputStream& stream) {
    constexpr bool isSmall = Size * sizeof(double) < 256;
    using UnpackType =
        std::conditional_t<isSmall, wpi::util::UnpackCallback<double, Size>,
                           wpi::util::StdVectorUnpackCallback<double, Size>>;
    UnpackType rows;
    rows.Vec().reserve(Size);
    rows.SetLimits(wpi::util::DecodeLimits::Fail);
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

    wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols> mat;
    for (int i = 0; i < Size; i++) {
      mat(i) = items[i];
    }

    return mat;
  }

  static bool Pack(
      OutputStream& stream,
      const wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value) {
    std::span<const double> rowsSpan{value.data(), static_cast<size_t>(Size)};
    wpi::util::PackCallback<double> rows{rowsSpan};
    wpi_proto_ProtobufVector msg{
        .rows = rows.Callback(),
    };
    return stream.Encode(msg);
  }
};
