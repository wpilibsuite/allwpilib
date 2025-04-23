// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gtest/gtest.h>
#include <wpi/struct/Struct.h>

template <typename T>
class StructTest : public testing::Test {};

TYPED_TEST_SUITE_P(StructTest);

// For these tests:
// TypeParam defines Type, kTestData, and CheckEq
// Type is the data type
// StructType is the instantiation of wpi::Struct<>

TYPED_TEST_P(StructTest, RoundTrip) {
  using Type = typename TypeParam::Type;
  using StructType = wpi::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::PackStruct(buffer, TypeParam::kTestData);

  Type unpacked_data = wpi::UnpackStruct<Type>(buffer);
  TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
}

TYPED_TEST_P(StructTest, DoublePack) {
  using Type = typename TypeParam::Type;
  using StructType = wpi::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::PackStruct(buffer, TypeParam::kTestData);
  wpi::PackStruct(buffer, TypeParam::kTestData);

  Type unpacked_data = wpi::UnpackStruct<Type>(buffer);
  TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
}

TYPED_TEST_P(StructTest, DoubleUnpack) {
  using Type = typename TypeParam::Type;
  using StructType = wpi::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::PackStruct(buffer, TypeParam::kTestData);

  {
    Type unpacked_data = wpi::UnpackStruct<Type>(buffer);
    TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
  }

  {
    Type unpacked_data = wpi::UnpackStruct<Type>(buffer);
    TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
  }
}

REGISTER_TYPED_TEST_SUITE_P(StructTest, RoundTrip, DoublePack, DoubleUnpack);
