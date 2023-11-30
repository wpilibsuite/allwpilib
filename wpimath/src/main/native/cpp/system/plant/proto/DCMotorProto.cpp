// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/system/plant/proto/DCMotorProto.h"

#include "plant.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::DCMotor>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufDCMotor>(
      arena);
}

frc::DCMotor wpi::Protobuf<frc::DCMotor>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufDCMotor*>(&msg);
  return frc::DCMotor{
      units::volt_t{m->nominal_voltage()},
      units::newton_meter_t{m->stall_torque()},
      units::ampere_t{m->stall_current()},
      units::ampere_t{m->free_current()},
      units::radians_per_second_t{m->free_speed()},
  };
}

void wpi::Protobuf<frc::DCMotor>::Pack(google::protobuf::Message* msg,
                                       const frc::DCMotor& value) {
  auto m = static_cast<wpi::proto::ProtobufDCMotor*>(msg);
  m->set_nominal_voltage(value.nominalVoltage.value());
  m->set_stall_torque(value.stallTorque.value());
  m->set_stall_current(value.stallCurrent.value());
  m->set_free_current(value.freeCurrent.value());
  m->set_free_speed(value.freeSpeed.value());
}
