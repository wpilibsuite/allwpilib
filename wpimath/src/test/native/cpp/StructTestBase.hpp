// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/struct/Struct.hpp"

// For these tests:
// TypeParam defines Type, kTestData, and CheckEq
// Type is the data type
// StructType is the instantiation of wpi::util::Struct<>

template <typename TypeParam>
void CheckStructRoundTrip() {
  using Type = typename TypeParam::Type;
  using StructType = wpi::util::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::util::PackStruct(buffer, TypeParam::kTestData);

  Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
  TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
}

template <typename TypeParam>
void CheckStructDoublePack() {
  using Type = typename TypeParam::Type;
  using StructType = wpi::util::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::util::PackStruct(buffer, TypeParam::kTestData);
  wpi::util::PackStruct(buffer, TypeParam::kTestData);

  Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
  TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
}

template <typename TypeParam>
void CheckStructDoubleUnpack() {
  using Type = typename TypeParam::Type;
  using StructType = wpi::util::Struct<Type>;
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  wpi::util::PackStruct(buffer, TypeParam::kTestData);

  {
    Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
    TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
  }

  {
    Type unpacked_data = wpi::util::UnpackStruct<Type>(buffer);
    TypeParam::CheckEq(TypeParam::kTestData, unpacked_data);
  }
}

#define INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, Suite, TypeParam) \
  TEST_CASE(#Prefix " " #Suite " RoundTrip", "[wpimath]") {      \
    CheckStructRoundTrip<TypeParam>();                           \
  }                                                              \
  TEST_CASE(#Prefix " " #Suite " DoublePack", "[wpimath]") {     \
    CheckStructDoublePack<TypeParam>();                          \
  }                                                              \
  TEST_CASE(#Prefix " " #Suite " DoubleUnpack", "[wpimath]") {   \
    CheckStructDoubleUnpack<TypeParam>();                        \
  }
