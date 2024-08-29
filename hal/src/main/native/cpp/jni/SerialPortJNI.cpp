// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_SerialPortJNI.h"
#include "hal/SerialPort.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialInitializePort
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialInitializePort
  (JNIEnv* env, jclass, jbyte port)
{
  int32_t status = 0;
  auto handle =
      HAL_InitializeSerialPort(static_cast<HAL_SerialPort>(port), &status);
  CheckStatusForceThrow(env, status);
  return static_cast<jint>(handle);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialInitializePortDirect
 * Signature: (BLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialInitializePortDirect
  (JNIEnv* env, jclass, jbyte port, jstring portName)
{
  JStringRef portNameRef{env, portName};
  int32_t status = 0;
  auto handle = HAL_InitializeSerialPortDirect(
      static_cast<HAL_SerialPort>(port), portNameRef.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return static_cast<jint>(handle);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetBaudRate
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetBaudRate
  (JNIEnv* env, jclass, jint handle, jint rate)
{
  int32_t status = 0;
  HAL_SetSerialBaudRate(static_cast<HAL_SerialPortHandle>(handle), rate,
                        &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetDataBits
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetDataBits
  (JNIEnv* env, jclass, jint handle, jbyte bits)
{
  int32_t status = 0;
  HAL_SetSerialDataBits(static_cast<HAL_SerialPortHandle>(handle), bits,
                        &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetParity
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetParity
  (JNIEnv* env, jclass, jint handle, jbyte parity)
{
  int32_t status = 0;
  HAL_SetSerialParity(static_cast<HAL_SerialPortHandle>(handle), parity,
                      &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetStopBits
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetStopBits
  (JNIEnv* env, jclass, jint handle, jbyte bits)
{
  int32_t status = 0;
  HAL_SetSerialStopBits(static_cast<HAL_SerialPortHandle>(handle), bits,
                        &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetWriteMode
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetWriteMode
  (JNIEnv* env, jclass, jint handle, jbyte mode)
{
  int32_t status = 0;
  HAL_SetSerialWriteMode(static_cast<HAL_SerialPortHandle>(handle), mode,
                         &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetFlowControl
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetFlowControl
  (JNIEnv* env, jclass, jint handle, jbyte flow)
{
  int32_t status = 0;
  HAL_SetSerialFlowControl(static_cast<HAL_SerialPortHandle>(handle), flow,
                           &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetTimeout
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetTimeout
  (JNIEnv* env, jclass, jint handle, jdouble timeout)
{
  int32_t status = 0;
  HAL_SetSerialTimeout(static_cast<HAL_SerialPortHandle>(handle), timeout,
                       &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialEnableTermination
 * Signature: (IC)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialEnableTermination
  (JNIEnv* env, jclass, jint handle, jchar terminator)
{
  int32_t status = 0;
  HAL_EnableSerialTermination(static_cast<HAL_SerialPortHandle>(handle),
                              terminator, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialDisableTermination
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialDisableTermination
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_DisableSerialTermination(static_cast<HAL_SerialPortHandle>(handle),
                               &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetReadBufferSize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetReadBufferSize
  (JNIEnv* env, jclass, jint handle, jint size)
{
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(static_cast<HAL_SerialPortHandle>(handle), size,
                              &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetWriteBufferSize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetWriteBufferSize
  (JNIEnv* env, jclass, jint handle, jint size)
{
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(static_cast<HAL_SerialPortHandle>(handle), size,
                               &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialGetBytesReceived
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialGetBytesReceived
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  jint retVal = HAL_GetSerialBytesReceived(
      static_cast<HAL_SerialPortHandle>(handle), &status);
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialRead
 * Signature: (I[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialRead
  (JNIEnv* env, jclass, jint handle, jbyteArray dataReceived, jint size)
{
  wpi::SmallVector<char, 128> recvBuf;
  recvBuf.resize(size);
  int32_t status = 0;
  jint retVal = HAL_ReadSerial(static_cast<HAL_SerialPortHandle>(handle),
                               recvBuf.data(), size, &status);
  env->SetByteArrayRegion(dataReceived, 0, size,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialWrite
 * Signature: (I[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialWrite
  (JNIEnv* env, jclass, jint handle, jbyteArray dataToSend, jint size)
{
  int32_t status = 0;
  jint retVal = HAL_WriteSerial(
      static_cast<HAL_SerialPortHandle>(handle),
      reinterpret_cast<const char*>(JSpan<const jbyte>(env, dataToSend).data()),
      size, &status);
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialFlush
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialFlush
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_FlushSerial(static_cast<HAL_SerialPortHandle>(handle), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialClear
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialClear
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearSerial(static_cast<HAL_SerialPortHandle>(handle), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialClose
  (JNIEnv* env, jclass, jint handle)
{
  if (handle != HAL_kInvalidHandle) {
    HAL_CloseSerial(static_cast<HAL_SerialPortHandle>(handle));
  }
}

}  // extern "C"
