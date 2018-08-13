/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_PDPDataJNI.h"
#include "mockdata/PDPData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPDPInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPDPInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPDPInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPDPInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    registerTemperatureCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerTemperatureCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPDPTemperatureCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    cancelTemperatureCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelTemperatureCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPDPTemperatureCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    getTemperature
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getTemperature
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPDPTemperature(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    setTemperature
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setTemperature
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPDPTemperature(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    registerVoltageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerVoltageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPDPVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    cancelVoltageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelVoltageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPDPVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    getVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getVoltage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPDPVoltage(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    setVoltage
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setVoltage
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPDPVoltage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    registerCurrentCallback
 * Signature: (IILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerCurrentCallback
  (JNIEnv* env, jclass, jint index, jint channel, jobject callback,
   jboolean initialNotify)
{
  return sim::AllocateChannelCallback(env, index, channel, callback,
                                      initialNotify,
                                      &HALSIM_RegisterPDPCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    cancelCurrentCallback
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelCurrentCallback
  (JNIEnv* env, jclass, jint index, jint channel, jint handle)
{
  return sim::FreeChannelCallback(env, handle, index, channel,
                                  &HALSIM_CancelPDPCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    getCurrent
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getCurrent
  (JNIEnv*, jclass, jint index, jint channel)
{
  return HALSIM_GetPDPCurrent(index, channel);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    setCurrent
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setCurrent
  (JNIEnv*, jclass, jint index, jint channel, jdouble value)
{
  HALSIM_SetPDPCurrent(index, channel, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PDPDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetPDPData(index);
}

}  // extern "C"
