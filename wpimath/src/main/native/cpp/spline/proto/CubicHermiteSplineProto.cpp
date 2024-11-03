// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/proto/CubicHermiteSplineProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/spline.npb.h"

std::optional<frc::CubicHermiteSpline>
wpi::Protobuf<frc::CubicHermiteSpline>::Unpack(InputStream& stream) {
  wpi::WpiArrayUnpackCallback<double, 2> xInitial;
  wpi::WpiArrayUnpackCallback<double, 2> xFinal;
  wpi::WpiArrayUnpackCallback<double, 2> yInitial;
  wpi::WpiArrayUnpackCallback<double, 2> yFinal;
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

  return frc::CubicHermiteSpline{
      xInitial.Array(),
      xFinal.Array(),
      yInitial.Array(),
      yFinal.Array(),
  };
}

bool wpi::Protobuf<frc::CubicHermiteSpline>::Pack(
    OutputStream& stream, const frc::CubicHermiteSpline& value) {
  wpi::PackCallback<double> xInitial{value.GetInitialControlVector().x};
  wpi::PackCallback<double> xFinal{value.GetFinalControlVector().x};
  wpi::PackCallback<double> yInitial{value.GetInitialControlVector().y};
  wpi::PackCallback<double> yFinal{value.GetFinalControlVector().y};
  wpi_proto_ProtobufCubicHermiteSpline msg{
      .x_initial = xInitial.Callback(),
      .x_final = xFinal.Callback(),
      .y_initial = yInitial.Callback(),
      .y_final = yFinal.Callback(),
  };
  return stream.Encode(msg);
}
