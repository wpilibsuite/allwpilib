// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/proto/TrajectoryStateProto.h"

#include "trajectory.pb.h"

google::protobuf::Message* wpi::Protobuf<frc::Trajectory::State>::New(
    google::protobuf::Arena* arena) {
  return google::protobuf::Arena::CreateMessage<
      wpi::proto::ProtobufTrajectoryState>(arena);
}

frc::Trajectory::State wpi::Protobuf<frc::Trajectory::State>::Unpack(
    const google::protobuf::Message& msg) {
  auto m = static_cast<const wpi::proto::ProtobufTrajectoryState*>(&msg);
  return frc::Trajectory::State{
      units::second_t{m->time()},
      units::meters_per_second_t{m->velocity()},
      units::meters_per_second_squared_t{m->acceleration()},
      wpi::UnpackProtobuf<frc::Pose2d>(m->pose()),
      units::curvature_t{m->curvature()},
  };
}

void wpi::Protobuf<frc::Trajectory::State>::Pack(
    google::protobuf::Message* msg, const frc::Trajectory::State& value) {
  auto m = static_cast<wpi::proto::ProtobufTrajectoryState*>(msg);
  m->set_time(value.t.value());
  m->set_velocity(value.velocity.value());
  m->set_acceleration(value.acceleration.value());
  wpi::PackProtobuf(m->mutable_pose(), value.pose);
  m->set_curvature(value.curvature.value());
}
