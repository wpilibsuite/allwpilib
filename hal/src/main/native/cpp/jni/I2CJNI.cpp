// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
 * Method:    i2CInitialize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CInitialize
  (JNIEnv* env, jclass, jint port)
{
  int32_t status = 0;
  HAL_InitializeI2C(static_cast<HAL_I2CPort>(port), &status);
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CTransaction
 * Signature: (IBLjava/lang/Object;BLjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CTransaction
  (JNIEnv* env, jclass, jint port, jbyte address, jobject dataToSend,
   jbyte sendSize, jobject dataReceived, jbyte receiveSize)
{
  uint8_t* dataToSendPtr = nullptr;
  if (dataToSend != nullptr) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  jint returnValue =
      HAL_TransactionI2C(static_cast<HAL_I2CPort>(port), address, dataToSendPtr,
                         sendSize, dataReceivedPtr, receiveSize);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CTransactionB
 * Signature: (IB[BB[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CTransactionB
  (JNIEnv* env, jclass, jint port, jbyte address, jbyteArray dataToSend,
   jbyte sendSize, jbyteArray dataReceived, jbyte receiveSize)
{
  if (sendSize < 0) {
    ThrowIllegalArgumentException(env, "I2CJNI.i2cTransactionB() sendSize < 0");
    return 0;
  }
  if (receiveSize < 0) {
    ThrowIllegalArgumentException(env,
                                  "I2CJNI.i2cTransactionB() receiveSize < 0");
    return 0;
  }

  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  jint returnValue =
      HAL_TransactionI2C(static_cast<HAL_I2CPort>(port), address,
                         reinterpret_cast<const uint8_t*>(
                             JByteArrayRef(env, dataToSend).array().data()),
                         sendSize, recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CWrite
 * Signature: (IBLjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CWrite
  (JNIEnv* env, jclass, jint port, jbyte address, jobject dataToSend,
   jbyte sendSize)
{
  uint8_t* dataToSendPtr = nullptr;

  if (dataToSend != nullptr) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  jint returnValue = HAL_WriteI2C(static_cast<HAL_I2CPort>(port), address,
                                  dataToSendPtr, sendSize);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CWriteB
 * Signature: (IB[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CWriteB
  (JNIEnv* env, jclass, jint port, jbyte address, jbyteArray dataToSend,
   jbyte sendSize)
{
  jint returnValue =
      HAL_WriteI2C(static_cast<HAL_I2CPort>(port), address,
                   reinterpret_cast<const uint8_t*>(
                       JByteArrayRef(env, dataToSend).array().data()),
                   sendSize);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CRead
 * Signature: (IBLjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CRead
  (JNIEnv* env, jclass, jint port, jbyte address, jobject dataReceived,
   jbyte receiveSize)
{
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  jint returnValue = HAL_ReadI2C(static_cast<HAL_I2CPort>(port), address,
                                 dataReceivedPtr, receiveSize);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CReadB
 * Signature: (IB[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CReadB
  (JNIEnv* env, jclass, jint port, jbyte address, jbyteArray dataReceived,
   jbyte receiveSize)
{
  if (receiveSize < 0) {
    ThrowIllegalArgumentException(env, "I2CJNI.i2cReadB() receiveSize < 0");
    return 0;
  }

  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  jint returnValue = HAL_ReadI2C(static_cast<HAL_I2CPort>(port), address,
                                 recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_I2CJNI
 * Method:    i2CClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_I2CJNI_i2CClose
  (JNIEnv*, jclass, jint port)
{
  HAL_CloseI2C(static_cast<HAL_I2CPort>(port));
}

}  // extern "C"
