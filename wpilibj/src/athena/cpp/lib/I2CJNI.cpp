/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_I2CJNI.h"

#include "HAL/I2C.h"
#include "HALUtil.h"

using namespace frc;

// set the logging level
TLogLevel i2cJNILogLevel = logWARNING;

#define I2CJNI_LOG(level)     \
  if (level > i2cJNILogLevel) \
    ;                         \
  else                        \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2cInitialize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CInitialize(
    JNIEnv* env, jclass, jint port) {
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CInititalize";
  I2CJNI_LOG(logDEBUG) << "Port: " << port;
  int32_t status = 0;
  HAL_InitializeI2C(static_cast<HAL_I2CPort>(port), &status);
  I2CJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CTransaction
 * Signature: (IBLjava/nio/ByteBuffer;BLjava/nio/ByteBuffer;B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CTransaction(
    JNIEnv* env, jclass, jint port, jbyte address, jobject dataToSend,
    jbyte sendSize, jobject dataReceived, jbyte receiveSize) {
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CTransaction";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
  uint8_t* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr = (uint8_t*)env->GetDirectBufferAddress(dataToSend);
  }
  I2CJNI_LOG(logDEBUG) << "DataToSendPtr = " << (jint*)dataToSendPtr;
  I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)sendSize;
  uint8_t* dataReceivedPtr =
      (uint8_t*)env->GetDirectBufferAddress(dataReceived);
  I2CJNI_LOG(logDEBUG) << "DataReceivedPtr = " << (jint*)dataReceivedPtr;
  I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << (jint)receiveSize;
  jint returnValue = HAL_TransactionI2C(static_cast<HAL_I2CPort>(port), address, dataToSendPtr, sendSize,
                                    dataReceivedPtr, receiveSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CWrite
 * Signature: (IBLjava/nio/ByteBuffer;B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CWrite(
    JNIEnv* env, jclass, jint port, jbyte address, jobject dataToSend,
    jbyte sendSize) {
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CWrite";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
  uint8_t* dataToSendPtr = nullptr;

  if (dataToSend != 0) {
    dataToSendPtr = (uint8_t*)env->GetDirectBufferAddress(dataToSend);
  }
  I2CJNI_LOG(logDEBUG) << "DataToSendPtr = " << dataToSendPtr;
  I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)dataToSend;
  jint returnValue = HAL_WriteI2C(static_cast<HAL_I2CPort>(port), address, dataToSendPtr, sendSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CRead
 * Signature: (IBLjava/nio/ByteBuffer;B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CRead(
    JNIEnv* env, jclass, jint port, jbyte address, jobject dataReceived,
    jbyte receiveSize) {
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CRead";
  I2CJNI_LOG(logDEBUG) << "Port = " << port;
  I2CJNI_LOG(logDEBUG) << "Address = " << address;
  uint8_t* dataReceivedPtr =
      (uint8_t*)env->GetDirectBufferAddress(dataReceived);
  I2CJNI_LOG(logDEBUG) << "DataReceivedPtr = " << dataReceivedPtr;
  I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << receiveSize;
  jint returnValue = HAL_ReadI2C(static_cast<HAL_I2CPort>(port), address, dataReceivedPtr, receiveSize);
  I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CClose(JNIEnv*, jclass, jint port) {
  I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CClose";
  HAL_CloseI2C(static_cast<HAL_I2CPort>(port));
}

}  // extern "C"
