// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

#include <string_view>

namespace wpi {

void ThrowIllegalArgumentException(JNIEnv* env, std::string_view msg);
void ThrowIndexOobException(JNIEnv* env, std::string_view msg);
void ThrowIOException(JNIEnv* env, std::string_view msg);
void ThrowNullPointerException(JNIEnv* env, std::string_view msg);

}  // namespace wpi
