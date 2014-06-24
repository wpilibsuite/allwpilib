#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_can_CANJNI.h"

#include "HAL/CAN.hpp"
#include "NetworkCommunication/CANSessionMux.h"

// set the logging level
//TLogLevel canJNILogLevel = logDEBUG;
TLogLevel canJNILogLevel = logERROR;

#define CANJNI_LOG(level) \
    if (level > canJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_can_CANJNI
 * Method:    FRCNetworkCommunicationCANSessionMuxSendMessage
 * Signature: (ILjava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_can_CANJNI_FRCNetworkCommunicationCANSessionMuxSendMessage
  (JNIEnv * env, jclass, jint messageID, jobject data, jint periodMs, jobject status)
{
    CANJNI_LOG(logDEBUG) << "Calling CANJNI FRCNetworkCommunicationCANSessionMuxSendMessage";

    uint8_t *dataBuffer = (uint8_t *)(data? env->GetDirectBufferAddress(data) : 0);
    uint8_t dataSize = (uint8_t)(data? env->GetDirectBufferCapacity(data) : 0);
    int32_t *statusPtr = (int32_t *)env->GetDirectBufferAddress(status);

    CANJNI_LOG(logDEBUG) << "Message ID " << std::hex << messageID;

    if(logDEBUG <= canJNILogLevel)
    {
        if(dataBuffer)
        {
            std::ostringstream str;
            str << std::setfill('0') << std::hex;
            for(int i = 0; i < dataSize; i++)
            {
                str << std::setw(2) << (int)dataBuffer[i] << ' ';
            }

            Log().Get(logDEBUG) << "Data: " << str.str();
        }
        else
        {
            CANJNI_LOG(logDEBUG) << "Data: null";
        }
    }

    CANJNI_LOG(logDEBUG) << "Period: " << periodMs;

    FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, dataBuffer, dataSize, periodMs, statusPtr);

    CANJNI_LOG(logDEBUG) << "Status: " << *statusPtr;
}

static uint8_t buffer[8];

/*
 * Class:     edu_wpi_first_wpilibj_can_CANJNI
 * Method:    FRCNetworkCommunicationCANSessionMuxReceiveMessage
 * Signature: (Ljava/nio/IntBuffer;ILjava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_can_CANJNI_FRCNetworkCommunicationCANSessionMuxReceiveMessage
	(JNIEnv * env, jclass, jobject messageID, jint messageIDMask, jobject timeStamp, jobject status)
{
    CANJNI_LOG(logDEBUG) << "Calling CANJNI FRCNetworkCommunicationCANSessionMuxReceiveMessage";

    uint32_t *messageIDPtr = (uint32_t *)env->GetDirectBufferAddress(messageID);
    uint32_t *timeStampPtr = (uint32_t *)env->GetDirectBufferAddress(timeStamp);
    int32_t *statusPtr = (int32_t *)env->GetDirectBufferAddress(status);

    uint8_t dataSize = 0;

    FRC_NetworkCommunication_CANSessionMux_receiveMessage(messageIDPtr, messageIDMask, buffer, &dataSize, timeStampPtr, statusPtr);

    CANJNI_LOG(logDEBUG) << "Message ID " << std::hex << *messageIDPtr;

    if(logDEBUG <= canJNILogLevel)
    {
        std::ostringstream str;
        str << std::setfill('0') << std::hex;
        for(int i = 0; i < dataSize; i++)
        {
            str << std::setw(2) << (int)buffer[i] << ' ';
        }

        Log().Get(logDEBUG) << "Data: " << str.str();
    }

    CANJNI_LOG(logDEBUG) << "Timestamp: " << *timeStampPtr;
    CANJNI_LOG(logDEBUG) << "Status: " << *statusPtr;

    return env->NewDirectByteBuffer(buffer, dataSize);
}
