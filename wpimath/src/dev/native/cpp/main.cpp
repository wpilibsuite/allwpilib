// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <google/protobuf/arena.h>

#include <wpi/print.h>

#include <wpi/SmallVector.h>

#include "frc/geometry/Pose2d.h"

using namespace frc;

using ProtoType = wpi::Protobuf<frc::Pose2d>;

const Pose2d kExpectedData =
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}};

int main() {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  wpi::SmallVector<uint8_t, 64> gbuf;
  if (!wpi::detail::SerializeProtobuf(gbuf, *proto)) {
    printf(" proto Pack failed\n");
    return -1;
  }

  double dbl = kExpectedData.Translation().X().value();
  const uint8_t* val = (const uint8_t*)&dbl;
  printf("dbl %f\n", dbl);

  for (int i = 0; i < 8; i++) {
    printf("%2x ", (unsigned)val[i]);
  }
  printf("\n");

  wpi::SmallVector<uint8_t, 64> buf;
  wpi::ProtoOutputStream ostream{buf, ProtoType::Message()};

  if (!ProtoType::Pack(ostream, kExpectedData)) {
    printf("Pack failed\n");
    return -1;
  }

  printf("Buffer with size %d %d\n", (int)buf.size(), (int)gbuf.size());

  for (auto&& b : buf) {
    printf("%2x ", (unsigned)b);
  }
  printf("\n");

  for (auto&& b : gbuf) {
    printf("%2x ", (unsigned)b);
  }
  printf("\n");

  wpi::ProtoInputStream istream{buf, ProtoType::Message()};
  std::optional<Pose2d> unpacked_data = ProtoType::Unpack(istream);
  if (!unpacked_data.has_value()) {
    printf("unpack failed\n");
    return -1;
  }

  return 0;

  // EXPECT_EQ(kExpectedData.Translation(), unpacked_data->Translation());
  // EXPECT_EQ(kExpectedData.Rotation(), unpacked_data->Rotation());
}
