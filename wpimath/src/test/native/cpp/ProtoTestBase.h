// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gtest/gtest.h>
#include <wpi/protobuf/Protobuf.h>

#include "controller.pb.h"
#include "kinematics.pb.h"
#include "spline.pb.h"
#include "system.pb.h"
#include "wpimath.pb.h"

template <typename T>
class ProtoTest : public testing::Test {};

TYPED_TEST_SUITE_P(ProtoTest);

TYPED_TEST_P(ProtoTest, RoundTrip) {
  using Type = typename TypeParam::Type;
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = wpi::Protobuf<Type>::New(&arena);
  wpi::PackProtobuf(proto, TypeParam::kTestData);

  Type unpacked_data = wpi::UnpackProtobuf<Type>(*proto);
  TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
}

TYPED_TEST_P(ProtoTest, DoublePack) {
  using Type = typename TypeParam::Type;
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = wpi::Protobuf<Type>::New(&arena);
  wpi::PackProtobuf(proto, TypeParam::kTestData);
  wpi::PackProtobuf(proto, TypeParam::kTestData);

  Type unpacked_data = wpi::UnpackProtobuf<Type>(*proto);
  TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
}

TYPED_TEST_P(ProtoTest, DoubleUnpack) {
  using Type = typename TypeParam::Type;
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = wpi::Protobuf<Type>::New(&arena);
  wpi::PackProtobuf(proto, TypeParam::kTestData);

  {
    Type unpacked_data = wpi::UnpackProtobuf<Type>(*proto);
    TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
  }

  {
    Type unpacked_data = wpi::UnpackProtobuf<Type>(*proto);
    TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
  }
}

REGISTER_TYPED_TEST_SUITE_P(ProtoTest, RoundTrip, DoublePack, DoubleUnpack);
