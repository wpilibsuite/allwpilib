#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_RelayJNI.h"

#include "HAL/Digital.hpp"

// set the logging level
TLogLevel relayJNILogLevel = logWARNING;

#define RELAYJNI_LOG(level) \
    if (level > relayJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    setRelayForward
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_RelayJNI_setRelayForward
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI setRelayForward";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	RELAYJNI_LOG(logDEBUG) << "Flag = " << (jint)value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	RELAYJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setRelayForward(*javaId, value, statusPtr);
	RELAYJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    setRelayReverse
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_RelayJNI_setRelayReverse
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI setRelayReverse";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	RELAYJNI_LOG(logDEBUG) << "Flag = " << (jint)value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	RELAYJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setRelayReverse(*javaId, value, statusPtr);
	RELAYJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    getRelayForward
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_RelayJNI_getRelayForward
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI getRelayForward";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	RELAYJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getRelayForward(*javaId, statusPtr);
	RELAYJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	RELAYJNI_LOG(logDEBUG) << "getRelayForwardResult = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    getRelayReverse
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_RelayJNI_getRelayReverse
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI getRelayReverse";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	RELAYJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getRelayReverse(*javaId, statusPtr);
	RELAYJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	RELAYJNI_LOG(logDEBUG) << "getRelayReverseResult = " << (jint)returnValue;
	return returnValue;
}
