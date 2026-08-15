// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <array>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/jni_util.hpp"

namespace {

jlong gCapacity;
void* gAddress;

jlong GetDirectBufferCapacity(JNIEnv*, jobject) {
  return gCapacity;
}

void* GetDirectBufferAddress(JNIEnv*, jobject) {
  return gAddress;
}

class JNIEnvFixture {
 public:
  JNIEnvFixture() {
    m_functions.GetDirectBufferCapacity = GetDirectBufferCapacity;
    m_functions.GetDirectBufferAddress = GetDirectBufferAddress;
    m_env.functions = &m_functions;
  }

  JNIEnv* GetEnv() { return &m_env; }

 private:
  JNINativeInterface_ m_functions{};
  JNIEnv m_env{};
};

TEST_CASE("JSpan creates a fixed extent direct buffer range", "[JSpan]") {
  JNIEnvFixture env;
  std::array<jbyte, 8> data{0, 1, 2, 3, 4, 5, 6, 7};
  gCapacity = data.size();
  gAddress = data.data();

  auto result = wpi::util::java::JSpan<const jbyte, 3>::Create(
      env.GetEnv(), reinterpret_cast<jobject>(data.data()), 2);

  REQUIRE(result);
  CHECK(result->data() == data.data() + 2);
  CHECK(result->size() == 3);
  CHECK((*result)[0] == 2);
  CHECK((*result)[2] == 4);
}

TEST_CASE("JSpan rejects an out of bounds fixed extent direct buffer range",
          "[JSpan]") {
  JNIEnvFixture env;
  std::array<jbyte, 8> data{};
  gCapacity = data.size();
  gAddress = data.data();

  auto result = wpi::util::java::JSpan<const jbyte, 3>::Create(
      env.GetEnv(), reinterpret_cast<jobject>(data.data()), 6);

  REQUIRE_FALSE(result);
  CHECK(
      result.error() ==
      wpi::util::java::JSpan<const jbyte, 3>::DirectBufferError::OUT_OF_BOUNDS);
}

}  // namespace
