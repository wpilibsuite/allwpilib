// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_REVPHDataJNI.h"
#include "hal/simulation/REVPHData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterREVPHInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelREVPHInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetREVPHInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetREVPHInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerSolenoidOutputCallback
 * Signature: (IILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerSolenoidOutputCallback
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  return sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      &HALSIM_RegisterREVPHSolenoidOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    cancelSolenoidOutputCallback
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_cancelSolenoidOutputCallback
  (JNIEnv* env, jclass, jint index, jint channel, jint handle)
{
  return sim::FreeChannelCallback(env, handle, index, channel,
                                  &HALSIM_CancelREVPHSolenoidOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    getSolenoidOutput
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_getSolenoidOutput
  (JNIEnv*, jclass, jint index, jint channel)
{
  return HALSIM_GetREVPHSolenoidOutput(index, channel);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    setSolenoidOutput
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_setSolenoidOutput
  (JNIEnv*, jclass, jint index, jint channel, jboolean value)
{
  HALSIM_SetREVPHSolenoidOutput(index, channel, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerCompressorOnCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerCompressorOnCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterREVPHCompressorOnCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    cancelCompressorOnCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_cancelCompressorOnCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelREVPHCompressorOnCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    getCompressorOn
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_getCompressorOn
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetREVPHCompressorOn(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    setCompressorOn
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_setCompressorOn
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetREVPHCompressorOn(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerCompressorConfigTypeCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerCompressorConfigTypeCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterREVPHCompressorConfigTypeCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    cancelCompressorConfigTypeCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_cancelCompressorConfigTypeCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelREVPHCompressorConfigTypeCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    getCompressorConfigType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_getCompressorConfigType
  (JNIEnv*, jclass, jint index)
{
  return static_cast<jint>(HALSIM_GetREVPHCompressorConfigType(index));
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    setCompressorConfigType
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_setCompressorConfigType
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetREVPHCompressorConfigType(
      index, static_cast<HAL_REVPHCompressorConfigType>(value));
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerPressureSwitchCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerPressureSwitchCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterREVPHPressureSwitchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    cancelPressureSwitchCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_cancelPressureSwitchCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelREVPHPressureSwitchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    getPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_getPressureSwitch
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetREVPHPressureSwitch(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    setPressureSwitch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_setPressureSwitch
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetREVPHPressureSwitch(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerCompressorCurrentCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerCompressorCurrentCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterREVPHCompressorCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    cancelCompressorCurrentCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_cancelCompressorCurrentCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelREVPHCompressorCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_getCompressorCurrent
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetREVPHCompressorCurrent(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    setCompressorCurrent
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_setCompressorCurrent
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetREVPHCompressorCurrent(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerAllNonSolenoidCallbacks
 * Signature: (ILjava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerAllNonSolenoidCallbacks
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  sim::AllocateCallback(
      env, index, callback, initialNotify,
      [](int32_t index, HAL_NotifyCallback cb, void* param, HAL_Bool in) {
        HALSIM_RegisterREVPHAllNonSolenoidCallbacks(index, cb, param, in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    registerAllSolenoidCallbacks
 * Signature: (IILjava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_registerAllSolenoidCallbacks
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      [](int32_t index, int32_t channel, HAL_NotifyCallback cb, void* param,
         HAL_Bool in) {
        HALSIM_RegisterREVPHAllSolenoidCallbacks(index, channel, cb, param, in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_REVPHDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_REVPHDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetREVPHData(index);
}

}  // extern "C"
