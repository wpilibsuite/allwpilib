// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_CTREPCMDataJNI.h"
#include "hal/simulation/CTREPCMData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterCTREPCMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelCTREPCMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetCTREPCMInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetCTREPCMInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerSolenoidOutputCallback
 * Signature: (IILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerSolenoidOutputCallback
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  return sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      &HALSIM_RegisterCTREPCMSolenoidOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    cancelSolenoidOutputCallback
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_cancelSolenoidOutputCallback
  (JNIEnv* env, jclass, jint index, jint channel, jint handle)
{
  return sim::FreeChannelCallback(env, handle, index, channel,
                                  &HALSIM_CancelCTREPCMSolenoidOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    getSolenoidOutput
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_getSolenoidOutput
  (JNIEnv*, jclass, jint index, jint channel)
{
  return HALSIM_GetCTREPCMSolenoidOutput(index, channel);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    setSolenoidOutput
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_setSolenoidOutput
  (JNIEnv*, jclass, jint index, jint channel, jboolean value)
{
  HALSIM_SetCTREPCMSolenoidOutput(index, channel, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerCompressorOnCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerCompressorOnCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterCTREPCMCompressorOnCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    cancelCompressorOnCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_cancelCompressorOnCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelCTREPCMCompressorOnCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    getCompressorOn
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_getCompressorOn
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetCTREPCMCompressorOn(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    setCompressorOn
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_setCompressorOn
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetCTREPCMCompressorOn(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerClosedLoopEnabledCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerClosedLoopEnabledCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterCTREPCMClosedLoopEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    cancelClosedLoopEnabledCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_cancelClosedLoopEnabledCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelCTREPCMClosedLoopEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    getClosedLoopEnabled
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_getClosedLoopEnabled
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetCTREPCMClosedLoopEnabled(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    setClosedLoopEnabled
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_setClosedLoopEnabled
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetCTREPCMClosedLoopEnabled(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerPressureSwitchCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerPressureSwitchCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterCTREPCMPressureSwitchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    cancelPressureSwitchCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_cancelPressureSwitchCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelCTREPCMPressureSwitchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    getPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_getPressureSwitch
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetCTREPCMPressureSwitch(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    setPressureSwitch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_setPressureSwitch
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetCTREPCMPressureSwitch(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerCompressorCurrentCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerCompressorCurrentCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterCTREPCMCompressorCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    cancelCompressorCurrentCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_cancelCompressorCurrentCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelCTREPCMCompressorCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_getCompressorCurrent
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetCTREPCMCompressorCurrent(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    setCompressorCurrent
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_setCompressorCurrent
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetCTREPCMCompressorCurrent(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerAllNonSolenoidCallbacks
 * Signature: (ILjava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerAllNonSolenoidCallbacks
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  sim::AllocateCallback(
      env, index, callback, initialNotify,
      [](int32_t index, HAL_NotifyCallback cb, void* param, HAL_Bool in) {
        HALSIM_RegisterCTREPCMAllNonSolenoidCallbacks(index, cb, param, in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    registerAllSolenoidCallbacks
 * Signature: (IILjava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_registerAllSolenoidCallbacks
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      [](int32_t index, int32_t channel, HAL_NotifyCallback cb, void* param,
         HAL_Bool in) {
        HALSIM_RegisterCTREPCMAllSolenoidCallbacks(index, channel, cb, param,
                                                   in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_CTREPCMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_CTREPCMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetCTREPCMData(index);
}

}  // extern "C"
