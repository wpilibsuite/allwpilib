// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/spline/proto/CubicHermiteSplineProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/spline.npb.h"

std::optional<wpi::math::CubicHermiteSpline>
wpi::util::Protobuf<wpi::math::CubicHermiteSpline>::Unpack(InputStream& stream) {
  wpi::util::WpiArrayUnpackCallback<double, 2> xInitial;
  wpi::util::WpiArrayUnpackCallback<double, 2> xFinal;
  wpi::util::WpiArrayUnpackCallback<double, 2> yInitial;
  wpi::util::WpiArrayUnpackCallback<double, 2> yFinal;
  wpi_proto_ProtobufCubicHermiteSpline msg{
      .x_initial = xInitial.Callback(),
      .x_final = xFinal.Callback(),
      .y_initial = yInitial.Callback(),
      .y_final = yFinal.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  if (!xInitial.IsFull() || !yInitial.IsFull() || !xFinal.IsFull() ||
      !yFinal.IsFull()) {
    return {};
  }

  return wpi::math::CubicHermiteSpline{
      xInitial.Array(),
      xFinal.Array(),
      yInitial.Array(),
      yFinal.Array(),
  };
}

bool wpi::util::Protobuf<wpi::math::CubicHermiteSpline>::Pack(
    OutputStream& stream, const wpi::math::CubicHermiteSpline& value) {
  wpi::util::PackCallback<double> xInitial{value.GetInitialControlVector().x};
  wpi::util::PackCallback<double> xFinal{value.GetFinalControlVector().x};
  wpi::util::PackCallback<double> yInitial{value.GetInitialControlVector().y};
  wpi::util::PackCallback<double> yFinal{value.GetFinalControlVector().y};
  wpi_proto_ProtobufCubicHermiteSpline msg{
      .x_initial = xInitial.Callback(),
      .x_final = xFinal.Callback(),
      .y_initial = yInitial.Callback(),
      .y_final = yFinal.Callback(),
  };
  return stream.Encode(msg);
}
