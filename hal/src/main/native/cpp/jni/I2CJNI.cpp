/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
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
#include "hal/cpp/Log.h"

using namespace frc;
using namespace wpi::java;

// set the logging level
TLogLevel i2cJNILogLevel = logWARNING;

#define I2CJNI_LOG(level)     \
  if (level > i2cJNILogLevel) \
    ;                         \
  else                        \
    Log().Get(level)

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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CInititalize";
  I2CJNI_LOG(logDEBUG) << "Port: " << port;
  int32_t status = 0;
  HAL_InitializeI2C(static_cast<HAL_I2CPort>(port), &status);
  I2CJNI_LOG(logDEBUG) << "Status = " << status;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CTransaction";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
  uint8_t* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  I2CJNI_LOG(logDEBUG) << "DataToSendPtr = "
                       << reinterpret_cast<jint*>(dataToSendPtr);
  I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)sendSize;
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  I2CJNI_LOG(logDEBUG) << "DataReceivedPtr = "
                       << reinterpret_cast<jint*>(dataReceivedPtr);
  I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << (jint)receiveSize;
  jint returnValue =
      HAL_TransactionI2C(static_cast<HAL_I2CPort>(port), address, dataToSendPtr,
                         sendSize, dataReceivedPtr, receiveSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CTransactionB";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
  I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)sendSize;
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << (jint)receiveSize;
  jint returnValue =
      HAL_TransactionI2C(static_cast<HAL_I2CPort>(port), address,
                         reinterpret_cast<const uint8_t*>(
                             JByteArrayRef(env, dataToSend).array().data()),
                         sendSize, recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CWrite";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
  uint8_t* dataToSendPtr = nullptr;

  if (dataToSend != 0) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  I2CJNI_LOG(logDEBUG) << "DataToSendPtr = " << dataToSendPtr;
  I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)sendSize;
  jint returnValue = HAL_WriteI2C(static_cast<HAL_I2CPort>(port), address,
                                  dataToSendPtr, sendSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)returnValue;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CWrite";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
  I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)sendSize;
  jint returnValue =
      HAL_WriteI2C(static_cast<HAL_I2CPort>(port), address,
                   reinterpret_cast<const uint8_t*>(
                       JByteArrayRef(env, dataToSend).array().data()),
                   sendSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)returnValue;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CRead";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << address;
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  I2CJNI_LOG(logDEBUG) << "DataReceivedPtr = " << dataReceivedPtr;
  I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << receiveSize;
  jint returnValue = HAL_ReadI2C(static_cast<HAL_I2CPort>(port), address,
                                 dataReceivedPtr, receiveSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CRead";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << address;
  I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << receiveSize;
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(receiveSize);
  jint returnValue = HAL_ReadI2C(static_cast<HAL_I2CPort>(port), address,
                                 recvBuf.data(), receiveSize);
  env->SetByteArrayRegion(dataReceived, 0, receiveSize,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
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
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CClose";
  HAL_CloseI2C(static_cast<HAL_I2CPort>(port));
}

}  // extern "C"
