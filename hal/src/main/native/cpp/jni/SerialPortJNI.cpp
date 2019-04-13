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
#include "edu_wpi_first_hal_SerialPortJNI.h"
#include "hal/SerialPort.h"
#include "hal/cpp/Log.h"

using namespace frc;
using namespace wpi::java;

// set the logging level
TLogLevel serialJNILogLevel = logWARNING;

#define SERIALJNI_LOG(level)     \
  if (level > serialJNILogLevel) \
    ;                            \
  else                           \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialInitializePort
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialInitializePort
  (JNIEnv* env, jclass, jbyte port)
{
  SERIALJNI_LOG(logDEBUG) << "Calling Serial Initialize";
  SERIALJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_InitializeSerialPort(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialInitializePortDirect
 * Signature: (BLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialInitializePortDirect
  (JNIEnv* env, jclass, jbyte port, jstring portName)
{
  SERIALJNI_LOG(logDEBUG) << "Calling Serial Initialize Direct";
  SERIALJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  JStringRef portNameRef{env, portName};
  SERIALJNI_LOG(logDEBUG) << "PortName = " << portNameRef.c_str();
  int32_t status = 0;
  HAL_InitializeSerialPortDirect(static_cast<HAL_SerialPort>(port),
                                 portNameRef.c_str(), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetBaudRate
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetBaudRate
  (JNIEnv* env, jclass, jbyte port, jint rate)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Baud Rate";
  SERIALJNI_LOG(logDEBUG) << "Baud: " << rate;
  int32_t status = 0;
  HAL_SetSerialBaudRate(static_cast<HAL_SerialPort>(port), rate, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetDataBits
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetDataBits
  (JNIEnv* env, jclass, jbyte port, jbyte bits)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Data Bits";
  SERIALJNI_LOG(logDEBUG) << "Data Bits: " << bits;
  int32_t status = 0;
  HAL_SetSerialDataBits(static_cast<HAL_SerialPort>(port), bits, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetParity
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetParity
  (JNIEnv* env, jclass, jbyte port, jbyte parity)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Parity";
  SERIALJNI_LOG(logDEBUG) << "Parity: " << parity;
  int32_t status = 0;
  HAL_SetSerialParity(static_cast<HAL_SerialPort>(port), parity, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetStopBits
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetStopBits
  (JNIEnv* env, jclass, jbyte port, jbyte bits)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Stop Bits";
  SERIALJNI_LOG(logDEBUG) << "Stop Bits: " << bits;
  int32_t status = 0;
  HAL_SetSerialStopBits(static_cast<HAL_SerialPort>(port), bits, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetWriteMode
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetWriteMode
  (JNIEnv* env, jclass, jbyte port, jbyte mode)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Mode";
  SERIALJNI_LOG(logDEBUG) << "Write mode: " << mode;
  int32_t status = 0;
  HAL_SetSerialWriteMode(static_cast<HAL_SerialPort>(port), mode, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetFlowControl
 * Signature: (BB)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetFlowControl
  (JNIEnv* env, jclass, jbyte port, jbyte flow)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Flow Control";
  SERIALJNI_LOG(logDEBUG) << "Flow Control: " << flow;
  int32_t status = 0;
  HAL_SetSerialFlowControl(static_cast<HAL_SerialPort>(port), flow, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetTimeout
 * Signature: (BD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetTimeout
  (JNIEnv* env, jclass, jbyte port, jdouble timeout)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Timeout";
  SERIALJNI_LOG(logDEBUG) << "Timeout: " << timeout;
  int32_t status = 0;
  HAL_SetSerialTimeout(static_cast<HAL_SerialPort>(port), timeout, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialEnableTermination
 * Signature: (BC)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialEnableTermination
  (JNIEnv* env, jclass, jbyte port, jchar terminator)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Enable Termination";
  SERIALJNI_LOG(logDEBUG) << "Terminator: " << terminator;
  int32_t status = 0;
  HAL_EnableSerialTermination(static_cast<HAL_SerialPort>(port), terminator,
                              &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialDisableTermination
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialDisableTermination
  (JNIEnv* env, jclass, jbyte port)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Disable termination";
  int32_t status = 0;
  HAL_DisableSerialTermination(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetReadBufferSize
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetReadBufferSize
  (JNIEnv* env, jclass, jbyte port, jint size)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Read Buffer Size";
  SERIALJNI_LOG(logDEBUG) << "Size: " << size;
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(static_cast<HAL_SerialPort>(port), size, &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialSetWriteBufferSize
 * Signature: (BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialSetWriteBufferSize
  (JNIEnv* env, jclass, jbyte port, jint size)
{
  SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Buffer Size";
  SERIALJNI_LOG(logDEBUG) << "Size: " << size;
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(static_cast<HAL_SerialPort>(port), size,
                               &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialGetBytesReceived
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialGetBytesReceived
  (JNIEnv* env, jclass, jbyte port)
{
  SERIALJNI_LOG(logDEBUG) << "Serial Get Bytes Received";
  int32_t status = 0;
  jint retVal =
      HAL_GetSerialBytesReceived(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialRead
 * Signature: (B[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialRead
  (JNIEnv* env, jclass, jbyte port, jbyteArray dataReceived, jint size)
{
  SERIALJNI_LOG(logDEBUG) << "Serial Read";
  wpi::SmallVector<char, 128> recvBuf;
  recvBuf.resize(size);
  int32_t status = 0;
  jint retVal = HAL_ReadSerial(static_cast<HAL_SerialPort>(port),
                               recvBuf.data(), size, &status);
  env->SetByteArrayRegion(dataReceived, 0, size,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialWrite
 * Signature: (B[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialWrite
  (JNIEnv* env, jclass, jbyte port, jbyteArray dataToSend, jint size)
{
  SERIALJNI_LOG(logDEBUG) << "Serial Write";
  int32_t status = 0;
  jint retVal =
      HAL_WriteSerial(static_cast<HAL_SerialPort>(port),
                      reinterpret_cast<const char*>(
                          JByteArrayRef(env, dataToSend).array().data()),
                      size, &status);
  SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialFlush
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialFlush
  (JNIEnv* env, jclass, jbyte port)
{
  SERIALJNI_LOG(logDEBUG) << "Serial Flush";
  int32_t status = 0;
  HAL_FlushSerial(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialClear
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialClear
  (JNIEnv* env, jclass, jbyte port)
{
  SERIALJNI_LOG(logDEBUG) << "Serial Clear";
  int32_t status = 0;
  HAL_ClearSerial(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SerialPortJNI
 * Method:    serialClose
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SerialPortJNI_serialClose
  (JNIEnv* env, jclass, jbyte port)
{
  SERIALJNI_LOG(logDEBUG) << "Serial Close";
  int32_t status = 0;
  HAL_CloseSerial(static_cast<HAL_SerialPort>(port), &status);
  SERIALJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
