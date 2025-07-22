// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <memory>

#include "CallbackStore.h"
#include "ConstBufferCallbackStore.h"
#include "edu_wpi_first_hal_simulation_AddressableLEDDataJNI.h"
#include "hal/simulation/AddressableLEDData.h"

static_assert(sizeof(jbyte) * 3 == sizeof(HAL_AddressableLEDData));

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
 * Method:    registerStartCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerStartCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAddressableLEDStartCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelStartCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelStartCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAddressableLEDStartCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getStart
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getStart
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAddressableLEDStart(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setStart
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setStart
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAddressableLEDStart(index, value);
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
 * Method:    registerDataCallback
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_registerDataCallback
  (JNIEnv* env, jclass, jobject callback)
{
  return sim::AllocateConstBufferCallback(
      env, -1, callback,
      [](int32_t, HAL_ConstBufferCallback callback, void* param) {
        return HALSIM_RegisterAddressableLEDDataCallback(callback, param);
      });
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    cancelDataCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_cancelDataCallback
  (JNIEnv* env, jclass, jint handle)
{
  sim::FreeConstBufferCallback(env, handle, -1, [](int32_t, int32_t uid) {
    HALSIM_CancelAddressableLEDDataCallback(uid);
  });
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    getData
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_getData
  (JNIEnv* env, jclass, jint start, jint length)
{
  auto data =
      std::make_unique<HAL_AddressableLEDData[]>(HAL_kAddressableLEDMaxLength);
  length = HALSIM_GetAddressableLEDData(start, length, data.get());
  return MakeJByteArray(
      env, std::span(reinterpret_cast<jbyte*>(data.get()), length * 3));
}

/*
 * Class:     edu_wpi_first_hal_simulation_AddressableLEDDataJNI
 * Method:    setData
 * Signature: (I[B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AddressableLEDDataJNI_setData
  (JNIEnv* env, jclass, jint start, jbyteArray arr)
{
  JSpan<const jbyte> jArrRef{env, arr};
  auto arrRef = jArrRef.array();
  HALSIM_SetAddressableLEDData(
      start, arrRef.size() / 3,
      reinterpret_cast<const HAL_AddressableLEDData*>(arrRef.data()));
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

}  // extern "C"
