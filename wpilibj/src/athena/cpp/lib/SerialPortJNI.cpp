/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_SerialPortJNI.h"

#include "HAL/SerialPort.h"
#include "HALUtil.h"

// set the logging level
TLogLevel serialJNILogLevel = logWARNING;

#define SERIALJNI_LOG(level)     \
  if (level > serialJNILogLevel) \
    ;                            \
  else                           \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialInitializePort
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialInitializePort(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Calling Serial Initialize";
  SERIALJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_InitializeSerialPort(port, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetBaudRate
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetBaudRate(
    JNIEnv* env, jclass, jbyte port, jint rate) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Baud Rate";
  SERIALJNI_LOG(logDEBUG) << "Baud: " << rate;
  int32_t status = 0;
  HAL_SetSerialBaudRate(port, rate, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetDataBits
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetDataBits(
    JNIEnv* env, jclass, jbyte port, jbyte bits) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Data Bits";
  SERIALJNI_LOG(logDEBUG) << "Data Bits: " << bits;
  int32_t status = 0;
  HAL_SetSerialDataBits(port, bits, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetParity
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetParity(
    JNIEnv* env, jclass, jbyte port, jbyte parity) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Parity";
  SERIALJNI_LOG(logDEBUG) << "Parity: " << parity;
  int32_t status = 0;
  HAL_SetSerialParity(port, parity, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetStopBits
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetStopBits(
    JNIEnv* env, jclass, jbyte port, jbyte bits) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Stop Bits";
  SERIALJNI_LOG(logDEBUG) << "Stop Bits: " << bits;
  int32_t status = 0;
  HAL_SetSerialStopBits(port, bits, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetWriteMode
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetWriteMode(
    JNIEnv* env, jclass, jbyte port, jbyte mode) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Mode";
  SERIALJNI_LOG(logDEBUG) << "Write mode: " << mode;
  int32_t status = 0;
  HAL_SetSerialWriteMode(port, mode, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetFlowControl
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetFlowControl(
    JNIEnv* env, jclass, jbyte port, jbyte flow) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Flow Control";
  SERIALJNI_LOG(logDEBUG) << "Flow Control: " << flow;
  int32_t status = 0;
  HAL_SetSerialFlowControl(port, flow, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetTimeout
 * Signature: (BF)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetTimeout(
    JNIEnv* env, jclass, jbyte port, jfloat timeout) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Timeout";
  SERIALJNI_LOG(logDEBUG) << "Timeout: " << timeout;
  int32_t status = 0;
  HAL_SetSerialTimeout(port, timeout, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialEnableTermination
 * Signature: (BC)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialEnableTermination(
    JNIEnv* env, jclass, jbyte port, jchar terminator) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Enable Termination";
  SERIALJNI_LOG(logDEBUG) << "Terminator: " << terminator;
  int32_t status = 0;
  HAL_EnableSerialTermination(port, terminator, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialDisableTermination
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialDisableTermination(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Disable termination";
  int32_t status = 0;
  HAL_DisableSerialTermination(port, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetReadBufferSize
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetReadBufferSize(
    JNIEnv* env, jclass, jbyte port, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Read Buffer Size";
  SERIALJNI_LOG(logDEBUG) << "Size: " << size;
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(port, size, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetWriteBufferSize
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetWriteBufferSize(
    JNIEnv* env, jclass, jbyte port, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Buffer Size";
  SERIALJNI_LOG(logDEBUG) << "Size: " << size;
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(port, size, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialGetBytesRecieved
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialGetBytesRecieved(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Get Bytes Received";
  int32_t status = 0;
  jint retVal = HAL_GetSerialBytesReceived(port, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialRead
 * Signature: (BLjava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialRead(
    JNIEnv* env, jclass, jbyte port, jobject dataReceived, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Serial Read";
  jbyte* dataReceivedPtr = nullptr;
  dataReceivedPtr = (jbyte*)env->GetDirectBufferAddress(dataReceived);
  int32_t status = 0;
  jint retVal = HAL_ReadSerial(port, reinterpret_cast<char*>(dataReceivedPtr), size, &status);
  SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialWrite
 * Signature: (BLjava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialWrite(
    JNIEnv* env, jclass, jbyte port, jobject dataToSend, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Serial Write";
  jbyte* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr = (jbyte*)env->GetDirectBufferAddress(dataToSend);
  }
  int32_t status = 0;
  jint retVal = HAL_WriteSerial(port, reinterpret_cast<char*>(dataToSendPtr), size, &status);
  SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialFlush
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialFlush(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Flush";
  int32_t status = 0;
  HAL_FlushSerial(port, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialClear
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialClear(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Clear";
  int32_t status = 0;
  HAL_ClearSerial(port, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialClose
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialClose(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Close";
  int32_t status = 0;
  HAL_CloseSerial(port, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
