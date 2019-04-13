/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI.h"
#include "mockdata/AccelerometerData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    registerActiveCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerActiveCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerActiveCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    cancelActiveCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelActiveCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerActiveCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    getActive
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getActive
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAccelerometerActive(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    setActive
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setActive
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAccelerometerActive(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    registerRangeCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerRangeCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerRangeCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    cancelRangeCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelRangeCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerRangeCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    getRange
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getRange
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAccelerometerRange(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    setRange
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setRange
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAccelerometerRange(index,
                               static_cast<HAL_AccelerometerRange>(value));
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    registerXCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerXCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerXCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    cancelXCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelXCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerXCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    getX
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getX
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAccelerometerX(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    setX
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setX
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAccelerometerX(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    registerYCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerYCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerYCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    cancelYCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelYCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerYCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    getY
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getY
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAccelerometerY(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    setY
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setY
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAccelerometerY(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    registerZCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerZCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerZCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    cancelZCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelZCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerZCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    getZ
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getZ
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAccelerometerZ(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    setZ
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setZ
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAccelerometerZ(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAccelerometerData(index);
}

}  // extern "C"
