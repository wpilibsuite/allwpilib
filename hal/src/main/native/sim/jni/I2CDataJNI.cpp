/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "BufferCallbackStore.h"
#include "CallbackStore.h"
#include "ConstBufferCallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_I2CDataJNI.h"
#include "mockdata/I2CData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterI2CInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelI2CInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetI2CInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetI2CInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    registerReadCallback
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_registerReadCallback
  (JNIEnv* env, jclass, jint index, jobject callback)
{
  return sim::AllocateBufferCallback(env, index, callback,
                                     &HALSIM_RegisterI2CReadCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    cancelReadCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_cancelReadCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  sim::FreeBufferCallback(env, handle, index, &HALSIM_CancelI2CReadCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    registerWriteCallback
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_registerWriteCallback
  (JNIEnv* env, jclass, jint index, jobject callback)
{
  return sim::AllocateConstBufferCallback(env, index, callback,
                                          &HALSIM_RegisterI2CWriteCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    cancelWriteCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_cancelWriteCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  sim::FreeConstBufferCallback(env, handle, index,
                               &HALSIM_CancelI2CWriteCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_I2CDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_I2CDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetI2CData(index);
}

}  // extern "C"
