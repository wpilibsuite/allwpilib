/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_PCMDataJNI.h"
#include "hal/simulation/PCMData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getSolenoidDisplayName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getSolenoidDisplayName
  (JNIEnv* env, jclass, jint index, jint channel)
{
  const char* displayName = HALSIM_GetSolenoidDisplayName(index, channel);
  return wpi::java::MakeJString(env, displayName);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setSolenoidDisplayName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setSolenoidDisplayName
  (JNIEnv* env, jclass, jint index, jint channel, jstring displayName)
{
  wpi::java::JStringRef displayNameRef{env, displayName};
  HALSIM_SetSolenoidDisplayName(index, channel, displayNameRef.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerSolenoidInitializedCallback
 * Signature: (IILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerSolenoidInitializedCallback
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  return sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      &HALSIM_RegisterPCMSolenoidInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelSolenoidInitializedCallback
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelSolenoidInitializedCallback
  (JNIEnv* env, jclass, jint index, jint channel, jint handle)
{
  return sim::FreeChannelCallback(env, handle, index, channel,
                                  &HALSIM_CancelPCMSolenoidInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getSolenoidInitialized
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getSolenoidInitialized
  (JNIEnv*, jclass, jint index, jint channel)
{
  return HALSIM_GetPCMSolenoidInitialized(index, channel);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setSolenoidInitialized
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setSolenoidInitialized
  (JNIEnv*, jclass, jint index, jint channel, jboolean value)
{
  HALSIM_SetPCMSolenoidInitialized(index, channel, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerSolenoidOutputCallback
 * Signature: (IILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerSolenoidOutputCallback
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  return sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      &HALSIM_RegisterPCMSolenoidOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelSolenoidOutputCallback
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelSolenoidOutputCallback
  (JNIEnv* env, jclass, jint index, jint channel, jint handle)
{
  return sim::FreeChannelCallback(env, handle, index, channel,
                                  &HALSIM_CancelPCMSolenoidOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getSolenoidOutput
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getSolenoidOutput
  (JNIEnv*, jclass, jint index, jint channel)
{
  return HALSIM_GetPCMSolenoidOutput(index, channel);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setSolenoidOutput
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setSolenoidOutput
  (JNIEnv*, jclass, jint index, jint channel, jboolean value)
{
  HALSIM_SetPCMSolenoidOutput(index, channel, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerCompressorInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerCompressorInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterPCMCompressorInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelCompressorInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelCompressorInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPCMCompressorInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getCompressorInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getCompressorInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPCMCompressorInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setCompressorInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setCompressorInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPCMCompressorInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerCompressorOnCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerCompressorOnCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPCMCompressorOnCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelCompressorOnCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelCompressorOnCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPCMCompressorOnCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getCompressorOn
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getCompressorOn
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPCMCompressorOn(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setCompressorOn
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setCompressorOn
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPCMCompressorOn(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerClosedLoopEnabledCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerClosedLoopEnabledCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPCMClosedLoopEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelClosedLoopEnabledCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelClosedLoopEnabledCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPCMClosedLoopEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getClosedLoopEnabled
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getClosedLoopEnabled
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPCMClosedLoopEnabled(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setClosedLoopEnabled
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setClosedLoopEnabled
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPCMClosedLoopEnabled(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerPressureSwitchCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerPressureSwitchCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPCMPressureSwitchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelPressureSwitchCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelPressureSwitchCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPCMPressureSwitchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getPressureSwitch
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPCMPressureSwitch(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setPressureSwitch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setPressureSwitch
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPCMPressureSwitch(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerCompressorCurrentCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerCompressorCurrentCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPCMCompressorCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    cancelCompressorCurrentCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_cancelCompressorCurrentCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPCMCompressorCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_getCompressorCurrent
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPCMCompressorCurrent(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    setCompressorCurrent
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_setCompressorCurrent
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPCMCompressorCurrent(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerAllNonSolenoidCallbacks
 * Signature: (ILjava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerAllNonSolenoidCallbacks
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  sim::AllocateCallback(
      env, index, callback, initialNotify,
      [](int32_t index, HAL_NotifyCallback cb, void* param, HAL_Bool in) {
        HALSIM_RegisterPCMAllNonSolenoidCallbacks(index, cb, param, in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    registerAllSolenoidCallbacks
 * Signature: (IILjava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_registerAllSolenoidCallbacks
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      [](int32_t index, int32_t channel, HAL_NotifyCallback cb, void* param,
         HAL_Bool in) {
        HALSIM_RegisterPCMAllSolenoidCallbacks(index, channel, cb, param, in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_PCMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PCMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetPCMData(index);
}

}  // extern "C"
