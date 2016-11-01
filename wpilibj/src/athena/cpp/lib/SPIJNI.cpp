/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_SPIJNI.h"

#include "HAL/SPI.h"
#include "HALUtil.h"

using namespace frc;

// set the logging level
TLogLevel spiJNILogLevel = logWARNING;

#define SPIJNI_LOG(level)     \
  if (level > spiJNILogLevel) \
    ;                         \
  else                        \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiInitialize
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiInitialize(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiInitialize";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_InitializeSPI(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiTransaction
 * Signature: (BLjava/nio/ByteBuffer;Ljava/nio/ByteBuffer;B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiTransaction(
    JNIEnv *env, jclass, jbyte port, jobject dataToSend, jobject dataReceived,
    jbyte size) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiTransaction";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  uint8_t *dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr = (uint8_t *)env->GetDirectBufferAddress(dataToSend);
  }
  uint8_t *dataReceivedPtr =
      (uint8_t *)env->GetDirectBufferAddress(dataReceived);
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  SPIJNI_LOG(logDEBUG) << "DataToSendPtr = " << dataToSendPtr;
  SPIJNI_LOG(logDEBUG) << "DataReceivedPtr = " << dataReceivedPtr;
  jint retVal = HAL_TransactionSPI(port, dataToSendPtr, dataReceivedPtr, size);
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiWrite
 * Signature: (BLjava/nio/ByteBuffer;B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiWrite(
    JNIEnv *env, jclass, jbyte port, jobject dataToSend, jbyte size) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiWrite";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  uint8_t *dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr = (uint8_t *)env->GetDirectBufferAddress(dataToSend);
  }
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  SPIJNI_LOG(logDEBUG) << "DataToSendPtr = " << dataToSendPtr;
  jint retVal = HAL_WriteSPI(port, dataToSendPtr, size);
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiRead
 * Signature: (BLjava/nio/ByteBuffer;B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiRead(
    JNIEnv *env, jclass, jbyte port, jobject dataReceived, jbyte size) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiRead";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  uint8_t *dataReceivedPtr =
      (uint8_t *)env->GetDirectBufferAddress(dataReceived);
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  SPIJNI_LOG(logDEBUG) << "DataReceivedPtr = " << dataReceivedPtr;
  jint retVal = HAL_ReadSPI(port, (uint8_t *)dataReceivedPtr, size);
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiClose
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiClose(JNIEnv *, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiClose";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  HAL_CloseSPI(port);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiSetSpeed
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiSetSpeed(
    JNIEnv *, jclass, jbyte port, jint speed) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetSpeed";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Speed = " << (jint)speed;
  HAL_SetSPISpeed(port, speed);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiSetOpts
 * Signature: (BIII)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiSetOpts(
    JNIEnv *, jclass, jbyte port, jint msb_first, jint sample_on_trailing,
    jint clk_idle_high) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetOpts";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "msb_first = " << msb_first;
  SPIJNI_LOG(logDEBUG) << "sample_on_trailing = " << sample_on_trailing;
  SPIJNI_LOG(logDEBUG) << "clk_idle_high = " << clk_idle_high;
  HAL_SetSPIOpts(port, msb_first, sample_on_trailing, clk_idle_high);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiSetChipSelectActiveHigh
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiSetChipSelectActiveHigh(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetCSActiveHigh";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveHigh(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiSetChipSelectActiveLow
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiSetChipSelectActiveLow(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetCSActiveLow";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveLow(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiInitAccumulator
 * Signature: (BIIBIIBBZZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiInitAccumulator(
    JNIEnv *env, jclass, jbyte port, jint period, jint cmd, jbyte xferSize,
    jint validMask, jint validValue, jbyte dataShift, jbyte dataSize,
    jboolean isSigned, jboolean bigEndian) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiInitAccumulator";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Period = " << period;
  SPIJNI_LOG(logDEBUG) << "Cmd = " << cmd;
  SPIJNI_LOG(logDEBUG) << "XferSize = " << (jint)xferSize;
  SPIJNI_LOG(logDEBUG) << "ValidMask = " << validMask;
  SPIJNI_LOG(logDEBUG) << "ValidValue = " << validValue;
  SPIJNI_LOG(logDEBUG) << "DataShift = " << (jint)dataShift;
  SPIJNI_LOG(logDEBUG) << "DataSize = " << (jint)dataSize;
  SPIJNI_LOG(logDEBUG) << "IsSigned = " << (jint)isSigned;
  SPIJNI_LOG(logDEBUG) << "BigEndian = " << (jint)bigEndian;
  int32_t status = 0;
  HAL_InitSPIAccumulator(port, period, cmd, xferSize, validMask, validValue,
                     dataShift, dataSize, isSigned, bigEndian, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiFreeAccumulator
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiFreeAccumulator(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiFreeAccumulator";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_FreeSPIAccumulator(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiResetAccumulator
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiResetAccumulator(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiResetAccumulator";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_ResetSPIAccumulator(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiSetAccumulatorCenter
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiSetAccumulatorCenter(
    JNIEnv *env, jclass, jbyte port, jint center) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetAccumulatorCenter";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Center = " << center;
  int32_t status = 0;
  HAL_SetSPIAccumulatorCenter(port, center, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiSetAccumulatorDeadband
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiSetAccumulatorDeadband(
    JNIEnv *env, jclass, jbyte port, jint deadband) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetAccumulatorDeadband";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Deadband = " << deadband;
  int32_t status = 0;
  HAL_SetSPIAccumulatorDeadband(port, deadband, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiGetAccumulatorLastValue
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiGetAccumulatorLastValue(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiGetAccumulatorLastValue";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  jint retVal = HAL_GetSPIAccumulatorLastValue(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiGetAccumulatorValue
 * Signature: (B)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiGetAccumulatorValue(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiGetAccumulatorValue";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  jlong retVal = HAL_GetSPIAccumulatorValue(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiGetAccumulatorCount
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiGetAccumulatorCount(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiGetAccumulatorCount";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  jint retVal = HAL_GetSPIAccumulatorCount(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiGetAccumulatorAverage
 * Signature: (B)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiGetAccumulatorAverage(
    JNIEnv *env, jclass, jbyte port) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiGetAccumulatorAverage";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  jdouble retVal = HAL_GetSPIAccumulatorAverage(port, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    spiGetAccumulatorOutput
 * Signature: (BLjava/nio/LongBuffer;Ljava/nio/LongBuffer;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SPIJNI_spiGetAccumulatorOutput(
    JNIEnv *env, jclass, jbyte port, jobject value, jobject count) {
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiGetAccumulatorOutput";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;

  jlong *valuePtr = (jlong *)env->GetDirectBufferAddress(value);
  jlong *countPtr = (jlong *)env->GetDirectBufferAddress(count);

  HAL_GetSPIAccumulatorOutput(port, valuePtr, countPtr, &status);

  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "Value = " << *valuePtr;
  SPIJNI_LOG(logDEBUG) << "Count = " << *countPtr;
  CheckStatus(env, status);
}

}  // extern "C"
