// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <optional>

#include <wpi/math/system/plant/proto/DCMotorProto.h>

#include "wpimath/protobuf/plant.npb.h"

std::optional<wpi::math::DCMotor> wpi::Protobuf<wpi::math::DCMotor>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufDCMotor msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DCMotor{
      units::volt_t{msg.nominal_voltage},
      units::newton_meter_t{msg.stall_torque},
      units::ampere_t{msg.stall_current},
      units::ampere_t{msg.free_current},
      units::radians_per_second_t{msg.free_speed},
  };
}

bool wpi::Protobuf<wpi::math::DCMotor>::Pack(OutputStream& stream,
                                             const wpi::math::DCMotor& value) {
  wpi_proto_ProtobufDCMotor msg{
      .nominal_voltage = value.nominalVoltage.value(),
      .stall_torque = value.stallTorque.value(),
      .stall_current = value.stallCurrent.value(),
      .free_current = value.freeCurrent.value(),
      .free_speed = value.freeSpeed.value(),
  };
  return stream.Encode(msg);
}
