// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_PowerDistributionDataJNI.h"
#include "hal/simulation/PowerDistributionData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterPowerDistributionInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPowerDistributionInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPowerDistributionInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPowerDistributionInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    registerTemperatureCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_registerTemperatureCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterPowerDistributionTemperatureCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    cancelTemperatureCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_cancelTemperatureCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPowerDistributionTemperatureCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    getTemperature
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_getTemperature
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPowerDistributionTemperature(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    setTemperature
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_setTemperature
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPowerDistributionTemperature(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    registerVoltageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_registerVoltageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterPowerDistributionVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    cancelVoltageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_cancelVoltageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPowerDistributionVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    getVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_getVoltage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPowerDistributionVoltage(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    setVoltage
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_setVoltage
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPowerDistributionVoltage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    registerCurrentCallback
 * Signature: (IILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_registerCurrentCallback
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  return sim::AllocateChannelCallback(
      env, index, channel, callback, initialNotify,
      &HALSIM_RegisterPowerDistributionCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    cancelCurrentCallback
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_cancelCurrentCallback
  (JNIEnv* env, jclass, jint index, jint channel, jint handle)
{
  return sim::FreeChannelCallback(
      env, handle, index, channel,
      &HALSIM_CancelPowerDistributionCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    getCurrent
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_getCurrent
  (JNIEnv*, jclass, jint index, jint channel)
{
  return HALSIM_GetPowerDistributionCurrent(index, channel);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    setCurrent
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_setCurrent
  (JNIEnv*, jclass, jint index, jint channel, jdouble value)
{
  HALSIM_SetPowerDistributionCurrent(index, channel, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PowerDistributionDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PowerDistributionDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetPowerDistributionData(index);
}

}  // extern "C"
