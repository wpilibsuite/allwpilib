#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_can_CANJNI.h"

#include "HAL/CAN.hpp"
#include "NetworkCommunication/JaguarCANDriver.h"

// set the logging level
TLogLevel canJNILogLevel = logWARNING;

#define CANJNI_LOG(level) \
    if (level > canJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_can_CANJNI
 * Method:    FRCNetworkCommunicationJaguarCANDriverSendMessage
 * Signature: (I[BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_can_CANJNI_FRCNetworkCommunicationJaguarCANDriverSendMessage
  (JNIEnv *env, jclass, jint messageID, jobject data, jobject status)
{
	CANJNI_LOG(logDEBUG) << "Calling CANJNI JaguarCANDriverSendMessage";
	CANJNI_LOG(logDEBUG) << "MessageID = " << std::hex << messageID;
	jbyte * dataPtr = NULL;
	jlong dataCapacity = 0;

	if(data != 0)
	{
		dataPtr = (jbyte*)env->GetDirectBufferAddress(data);
		dataCapacity = env->GetDirectBufferCapacity(data);
	}
	CANJNI_LOG(logDEBUG) << "MessageSize = " << dataCapacity;

	if( logDEBUG <= canJNILogLevel )
	{
		std::ostringstream str;
		str << std::setfill('0') << std::hex;
		for( int dataIndex = 0; dataIndex < dataCapacity; dataIndex++)
		{
			str << std::setw(2) << static_cast<unsigned int>(dataPtr[dataIndex]) << " ";
		}
		Log().Get(logDEBUG) << "MSG: " << str.str();
	}

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//CANJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	FRC_NetworkCommunication_JaguarCANDriver_sendMessage((uint32_t) messageID, (const uint8_t*)dataPtr, (uint8_t)dataCapacity, statusPtr);
	CANJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_can_CANJNI
 * Method:    FRCNetworkCommunicationJaguarCANDriverReceiveMessage
 * Signature: (Ljava/nio/IntBuffer;Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_can_CANJNI_FRCNetworkCommunicationJaguarCANDriverReceiveMessage
  (JNIEnv * env, jclass, jobject messageID, jint timeout, jobject status)
{
	CANJNI_LOG(logDEBUG) << "Calling CANJNI JaguarCANDriverReceiveMessage";
	jint * messageIDPtr = (jint*)env->GetDirectBufferAddress(messageID);
	CANJNI_LOG(logDEBUG) << "MessageID In = " << std::hex << *messageIDPtr;
	CANJNI_LOG(logDEBUG) << "Timeout = " <<  timeout;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//CANJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	uint8_t dataSize = 8;
	jbyte* dataPtr = new jbyte[8];
	//CANJNI_LOG(logDEBUG) << "Original MessageSize = " << (jint)dataSize;
	//CANJNI_LOG(logDEBUG) << "Original MessagePtr = " << (jint*)dataPtr;
	FRC_NetworkCommunication_JaguarCANDriver_receiveMessage((uint32_t*)messageIDPtr, (uint8_t*)dataPtr, &dataSize, timeout, statusPtr);

	//CANJNI_LOG(logDEBUG) << "MessageID Out = " << std::hex << *messageIDPtr;
	CANJNI_LOG(logDEBUG) << "MessageSize = " << (jint)dataSize;
	CANJNI_LOG(logDEBUG) << "MessagePtr = " << (jint*)dataPtr;

	if( logDEBUG <= canJNILogLevel )
	{
		std::ostringstream str;
		str << std::setfill('0') << std::hex;
		for( int dataIndex = 0; dataIndex < dataSize; dataIndex++)
		{
			str << std::setw(2) << static_cast<unsigned int>(dataPtr[dataIndex]) << " ";
		}
		Log().Get(logDEBUG) << "MSG: " << str.str();
	}

	CANJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

	if( dataSize > 0 )
	{
		return env->NewDirectByteBuffer( dataPtr, dataSize);
	}
	else
	{
		return 0;
	}
}

