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
#include "SpiReadAutoReceiveBufferCallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_SPIDataJNI.h"
#include "mockdata/SPIData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetSPIInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetSPIInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    registerReadCallback
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_registerReadCallback
  (JNIEnv* env, jclass, jint index, jobject callback)
{
  return sim::AllocateBufferCallback(env, index, callback,
                                     &HALSIM_RegisterSPIReadCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    cancelReadCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_cancelReadCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  sim::FreeBufferCallback(env, handle, index, &HALSIM_CancelSPIReadCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    registerWriteCallback
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_registerWriteCallback
  (JNIEnv* env, jclass, jint index, jobject callback)
{
  return sim::AllocateConstBufferCallback(env, index, callback,
                                          &HALSIM_RegisterSPIWriteCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    cancelWriteCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_cancelWriteCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  sim::FreeConstBufferCallback(env, handle, index,
                               &HALSIM_CancelSPIWriteCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    registerReadAutoReceiveBufferCallback
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_registerReadAutoReceiveBufferCallback
  (JNIEnv* env, jclass, jint index, jobject callback)
{
  return sim::AllocateSpiBufferCallback(
      env, index, callback, &HALSIM_RegisterSPIReadAutoReceivedDataCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    cancelReadAutoReceiveBufferCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_cancelReadAutoReceiveBufferCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  sim::FreeSpiBufferCallback(env, handle, index,
                             &HALSIM_CancelSPIReadAutoReceivedDataCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_SPIDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetSPIData(index);
}

}  // extern "C"
