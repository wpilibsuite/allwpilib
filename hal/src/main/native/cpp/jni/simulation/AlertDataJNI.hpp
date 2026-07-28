// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

namespace wpi::hal::sim {
bool InitializeAlertDataJNI(JNIEnv* env);
void FreeAlertDataJNI(JNIEnv* env);
}  // namespace wpi::hal::sim
