// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/proto/TrajectoryProto.h"

#include "trajectory.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Trajectory>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<wpi::proto::ProtobufTrajectory>(
      arena);
}

frc::Trajectory wpi::Protobuf<frc::Trajectory>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTrajectory*>(&msg);
  std::vector<frc::Trajectory::State> states;
  states.reserve(m->states().size());
  for (const auto& protoState : m->states()) {
    states.push_back(wpi::UnpackProtobuf<frc::Trajectory::State>(protoState));
  }
  return frc::Trajectory{states};
}

void wpi::Protobuf<frc::Trajectory>::Pack(google::protobuf::Message* msg,
                                          const frc::Trajectory& value) {
  auto m = static_cast<wpi::proto::ProtobufTrajectory*>(msg);
  m->mutable_states()->Reserve(value.States().size());
  for (const auto& state : value.States()) {
    wpi::PackProtobuf(m->add_states(), state);
  }
}
