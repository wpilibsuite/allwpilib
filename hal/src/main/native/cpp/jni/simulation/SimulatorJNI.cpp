// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SimulatorJNI.h"

#include <wpi/jni_util.h>

#include "BufferCallbackStore.h"
#include "CallbackStore.h"
#include "ConstBufferCallbackStore.h"
#include "SimDeviceDataJNI.h"
#include "SpiReadAutoReceiveBufferCallbackStore.h"
#include "edu_wpi_first_hal_simulation_SimulatorJNI.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/MockHooks.h"

using namespace wpi::java;

static JavaVM* jvm = nullptr;
static JClass notifyCallbackCls;
static JClass bufferCallbackCls;
static JClass constBufferCallbackCls;
static JClass spiReadAutoReceiveBufferCallbackCls;
static jmethodID notifyCallbackCallback;
static jmethodID bufferCallbackCallback;
static jmethodID constBufferCallbackCallback;
static jmethodID spiReadAutoReceiveBufferCallbackCallback;

namespace hal::sim {
jint SimOnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  notifyCallbackCls =
      JClass(env, "edu/wpi/first/hal/simulation/NotifyCallback");
  if (!notifyCallbackCls) {
    return JNI_ERR;
  }

  notifyCallbackCallback = env->GetMethodID(notifyCallbackCls, "callbackNative",
                                            "(Ljava/lang/String;IJD)V");
  if (!notifyCallbackCallback) {
    return JNI_ERR;
  }

  bufferCallbackCls =
      JClass(env, "edu/wpi/first/hal/simulation/BufferCallback");
  if (!bufferCallbackCls) {
    return JNI_ERR;
  }

  bufferCallbackCallback = env->GetMethodID(bufferCallbackCls, "callback",
                                            "(Ljava/lang/String;[BI)V");
  if (!bufferCallbackCallback) {
    return JNI_ERR;
  }

  constBufferCallbackCls =
      JClass(env, "edu/wpi/first/hal/simulation/ConstBufferCallback");
  if (!constBufferCallbackCls) {
    return JNI_ERR;
  }

  constBufferCallbackCallback = env->GetMethodID(
      constBufferCallbackCls, "callback", "(Ljava/lang/String;[BI)V");
  if (!constBufferCallbackCallback) {
    return JNI_ERR;
  }

  spiReadAutoReceiveBufferCallbackCls = JClass(
      env, "edu/wpi/first/hal/simulation/SpiReadAutoReceiveBufferCallback");
  if (!spiReadAutoReceiveBufferCallbackCls) {
    return JNI_ERR;
  }

  spiReadAutoReceiveBufferCallbackCallback =
      env->GetMethodID(spiReadAutoReceiveBufferCallbackCls, "callback",
                       "(Ljava/lang/String;[II)I");
  if (!spiReadAutoReceiveBufferCallbackCallback) {
    return JNI_ERR;
  }

  InitializeStore();
  InitializeBufferStore();
  InitializeConstBufferStore();
  InitializeSpiBufferStore();
  if (!InitializeSimDeviceDataJNI(env)) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

void SimOnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }

  notifyCallbackCls.free(env);
  bufferCallbackCls.free(env);
  constBufferCallbackCls.free(env);
  spiReadAutoReceiveBufferCallbackCls.free(env);
  FreeSimDeviceDataJNI(env);
  jvm = nullptr;
}

JavaVM* GetJVM() {
  return jvm;
}

jmethodID GetNotifyCallback() {
  return notifyCallbackCallback;
}

jmethodID GetBufferCallback() {
  return bufferCallbackCallback;
}

jmethodID GetConstBufferCallback() {
  return constBufferCallbackCallback;
}

jmethodID GetSpiReadAutoReceiveBufferCallback() {
  return spiReadAutoReceiveBufferCallbackCallback;
}
}  // namespace hal::sim

extern "C" {
/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    setRuntimeType
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_setRuntimeType
  (JNIEnv*, jclass, jint type)
{
  HALSIM_SetRuntimeType(static_cast<HAL_RuntimeType>(type));
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    waitForProgramStart
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_waitForProgramStart
  (JNIEnv*, jclass)
{
  HALSIM_WaitForProgramStart();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    setProgramStarted
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_setProgramStarted
  (JNIEnv*, jclass)
{
  HALSIM_SetProgramStarted();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    getProgramStarted
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_getProgramStarted
  (JNIEnv*, jclass)
{
  return HALSIM_GetProgramStarted();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    restartTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_restartTiming
  (JNIEnv*, jclass)
{
  HALSIM_RestartTiming();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    pauseTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_pauseTiming
  (JNIEnv*, jclass)
{
  HALSIM_PauseTiming();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    resumeTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_resumeTiming
  (JNIEnv*, jclass)
{
  HALSIM_ResumeTiming();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    isTimingPaused
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_isTimingPaused
  (JNIEnv*, jclass)
{
  return HALSIM_IsTimingPaused();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    stepTiming
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_stepTiming
  (JNIEnv*, jclass, jlong delta)
{
  HALSIM_StepTiming(delta);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    stepTimingAsync
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_stepTimingAsync
  (JNIEnv*, jclass, jlong delta)
{
  HALSIM_StepTimingAsync(delta);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimulatorJNI
 * Method:    resetHandles
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimulatorJNI_resetHandles
  (JNIEnv*, jclass)
{
  hal::HandleBase::ResetGlobalHandles();
}
}  // extern "C"
