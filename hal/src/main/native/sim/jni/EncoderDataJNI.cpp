/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_EncoderDataJNI.h"
#include "mockdata/EncoderData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetEncoderInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerCountCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerCountCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderCountCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelCountCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelCountCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderCountCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getCount
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderCount(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setCount
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setCount
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetEncoderCount(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerPeriodCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerPeriodCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderPeriodCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelPeriodCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelPeriodCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderPeriodCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getPeriod
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getPeriod
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderPeriod(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setPeriod
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setPeriod
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetEncoderPeriod(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerResetCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerResetCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderResetCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelResetCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelResetCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderResetCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getReset
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getReset
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderReset(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setReset
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setReset
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetEncoderReset(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerMaxPeriodCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerMaxPeriodCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderMaxPeriodCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelMaxPeriodCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelMaxPeriodCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderMaxPeriodCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getMaxPeriod
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getMaxPeriod
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderMaxPeriod(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setMaxPeriod
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setMaxPeriod
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetEncoderMaxPeriod(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerDirectionCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerDirectionCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderDirectionCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelDirectionCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelDirectionCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderDirectionCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getDirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getDirection
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderDirection(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setDirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setDirection
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetEncoderDirection(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerReverseDirectionCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerReverseDirectionCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderReverseDirectionCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelReverseDirectionCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelReverseDirectionCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderReverseDirectionCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getReverseDirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getReverseDirection
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderReverseDirection(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setReverseDirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setReverseDirection
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetEncoderReverseDirection(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    registerSamplesToAverageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_registerSamplesToAverageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterEncoderSamplesToAverageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    cancelSamplesToAverageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_cancelSamplesToAverageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelEncoderSamplesToAverageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    getSamplesToAverage
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_getSamplesToAverage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetEncoderSamplesToAverage(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    setSamplesToAverage
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_setSamplesToAverage
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetEncoderSamplesToAverage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_EncoderDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_EncoderDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetEncoderData(index);
}

}  // extern "C"
