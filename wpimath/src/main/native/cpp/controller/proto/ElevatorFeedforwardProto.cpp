// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/ElevatorFeedforwardProto.h"

#include <optional>

#include "wpimath/protobuf/controller.npb.h"

template <typename Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
std::optional<frc::ElevatorFeedforward<Input>>
wpi::Protobuf<frc::ElevatorFeedforward<Input>>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufElevatorFeedforward msg;
  if (!stream.Decode(msg)) {
    return std::nullopt;
  }

  return frc::ElevatorFeedforward<Input>{
      units::unit_t<Input>{msg.ks},
      units::unit_t<Input>{msg.kg},
      units::unit_t<typename frc::ElevatorFeedforward<Input>::kv_unit>{msg.kv},
      units::unit_t<typename frc::ElevatorFeedforward<Input>::ka_unit>{msg.ka},
      units::second_t{msg.dt}};
}

template <typename Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
bool wpi::Protobuf<frc::ElevatorFeedforward<Input>>::Pack(
    OutputStream& stream, const frc::ElevatorFeedforward<Input>& value) {
  wpi_proto_ProtobufElevatorFeedforward msg;
  msg.ks = value.GetKs().value();
  msg.kg = value.GetKg().value();
  msg.kv = value.GetKv().value();
  msg.ka = value.GetKa().value();
  msg.dt = value.GetDt().value();

  return stream.Encode(msg);
}

template struct wpi::Protobuf<frc::ElevatorFeedforward<units::volt>>;
template struct wpi::Protobuf<frc::ElevatorFeedforward<units::ampere>>;

