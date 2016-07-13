/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_can_CANJNI.h"

#include "FRC_NetworkCommunication/CANSessionMux.h"
#include "HAL/CAN.h"
#include "HALUtil.h"

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
 * Class:     edu_wpi_first_wpilibj_can_CANJNI
 * Method:    FRCNetCommCANSessionMuxSendMessage
 * Signature: (ILjava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_can_CANJNI_FRCNetCommCANSessionMuxSendMessage(
    JNIEnv *env, jclass, jint messageID, jobject data, jint periodMs) {
  CANJNI_LOG(logDEBUG)
      << "Calling CANJNI FRCNetCommCANSessionMuxSendMessage";

  uint8_t *dataBuffer =
      (uint8_t *)(data ? env->GetDirectBufferAddress(data) : 0);
  uint8_t dataSize = (uint8_t)(data ? env->GetDirectBufferCapacity(data) : 0);

  CANJNI_LOG(logDEBUG) << "Message ID " << std::hex << messageID;

  if (logDEBUG <= canJNILogLevel) {
    if (dataBuffer) {
      std::ostringstream str;
      str << std::setfill('0') << std::hex;
      for (int i = 0; i < dataSize; i++) {
        str << std::setw(2) << (int)dataBuffer[i] << ' ';
      }

      Log().Get(logDEBUG) << "Data: " << str.str();
    } else {
      CANJNI_LOG(logDEBUG) << "Data: null";
    }
  }

  CANJNI_LOG(logDEBUG) << "Period: " << periodMs;

  int32_t status = 0;
  FRC_NetworkCommunication_CANSessionMux_sendMessage(
      messageID, dataBuffer, dataSize, periodMs, &status);

  CANJNI_LOG(logDEBUG) << "Status: " << status;
  CheckCANStatus(env, status, messageID);
}

static uint8_t buffer[8];

/*
 * Class:     edu_wpi_first_wpilibj_can_CANJNI
 * Method:    FRCNetCommCANSessionMuxReceiveMessage
 * Signature: (Ljava/nio/IntBuffer;ILjava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_wpilibj_can_CANJNI_FRCNetCommCANSessionMuxReceiveMessage(
    JNIEnv *env, jclass, jobject messageID, jint messageIDMask,
    jobject timeStamp) {
  CANJNI_LOG(logDEBUG)
      << "Calling CANJNI FRCNetCommCANSessionMuxReceiveMessage";

  uint32_t *messageIDPtr = (uint32_t *)env->GetDirectBufferAddress(messageID);
  uint32_t *timeStampPtr = (uint32_t *)env->GetDirectBufferAddress(timeStamp);

  uint8_t dataSize = 0;

  int32_t status = 0;
  FRC_NetworkCommunication_CANSessionMux_receiveMessage(
      messageIDPtr, messageIDMask, buffer, &dataSize, timeStampPtr, &status);

  CANJNI_LOG(logDEBUG) << "Message ID " << std::hex << *messageIDPtr;

  if (logDEBUG <= canJNILogLevel) {
    std::ostringstream str;
    str << std::setfill('0') << std::hex;
    for (int i = 0; i < dataSize; i++) {
      str << std::setw(2) << (int)buffer[i] << ' ';
    }

    Log().Get(logDEBUG) << "Data: " << str.str();
  }

  CANJNI_LOG(logDEBUG) << "Timestamp: " << *timeStampPtr;
  CANJNI_LOG(logDEBUG) << "Status: " << status;

  if (!CheckCANStatus(env, status, *messageIDPtr)) return nullptr;
  return env->NewDirectByteBuffer(buffer, dataSize);
}

}  // extern "C"
