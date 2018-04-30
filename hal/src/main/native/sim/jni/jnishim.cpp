/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

// This will be removed when new sim is added in
#include <jni.h>

static JavaVM* jvm = nullptr;

namespace sim {
jint SimOnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  return JNI_VERSION_1_6;
}

void SimOnUnload(JavaVM * vm, void* reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;
  jvm = nullptr;
}
}  // namespace sim
