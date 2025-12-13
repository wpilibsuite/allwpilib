// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SimulatorJNI.h"

#include "BufferCallbackStore.h"
#include "CallbackStore.h"
#include "ConstBufferCallbackStore.h"
#include "OpModeOptionsCallbackStore.h"
#include "SimDeviceDataJNI.h"
#include "org_wpilib_hardware_hal_simulation_SimulatorJNI.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

static JavaVM* jvm = nullptr;
static JClass notifyCallbackCls;
static JClass bufferCallbackCls;
static JClass constBufferCallbackCls;
static JClass biConsumerCls;
static jmethodID notifyCallbackCallback;
static jmethodID bufferCallbackCallback;
static jmethodID constBufferCallbackCallback;
static jmethodID biConsumerCallback;

static const JClassInit classes[] = {
    {"org/wpilib/hardware/hal/simulation/NotifyCallback", &notifyCallbackCls},
    {"org/wpilib/hardware/hal/simulation/BufferCallback", &bufferCallbackCls},
    {"org/wpilib/hardware/hal/simulation/ConstBufferCallback",
     &constBufferCallbackCls},
    {"java/util/function/BiConsumer", &biConsumerCls},
};

static const struct JMethodInit {
  JClass* cls;
  const char* name;
  const char* sig;
  jmethodID* method;
} methods[] = {
    {&notifyCallbackCls, "callbackNative", "(Ljava/lang/String;IJD)V",
     &notifyCallbackCallback},
    {&bufferCallbackCls, "callback", "(Ljava/lang/String;[BI)V",
     &bufferCallbackCallback},
    {&constBufferCallbackCls, "callback", "(Ljava/lang/String;[BI)V",
     &constBufferCallbackCallback},
    {&biConsumerCls, "accept", "(Ljava/lang/Object;Ljava/lang/Object;)V",
     &biConsumerCallback},
};

namespace wpi::hal::sim {
jint SimOnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  for (auto& m : methods) {
    *m.method = env->GetMethodID(*m.cls, m.name, m.sig);
    if (!*m.method) {
      return JNI_ERR;
    }
  }

  InitializeStore();
  InitializeBufferStore();
  InitializeConstBufferStore();
  InitializeOpModeOptionsStore();
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
  biConsumerCls.free(env);
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

jmethodID GetBiConsumerCallback() {
  return biConsumerCallback;
}
}  // namespace wpi::hal::sim

extern "C" {
/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    setRuntimeType
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_setRuntimeType
  (JNIEnv*, jclass, jint type)
{
  HALSIM_SetRuntimeType(static_cast<HAL_RuntimeType>(type));
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    waitForProgramStart
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_waitForProgramStart
  (JNIEnv*, jclass)
{
  HALSIM_WaitForProgramStart();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    setProgramStarted
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_setProgramStarted
  (JNIEnv*, jclass, jboolean started)
{
  HALSIM_SetProgramStarted(started);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    getProgramStarted
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_getProgramStarted
  (JNIEnv*, jclass)
{
  return HALSIM_GetProgramStarted();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    setProgramState
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_setProgramState
  (JNIEnv*, jclass, jlong word)
{
  HALSIM_SetProgramState({word});
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    nativeGetProgramState
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_nativeGetProgramState
  (JNIEnv*, jclass)
{
  HAL_ControlWord word;
  HALSIM_GetProgramState(&word);
  return word.value;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    restartTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_restartTiming
  (JNIEnv*, jclass)
{
  HALSIM_RestartTiming();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    pauseTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_pauseTiming
  (JNIEnv*, jclass)
{
  HALSIM_PauseTiming();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    resumeTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_resumeTiming
  (JNIEnv*, jclass)
{
  HALSIM_ResumeTiming();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    isTimingPaused
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_isTimingPaused
  (JNIEnv*, jclass)
{
  return HALSIM_IsTimingPaused();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    stepTiming
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_stepTiming
  (JNIEnv*, jclass, jlong delta)
{
  HALSIM_StepTiming(delta);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    stepTimingAsync
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_stepTimingAsync
  (JNIEnv*, jclass, jlong delta)
{
  HALSIM_StepTimingAsync(delta);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_SimulatorJNI
 * Method:    resetHandles
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_SimulatorJNI_resetHandles
  (JNIEnv*, jclass)
{
  wpi::hal::HandleBase::ResetGlobalHandles();
}
}  // extern "C"
