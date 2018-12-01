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
#include "edu_wpi_first_hal_SPIJNI.h"
#include "hal/SPI.h"
#include "hal/cpp/Log.h"

using namespace frc;
using namespace wpi::java;

// set the logging level
TLogLevel spiJNILogLevel = logWARNING;

#define SPIJNI_LOG(level)     \
  if (level > spiJNILogLevel) \
    ;                         \
  else                        \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiInitialize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiInitialize
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiInitialize";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_InitializeSPI(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatusForceThrow(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiTransaction
 * Signature: (ILjava/lang/Object;Ljava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiTransaction
  (JNIEnv* env, jclass, jint port, jobject dataToSend, jobject dataReceived,
   jbyte size)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiTransaction";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  uint8_t* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  SPIJNI_LOG(logDEBUG) << "DataToSendPtr = " << dataToSendPtr;
  SPIJNI_LOG(logDEBUG) << "DataReceivedPtr = " << dataReceivedPtr;
  jint retVal = HAL_TransactionSPI(static_cast<HAL_SPIPort>(port),
                                   dataToSendPtr, dataReceivedPtr, size);
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiTransactionB
 * Signature: (I[B[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiTransactionB
  (JNIEnv* env, jclass, jint port, jbyteArray dataToSend,
   jbyteArray dataReceived, jbyte size)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiTransactionB";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(size);
  jint retVal =
      HAL_TransactionSPI(static_cast<HAL_SPIPort>(port),
                         reinterpret_cast<const uint8_t*>(
                             JByteArrayRef(env, dataToSend).array().data()),
                         recvBuf.data(), size);
  env->SetByteArrayRegion(dataReceived, 0, size,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiWrite
 * Signature: (ILjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiWrite
  (JNIEnv* env, jclass, jint port, jobject dataToSend, jbyte size)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiWrite";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  uint8_t* dataToSendPtr = nullptr;
  if (dataToSend != 0) {
    dataToSendPtr =
        reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataToSend));
  }
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  SPIJNI_LOG(logDEBUG) << "DataToSendPtr = " << dataToSendPtr;
  jint retVal =
      HAL_WriteSPI(static_cast<HAL_SPIPort>(port), dataToSendPtr, size);
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiWriteB
 * Signature: (I[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiWriteB
  (JNIEnv* env, jclass, jint port, jbyteArray dataToSend, jbyte size)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiWriteB";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  jint retVal = HAL_WriteSPI(static_cast<HAL_SPIPort>(port),
                             reinterpret_cast<const uint8_t*>(
                                 JByteArrayRef(env, dataToSend).array().data()),
                             size);
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiRead
 * Signature: (IZLjava/lang/Object;B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiRead
  (JNIEnv* env, jclass, jint port, jboolean initiate, jobject dataReceived,
   jbyte size)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiRead";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Initiate = " << (jboolean)initiate;
  uint8_t* dataReceivedPtr =
      reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(dataReceived));
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  SPIJNI_LOG(logDEBUG) << "DataReceivedPtr = " << dataReceivedPtr;
  jint retVal;
  if (initiate) {
    wpi::SmallVector<uint8_t, 128> sendBuf;
    sendBuf.resize(size);
    retVal = HAL_TransactionSPI(static_cast<HAL_SPIPort>(port), sendBuf.data(),
                                dataReceivedPtr, size);
  } else {
    retVal = HAL_ReadSPI(static_cast<HAL_SPIPort>(port),
                         reinterpret_cast<uint8_t*>(dataReceivedPtr), size);
  }
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiReadB
 * Signature: (IZ[BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiReadB
  (JNIEnv* env, jclass, jint port, jboolean initiate, jbyteArray dataReceived,
   jbyte size)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiReadB";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Initiate = " << (jboolean)initiate;
  SPIJNI_LOG(logDEBUG) << "Size = " << (jint)size;
  jint retVal;
  wpi::SmallVector<uint8_t, 128> recvBuf;
  recvBuf.resize(size);
  if (initiate) {
    wpi::SmallVector<uint8_t, 128> sendBuf;
    sendBuf.resize(size);
    retVal = HAL_TransactionSPI(static_cast<HAL_SPIPort>(port), sendBuf.data(),
                                recvBuf.data(), size);
  } else {
    retVal = HAL_ReadSPI(static_cast<HAL_SPIPort>(port), recvBuf.data(), size);
  }
  env->SetByteArrayRegion(dataReceived, 0, size,
                          reinterpret_cast<const jbyte*>(recvBuf.data()));
  SPIJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)retVal;
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiClose
  (JNIEnv*, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiClose";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  HAL_CloseSPI(static_cast<HAL_SPIPort>(port));
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiSetSpeed
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiSetSpeed
  (JNIEnv*, jclass, jint port, jint speed)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetSpeed";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "Speed = " << (jint)speed;
  HAL_SetSPISpeed(static_cast<HAL_SPIPort>(port), speed);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiSetOpts
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiSetOpts
  (JNIEnv*, jclass, jint port, jint msb_first, jint sample_on_trailing,
   jint clk_idle_high)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetOpts";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  SPIJNI_LOG(logDEBUG) << "msb_first = " << msb_first;
  SPIJNI_LOG(logDEBUG) << "sample_on_trailing = " << sample_on_trailing;
  SPIJNI_LOG(logDEBUG) << "clk_idle_high = " << clk_idle_high;
  HAL_SetSPIOpts(static_cast<HAL_SPIPort>(port), msb_first, sample_on_trailing,
                 clk_idle_high);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiSetChipSelectActiveHigh
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiSetChipSelectActiveHigh
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetCSActiveHigh";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveHigh(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiSetChipSelectActiveLow
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiSetChipSelectActiveLow
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetCSActiveLow";
  SPIJNI_LOG(logDEBUG) << "Port = " << (jint)port;
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveLow(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiInitAuto
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiInitAuto
  (JNIEnv* env, jclass, jint port, jint bufferSize)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiInitAuto";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  SPIJNI_LOG(logDEBUG) << "BufferSize = " << bufferSize;
  int32_t status = 0;
  HAL_InitSPIAuto(static_cast<HAL_SPIPort>(port), bufferSize, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiFreeAuto
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiFreeAuto
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiFreeAuto";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  int32_t status = 0;
  HAL_FreeSPIAuto(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiStartAutoRate
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiStartAutoRate
  (JNIEnv* env, jclass, jint port, jdouble period)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiStartAutoRate";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  SPIJNI_LOG(logDEBUG) << "Period = " << period;
  int32_t status = 0;
  HAL_StartSPIAutoRate(static_cast<HAL_SPIPort>(port), period, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiStartAutoTrigger
 * Signature: (IIIZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiStartAutoTrigger
  (JNIEnv* env, jclass, jint port, jint digitalSourceHandle,
   jint analogTriggerType, jboolean triggerRising, jboolean triggerFalling)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiStartAutoTrigger";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  SPIJNI_LOG(logDEBUG) << "DigitalSourceHandle = " << digitalSourceHandle;
  SPIJNI_LOG(logDEBUG) << "AnalogTriggerType = " << analogTriggerType;
  SPIJNI_LOG(logDEBUG) << "TriggerRising = " << (jint)triggerRising;
  SPIJNI_LOG(logDEBUG) << "TriggerFalling = " << (jint)triggerFalling;
  int32_t status = 0;
  HAL_StartSPIAutoTrigger(static_cast<HAL_SPIPort>(port), digitalSourceHandle,
                          static_cast<HAL_AnalogTriggerType>(analogTriggerType),
                          triggerRising, triggerFalling, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiStopAuto
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiStopAuto
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiStopAuto";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  int32_t status = 0;
  HAL_StopSPIAuto(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiSetAutoTransmitData
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiSetAutoTransmitData
  (JNIEnv* env, jclass, jint port, jbyteArray dataToSend, jint zeroSize)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiSetAutoTransmitData";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  SPIJNI_LOG(logDEBUG) << "ZeroSize = " << zeroSize;
  JByteArrayRef jarr(env, dataToSend);
  int32_t status = 0;
  HAL_SetSPIAutoTransmitData(
      static_cast<HAL_SPIPort>(port),
      reinterpret_cast<const uint8_t*>(jarr.array().data()),
      jarr.array().size(), zeroSize, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiForceAutoRead
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiForceAutoRead
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiForceAutoRead";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  int32_t status = 0;
  HAL_ForceSPIAutoRead(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiReadAutoReceivedData
 * Signature: (ILjava/lang/Object;ID)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiReadAutoReceivedData__ILjava_nio_ByteBuffer_2ID
  (JNIEnv* env, jclass, jint port, jobject buffer, jint numToRead,
   jdouble timeout)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiReadAutoReceivedData";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  SPIJNI_LOG(logDEBUG) << "NumToRead = " << numToRead;
  SPIJNI_LOG(logDEBUG) << "Timeout = " << timeout;
  uint32_t* recvBuf =
      reinterpret_cast<uint32_t*>(env->GetDirectBufferAddress(buffer));
  int32_t status = 0;
  jint retval = HAL_ReadSPIAutoReceivedData(
      static_cast<HAL_SPIPort>(port), recvBuf, numToRead, timeout, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "Return = " << retval;
  CheckStatus(env, status);
  return retval;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiReadAutoReceivedData
 * Signature: (I[IID)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiReadAutoReceivedData__I_3IID
  (JNIEnv* env, jclass, jint port, jintArray buffer, jint numToRead,
   jdouble timeout)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiReadAutoReceivedData";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  SPIJNI_LOG(logDEBUG) << "NumToRead = " << numToRead;
  SPIJNI_LOG(logDEBUG) << "Timeout = " << timeout;
  wpi::SmallVector<uint32_t, 128> recvBuf;
  recvBuf.resize(numToRead);
  int32_t status = 0;
  jint retval =
      HAL_ReadSPIAutoReceivedData(static_cast<HAL_SPIPort>(port),
                                  recvBuf.data(), numToRead, timeout, &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "Return = " << retval;
  if (!CheckStatus(env, status)) return retval;
  if (numToRead > 0) {
    env->SetIntArrayRegion(buffer, 0, numToRead,
                           reinterpret_cast<const jint*>(recvBuf.data()));
  }
  return retval;
}

/*
 * Class:     edu_wpi_first_hal_SPIJNI
 * Method:    spiGetAutoDroppedCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SPIJNI_spiGetAutoDroppedCount
  (JNIEnv* env, jclass, jint port)
{
  SPIJNI_LOG(logDEBUG) << "Calling SPIJNI spiGetAutoDroppedCount";
  SPIJNI_LOG(logDEBUG) << "Port = " << port;
  int32_t status = 0;
  auto retval =
      HAL_GetSPIAutoDroppedCount(static_cast<HAL_SPIPort>(port), &status);
  SPIJNI_LOG(logDEBUG) << "Status = " << status;
  SPIJNI_LOG(logDEBUG) << "Return = " << retval;
  CheckStatus(env, status);
  return retval;
}

}  // extern "C"
