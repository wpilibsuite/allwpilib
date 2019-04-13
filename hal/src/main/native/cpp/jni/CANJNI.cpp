/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include <wpi/SmallString.h>
#include <wpi/jni_util.h>
#include <wpi/raw_ostream.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_can_CANJNI.h"
#include "hal/CAN.h"
#include "hal/cpp/Log.h"

using namespace frc;
using namespace wpi::java;

// set the logging level
// TLogLevel canJNILogLevel = logDEBUG;
TLogLevel canJNILogLevel = logERROR;

#define CANJNI_LOG(level)     \
  if (level > canJNILogLevel) \
    ;                         \
  else                        \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    FRCNetCommCANSessionMuxSendMessage
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_can_CANJNI_FRCNetCommCANSessionMuxSendMessage
  (JNIEnv* env, jclass, jint messageID, jbyteArray data, jint periodMs)
{
  CANJNI_LOG(logDEBUG) << "Calling CANJNI FRCNetCommCANSessionMuxSendMessage";

  JByteArrayRef dataArray{env, data};

  const uint8_t* dataBuffer =
      reinterpret_cast<const uint8_t*>(dataArray.array().data());
  uint8_t dataSize = dataArray.array().size();

  CANJNI_LOG(logDEBUG) << "Message ID ";
  CANJNI_LOG(logDEBUG).write_hex(messageID);

  if (logDEBUG <= canJNILogLevel) {
    if (dataBuffer) {
      wpi::SmallString<128> buf;
      wpi::raw_svector_ostream str(buf);
      for (int32_t i = 0; i < dataSize; i++) {
        str.write_hex(dataBuffer[i]) << ' ';
      }

      Log().Get(logDEBUG) << "Data: " << str.str();
    } else {
      CANJNI_LOG(logDEBUG) << "Data: null";
    }
  }

  CANJNI_LOG(logDEBUG) << "Period: " << periodMs;

  int32_t status = 0;
  HAL_CAN_SendMessage(messageID, dataBuffer, dataSize, periodMs, &status);

  CANJNI_LOG(logDEBUG) << "Status: " << status;
  CheckCANStatus(env, status, messageID);
}

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    FRCNetCommCANSessionMuxReceiveMessage
 * Signature: (Ljava/lang/Object;ILjava/lang/Object;)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_hal_can_CANJNI_FRCNetCommCANSessionMuxReceiveMessage
  (JNIEnv* env, jclass, jobject messageID, jint messageIDMask,
   jobject timeStamp)
{
  CANJNI_LOG(logDEBUG)
      << "Calling CANJNI FRCNetCommCANSessionMuxReceiveMessage";

  uint32_t* messageIDPtr =
      reinterpret_cast<uint32_t*>(env->GetDirectBufferAddress(messageID));
  uint32_t* timeStampPtr =
      reinterpret_cast<uint32_t*>(env->GetDirectBufferAddress(timeStamp));

  uint8_t dataSize = 0;
  uint8_t buffer[8];

  int32_t status = 0;
  HAL_CAN_ReceiveMessage(messageIDPtr, messageIDMask, buffer, &dataSize,
                         timeStampPtr, &status);

  CANJNI_LOG(logDEBUG) << "Message ID ";
  CANJNI_LOG(logDEBUG).write_hex(*messageIDPtr);

  if (logDEBUG <= canJNILogLevel) {
    wpi::SmallString<128> buf;
    wpi::raw_svector_ostream str(buf);

    for (int32_t i = 0; i < dataSize; i++) {
      // Pad one-digit data with a zero
      if (buffer[i] <= 16) {
        str << '0';
      }

      str.write_hex(buffer[i]) << ' ';
    }

    Log().Get(logDEBUG) << "Data: " << str.str();
  }

  CANJNI_LOG(logDEBUG) << "Timestamp: " << *timeStampPtr;
  CANJNI_LOG(logDEBUG) << "Status: " << status;

  if (!CheckCANStatus(env, status, *messageIDPtr)) return nullptr;
  return MakeJByteArray(env,
                        wpi::StringRef{reinterpret_cast<const char*>(buffer),
                                       static_cast<size_t>(dataSize)});
}

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    GetCANStatus
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_can_CANJNI_GetCANStatus
  (JNIEnv* env, jclass, jobject canStatus)
{
  CANJNI_LOG(logDEBUG) << "Calling CANJNI HAL_CAN_GetCANStatus";

  float percentBusUtilization = 0;
  uint32_t busOffCount = 0;
  uint32_t txFullCount = 0;
  uint32_t receiveErrorCount = 0;
  uint32_t transmitErrorCount = 0;
  int32_t status = 0;
  HAL_CAN_GetCANStatus(&percentBusUtilization, &busOffCount, &txFullCount,
                       &receiveErrorCount, &transmitErrorCount, &status);

  if (!CheckStatus(env, status)) return;

  SetCanStatusObject(env, canStatus, percentBusUtilization, busOffCount,
                     txFullCount, receiveErrorCount, transmitErrorCount);
}

}  // extern "C"
