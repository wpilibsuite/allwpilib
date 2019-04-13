/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SimulatorJNI.h"

#include "BufferCallbackStore.h"
#include "CallbackStore.h"
#include "ConstBufferCallbackStore.h"
#include "SpiReadAutoReceiveBufferCallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_SimulatorJNI.h"
#include "hal/HAL.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/MockHooks.h"

using namespace wpi::java;

static JavaVM* jvm = nullptr;
static JClass simValueCls;
static JClass notifyCallbackCls;
static JClass bufferCallbackCls;
static JClass constBufferCallbackCls;
static JClass spiReadAutoReceiveBufferCallbackCls;
static jmethodID notifyCallbackCallback;
static jmethodID bufferCallbackCallback;
static jmethodID constBufferCallbackCallback;
static jmethodID spiReadAutoReceiveBufferCallbackCallback;

namespace sim {
jint SimOnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  simValueCls = JClass(env, "edu/wpi/first/hal/sim/SimValue");
  if (!simValueCls) return JNI_ERR;

  notifyCallbackCls = JClass(env, "edu/wpi/first/hal/sim/NotifyCallback");
  if (!notifyCallbackCls) return JNI_ERR;

  notifyCallbackCallback = env->GetMethodID(notifyCallbackCls, "callbackNative",
                                            "(Ljava/lang/String;IJD)V");
  if (!notifyCallbackCallback) return JNI_ERR;

  bufferCallbackCls = JClass(env, "edu/wpi/first/hal/sim/BufferCallback");
  if (!bufferCallbackCls) return JNI_ERR;

  bufferCallbackCallback = env->GetMethodID(bufferCallbackCls, "callback",
                                            "(Ljava/lang/String;[BI)V");
  if (!bufferCallbackCallback) return JNI_ERR;

  constBufferCallbackCls =
      JClass(env, "edu/wpi/first/hal/sim/ConstBufferCallback");
  if (!constBufferCallbackCls) return JNI_ERR;

  constBufferCallbackCallback = env->GetMethodID(
      constBufferCallbackCls, "callback", "(Ljava/lang/String;[BI)V");
  if (!constBufferCallbackCallback) return JNI_ERR;

  spiReadAutoReceiveBufferCallbackCls =
      JClass(env, "edu/wpi/first/hal/sim/SpiReadAutoReceiveBufferCallback");
  if (!spiReadAutoReceiveBufferCallbackCls) return JNI_ERR;

  spiReadAutoReceiveBufferCallbackCallback =
      env->GetMethodID(spiReadAutoReceiveBufferCallbackCls, "callback",
                       "(Ljava/lang/String;[II)I");
  if (!spiReadAutoReceiveBufferCallbackCallback) return JNI_ERR;

  InitializeStore();
  InitializeBufferStore();
  InitializeConstBufferStore();
  InitializeSpiBufferStore();

  return JNI_VERSION_1_6;
}

void SimOnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;

  simValueCls.free(env);
  notifyCallbackCls.free(env);
  bufferCallbackCls.free(env);
  constBufferCallbackCls.free(env);
  spiReadAutoReceiveBufferCallbackCls.free(env);
  jvm = nullptr;
}

JavaVM* GetJVM() { return jvm; }

jmethodID GetNotifyCallback() { return notifyCallbackCallback; }

jmethodID GetBufferCallback() { return bufferCallbackCallback; }

jmethodID GetConstBufferCallback() { return constBufferCallbackCallback; }

jmethodID GetSpiReadAutoReceiveBufferCallback() {
  return spiReadAutoReceiveBufferCallbackCallback;
}
}  // namespace sim

extern "C" {
/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SimulatorJNI
 * Method:    waitForProgramStart
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SimulatorJNI_waitForProgramStart
  (JNIEnv*, jclass)
{
  HALSIM_WaitForProgramStart();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SimulatorJNI
 * Method:    setProgramStarted
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SimulatorJNI_setProgramStarted
  (JNIEnv*, jclass)
{
  HALSIM_SetProgramStarted();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SimulatorJNI
 * Method:    restartTiming
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SimulatorJNI_restartTiming
  (JNIEnv*, jclass)
{
  HALSIM_RestartTiming();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SimulatorJNI
 * Method:    resetHandles
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SimulatorJNI_resetHandles
  (JNIEnv*, jclass)
{
  hal::HandleBase::ResetGlobalHandles();
}
}  // extern "C"
