/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_OSSerialPortJNI.h"

#include "HAL/OSSerialPort.h"
#include "HALUtil.h"

using namespace frc;

// set the logging level
TLogLevel osserialJNILogLevel = logWARNING;

#define SERIALJNI_LOG(level)       \
  if (level > osserialJNILogLevel) \
    ;                              \
  else                             \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialInitializePort
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialInitializePort(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Calling Serial Initialize";
  SERIALJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_InitializeOSSerialPort(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetBaudRate
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetBaudRate(
    JNIEnv* env, jclass, jbyte port, jint rate) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Baud Rate";
  SERIALJNI_LOG(logDEBUG) << "Baud: " << rate;
  int32_t status = 0;
  HAL_SetOSSerialBaudRate(static_cast<HAL_SerialPort>(port), rate, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetDataBits
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetDataBits(
    JNIEnv* env, jclass, jbyte port, jbyte bits) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Data Bits";
  SERIALJNI_LOG(logDEBUG) << "Data Bits: " << bits;
  int32_t status = 0;
  HAL_SetOSSerialDataBits(static_cast<HAL_SerialPort>(port), bits, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetParity
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetParity(
    JNIEnv* env, jclass, jbyte port, jbyte parity) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Parity";
  SERIALJNI_LOG(logDEBUG) << "Parity: " << parity;
  int32_t status = 0;
  HAL_SetOSSerialParity(static_cast<HAL_SerialPort>(port), parity, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetStopBits
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetStopBits(
    JNIEnv* env, jclass, jbyte port, jbyte bits) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Stop Bits";
  SERIALJNI_LOG(logDEBUG) << "Stop Bits: " << bits;
  int32_t status = 0;
  HAL_SetOSSerialStopBits(static_cast<HAL_SerialPort>(port), bits, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetWriteMode
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetWriteMode(
    JNIEnv* env, jclass, jbyte port, jbyte mode) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Mode";
  SERIALJNI_LOG(logDEBUG) << "Write mode: " << mode;
  int32_t status = 0;
  HAL_SetOSSerialWriteMode(static_cast<HAL_SerialPort>(port), mode, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetFlowControl
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetFlowControl(
    JNIEnv* env, jclass, jbyte port, jbyte flow) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Flow Control";
  SERIALJNI_LOG(logDEBUG) << "Flow Control: " << flow;
  int32_t status = 0;
  HAL_SetOSSerialFlowControl(static_cast<HAL_SerialPort>(port), flow, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetTimeout
 * Signature: (BD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetTimeout(
    JNIEnv* env, jclass, jbyte port, jdouble timeout) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Timeout";
  SERIALJNI_LOG(logDEBUG) << "Timeout: " << timeout;
  int32_t status = 0;
  HAL_SetOSSerialTimeout(static_cast<HAL_SerialPort>(port), timeout, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialEnableTermination
 * Signature: (BC)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialEnableTermination(
    JNIEnv* env, jclass, jbyte port, jchar terminator) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Enable Termination";
  SERIALJNI_LOG(logDEBUG) << "Terminator: " << terminator;
  int32_t status = 0;
  HAL_EnableOSSerialTermination(static_cast<HAL_SerialPort>(port), terminator, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialDisableTermination
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialDisableTermination(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Disable termination";
  int32_t status = 0;
  HAL_DisableOSSerialTermination(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetReadBufferSize
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetReadBufferSize(
    JNIEnv* env, jclass, jbyte port, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Read Buffer Size";
  SERIALJNI_LOG(logDEBUG) << "Size: " << size;
  int32_t status = 0;
  HAL_SetOSSerialReadBufferSize(static_cast<HAL_SerialPort>(port), size, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialSetWriteBufferSize
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialSetWriteBufferSize(
    JNIEnv* env, jclass, jbyte port, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Buffer Size";
  SERIALJNI_LOG(logDEBUG) << "Size: " << size;
  int32_t status = 0;
  HAL_SetOSSerialWriteBufferSize(static_cast<HAL_SerialPort>(port), size, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialGetBytesRecieved
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialGetBytesRecieved(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Get Bytes Received";
  int32_t status = 0;
  jint retVal = HAL_GetOSSerialBytesReceived(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialRead
 * Signature: (BLjava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialRead(
    JNIEnv* env, jclass, jbyte port, jobject dataReceived, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Serial Read";
  jbyte* dataReceivedPtr = nullptr;
  dataReceivedPtr = (jbyte*)env->GetDirectBufferAddress(dataReceived);
  int32_t status = 0;
  jint retVal = HAL_ReadOSSerial(static_cast<HAL_SerialPort>(port), reinterpret_cast<char*>(dataReceivedPtr), 
                                 size, &status);
  SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialWrite
 * Signature: (BLjava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialWrite(
    JNIEnv* env, jclass, jbyte port, jobject dataToSend, jint size) {
  SERIALJNI_LOG(logDEBUG) << "Serial Write";
  jbyte* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr = (jbyte*)env->GetDirectBufferAddress(dataToSend);
  }
  int32_t status = 0;
  jint retVal = HAL_WriteOSSerial(static_cast<HAL_SerialPort>(port), reinterpret_cast<char*>(dataToSendPtr), 
                                  size, &status);
  SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialFlush
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialFlush(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Flush";
  int32_t status = 0;
  HAL_FlushOSSerial(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialClear
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialClear(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Clear";
  int32_t status = 0;
  HAL_ClearOSSerial(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_OSSerialPortJNI
 * Method:    serialClose
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_OSSerialPortJNI_serialClose(
    JNIEnv* env, jclass, jbyte port) {
  SERIALJNI_LOG(logDEBUG) << "Serial Close";
  int32_t status = 0;
  HAL_CloseOSSerial(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
