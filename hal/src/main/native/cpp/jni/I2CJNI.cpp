/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_I2CJNI.h"
#include "hal/I2C.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    registerI2CBus
 * Signature: (ILedu/wpi/first/hal/I2CBus;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_I2CJNI_registerI2CBus
  (JNIEnv* env, jclass, jint bus, jobject impl)
{
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    getSystemI2CBus
 * Signature: (I)Ledu/wpi/first/hal/I2CBus;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_I2CJNI_getSystemI2CBus
  (JNIEnv* env, jclass, jint bus)
{
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    getRegisteredI2CBus
 * Signature: (I)Ledu/wpi/first/hal/I2CBus;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_I2CJNI_getRegisteredI2CBus
  (JNIEnv* env, jclass, jint bus)
{
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    unregisterI2CBus
 * Signature: (I)Ledu/wpi/first/hal/I2CBus;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_I2CJNI_unregisterI2CBus
  (JNIEnv* env, jclass, jint bus)
{
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    nativeOpenBus
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_I2CJNI_nativeOpenBus
  (JNIEnv* env, jclass, jlong impl)
{
  int32_t status = 0;
  bool rv = reinterpret_cast<I2CBus*>(impl)->OpenBus(&status);
  CheckStatusForceThrow(env, status);
  return rv;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    nativeCloseBus
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_I2CJNI_nativeCloseBus
  (JNIEnv*, jclass, jlong impl)
{
  reinterpret_cast<I2CBus*>(impl)->CloseBus();
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    nativeInitialize
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_I2CJNI_nativeInitialize
  (JNIEnv* env, jclass, jlong impl, jint deviceAddress)
{
  int32_t status = 0;
  bool rv = reinterpret_cast<I2CBus*>(impl)->Initialize(deviceAddress, &status);
  CheckStatusForceThrow(env, status);
  return rv;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    nativeTransaction
 * Signature: (JI[BI[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_nativeTransaction
  (JNIEnv* env, jclass, jlong impl, jint deviceAddress, jbyteArray dataToSend,
   jint sendSize, jbyteArray dataReceived, jint receiveSize)
{
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  jint rv = reinterpret_cast<I2CBus*>(impl)->Transaction(
      deviceAddress,
      reinterpret_cast<const uint8_t*>(
          JByteArrayRef(env, dataToSend).array().data()),
      sendSize, recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  return rv;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    nativeWrite
 * Signature: (JI[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_nativeWrite
  (JNIEnv* env, jclass, jlong impl, jint deviceAddress, jbyteArray dataToSend,
   jint sendSize)
{
  return reinterpret_cast<I2CBus*>(impl)->Write(
      deviceAddress,
      reinterpret_cast<const uint8_t*>(
          JByteArrayRef(env, dataToSend).array().data()),
      sendSize);
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    nativeRead
 * Signature: (JI[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_nativeRead
  (JNIEnv* env, jclass, jlong impl, jint deviceAddress, jbyteArray buffer,
   jint count)
{
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(count);
  jint rv = reinterpret_cast<I2CBus*>(impl)->Read(deviceAddress, recvBuf.data(),
                                                  count);
  env->SetByteArrayRegion(buffer, 0, count,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  return rv;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CInitialize
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CInitialize
  (JNIEnv* env, jclass, jint bus, jint deviceAddress)
{
  int32_t status = 0;
  jint handle = HAL_InitializeI2C(bus, deviceAddress, &status);
  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CTransaction
 * Signature: (ILjava/lang/Object;BLjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CTransaction
  (JNIEnv* env, jclass, jint handle, jobject dataToSend, jbyte sendSize,
   jobject dataReceived, jbyte receiveSize)
{
  uint8_t* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  jint returnValue = HAL_TransactionI2C(handle, dataToSendPtr, sendSize,
                                        dataReceivedPtr, receiveSize);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CTransactionB
 * Signature: (I[BB[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CTransactionB
  (JNIEnv* env, jclass, jint handle, jbyteArray dataToSend, jbyte sendSize,
   jbyteArray dataReceived, jbyte receiveSize)
{
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  jint rv =
      HAL_TransactionI2C(handle,
                         reinterpret_cast<const uint8_t*>(
                             JByteArrayRef(env, dataToSend).array().data()),
                         sendSize, recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  return rv;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CWrite
 * Signature: (ILjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CWrite
  (JNIEnv* env, jclass, jint handle, jobject dataToSend, jbyte sendSize)
{
  uint8_t* dataToSendPtr = nullptr;

  if (dataToSend != 0) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  jint returnValue = HAL_WriteI2C(handle, dataToSendPtr, sendSize);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CWriteB
 * Signature: (I[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CWriteB
  (JNIEnv* env, jclass, jint handle, jbyteArray dataToSend, jbyte sendSize)
{
  return HAL_WriteI2C(handle,
                      reinterpret_cast<const uint8_t*>(
                          JByteArrayRef(env, dataToSend).array().data()),
                      sendSize);
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CRead
 * Signature: (ILjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CRead
  (JNIEnv* env, jclass, jint handle, jobject dataReceived, jbyte receiveSize)
{
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  return HAL_ReadI2C(handle, dataReceivedPtr, receiveSize);
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CReadB
 * Signature: (I[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CReadB
  (JNIEnv* env, jclass, jint handle, jbyteArray dataReceived, jbyte receiveSize)
{
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  jint rv = HAL_ReadI2C(handle, recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  return rv;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CClose
  (JNIEnv*, jclass, jint handle)
{
  HAL_CloseI2C(handle);
}

}  // extern "C"
