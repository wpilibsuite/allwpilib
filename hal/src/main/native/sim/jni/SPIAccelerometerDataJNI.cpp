/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI.h"
#include "mockdata/SPIAccelerometerData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    registerActiveCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerActiveCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerActiveCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    cancelActiveCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelActiveCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerActiveCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    getActive
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getActive
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetSPIAccelerometerActive(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    setActive
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setActive
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetSPIAccelerometerActive(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    registerRangeCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerRangeCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerRangeCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    cancelRangeCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelRangeCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerRangeCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    getRange
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getRange
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetSPIAccelerometerRange(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    setRange
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setRange
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetSPIAccelerometerRange(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    registerXCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerXCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerXCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    cancelXCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelXCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerXCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    getX
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getX
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetSPIAccelerometerX(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    setX
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setX
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetSPIAccelerometerX(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    registerYCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerYCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerYCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    cancelYCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelYCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerYCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    getY
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getY
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetSPIAccelerometerY(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    setY
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setY
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetSPIAccelerometerY(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    registerZCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerZCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerZCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    cancelZCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelZCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerZCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    getZ
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getZ
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetSPIAccelerometerZ(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    setZ
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setZ
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetSPIAccelerometerZ(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetSPIAccelerometerData(index);
}

}  // extern "C"
