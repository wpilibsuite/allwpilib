// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AlertDataJNI.hpp"

#include <jni.h>

#include "org_wpilib_hardware_hal_simulation_AlertDataJNI.h"
#include "wpi/hal/simulation/AlertData.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

static JClass alertInfoCls;

static jobject MakeAlertInfoJava(JNIEnv* env, const HALSIM_AlertInfo& info) {
  static jmethodID func = env->GetMethodID(
      alertInfoCls, "<init>", "(ILjava/lang/String;Ljava/lang/String;JI)V");
  return env->NewObject(
      alertInfoCls, func, static_cast<jint>(info.handle),
      MakeJString(env, wpi::util::to_string_view(&info.group)),
      MakeJString(env, wpi::util::to_string_view(&info.text)),
      static_cast<jlong>(info.activeStartTime), static_cast<jint>(info.level));
}
namespace wpi::hal::sim {

bool InitializeAlertDataJNI(JNIEnv* env) {
  alertInfoCls =
      JClass(env, "org/wpilib/hardware/hal/simulation/AlertDataJNI$AlertInfo");
  if (!alertInfoCls) {
    return false;
  }
  return true;
}

void FreeAlertDataJNI(JNIEnv* env) {
  alertInfoCls.free(env);
}

}  // namespace wpi::hal::sim

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_simulation_AlertDataJNI
 * Method:    getNumAlerts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_AlertDataJNI_getNumAlerts
  (JNIEnv*, jclass)
{
  return HALSIM_GetNumAlerts();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_AlertDataJNI
 * Method:    getAlerts
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_hardware_hal_simulation_AlertDataJNI_getAlerts
  (JNIEnv* env, jclass)
{
  int32_t allocLen = HALSIM_GetNumAlerts();
  HALSIM_AlertInfo* arr = new HALSIM_AlertInfo[allocLen];
  int32_t len = HALSIM_GetAlerts(arr, allocLen);

  jobjectArray ret = env->NewObjectArray(len, alertInfoCls, nullptr);
  for (int32_t i = 0; i < len; ++i) {
    env->SetObjectArrayElement(ret, i, MakeAlertInfoJava(env, arr[i]));
  }
  HALSIM_FreeAlerts(arr, len < allocLen ? len : allocLen);
  delete[] arr;
  return ret;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_AlertDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_AlertDataJNI_resetData
  (JNIEnv*, jclass)
{
  HALSIM_ResetAlertData();
}

}  // extern "C"
