/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "ConstBufferCallbackStore.h"
#include "edu_wpi_first_hal_simulation_AddressableLEDDataJNI.h"
#include "hal/simulation/AddressableLEDData.h"

static_assert(sizeof(jbyte) * 4 == sizeof(HAL_AddressableLEDData));

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAddressableLEDInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAddressableLEDInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAddressableLEDInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAddressableLEDInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getDisplayName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getDisplayName
  (JNIEnv* env, jclass, jint index)
{
  const char* displayName = HALSIM_GetAddressableLEDDisplayName(index);
  return wpi::java::MakeJString(env, displayName);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setDisplayName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setDisplayName
  (JNIEnv* env, jclass, jint index, jstring displayName)
{
  wpi::java::JStringRef displayNameRef{env, displayName};
  HALSIM_SetAddressableLEDDisplayName(index, displayNameRef.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    registerOutputPortCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerOutputPortCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAddressableLEDOutputPortCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelOutputPortCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelOutputPortCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAddressableLEDOutputPortCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getOutputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getOutputPort
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAddressableLEDOutputPort(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setOutputPort
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setOutputPort
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAddressableLEDOutputPort(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    registerLengthCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerLengthCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAddressableLEDLengthCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelLengthCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelLengthCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAddressableLEDLengthCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getLength
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getLength
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAddressableLEDLength(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setLength
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setLength
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAddressableLEDLength(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    registerRunningCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerRunningCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAddressableLEDRunningCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelRunningCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelRunningCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAddressableLEDRunningCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getRunning
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getRunning
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAddressableLEDRunning(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setRunning
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setRunning
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAddressableLEDRunning(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    registerDataCallback
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerDataCallback
  (JNIEnv* env, jclass, jint index, jobject callback)
{
  return sim::AllocateConstBufferCallback(
      env, index, callback, &HALSIM_RegisterAddressableLEDDataCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelDataCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelDataCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  sim::FreeConstBufferCallback(env, handle, index,
                               &HALSIM_CancelAddressableLEDDataCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getData
 * Signature: (I)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getData
  (JNIEnv* env, jclass, jint index)
{
  auto data =
      std::make_unique<HAL_AddressableLEDData[]>(HAL_kAddressableLEDMaxLength);
  int32_t length = HALSIM_GetAddressableLEDData(index, data.get());
  return MakeJByteArray(
      env, wpi::ArrayRef(reinterpret_cast<jbyte*>(data.get()), length * 4));
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setData
 * Signature: (I[B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setData
  (JNIEnv* env, jclass, jint index, jbyteArray arr)
{
  JByteArrayRef jArrRef{env, arr};
  auto arrRef = jArrRef.array();
  HALSIM_SetAddressableLEDData(
      index, reinterpret_cast<const HAL_AddressableLEDData*>(arrRef.data()),
      arrRef.size() / 4);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAddressableLEDData(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    findForChannel
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_findForChannel
  (JNIEnv*, jclass, jint channel)
{
  return HALSIM_FindAddressableLEDForChannel(channel);
}

}  // extern "C"
