#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_DIOJNI.h"

#include "HAL/Digital.hpp"

// set the logging level
TLogLevel dioJNILogLevel = logWARNING;

#define DIOJNI_LOG(level) \
    if (level > dioJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    initializeDigitalPort
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_initializeDigitalPort
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI initializeDigitalPort";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	void** dioPtr = (void**)new unsigned char[4];
	*dioPtr = initializeDigitalPort(*javaId, statusPtr);
	DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	DIOJNI_LOG(logDEBUG) << "DIO Ptr = " << *dioPtr;
	return env->NewDirectByteBuffer( dioPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    allocateDIO
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_allocateDIO
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI allocateDIO";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
    jbyte returnValue = allocateDIO(*javaId, value, statusPtr);
	DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	DIOJNI_LOG(logDEBUG) << "allocateDIOResult = " << (jint)returnValue;
    return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    freeDIO
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_freeDIO
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI freeDIO";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	freeDIO(*javaId, statusPtr);
	DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    setDIO
 * Signature: (Ljava/nio/ByteBuffer;SLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_setDIO
  (JNIEnv *env, jclass, jobject id, jshort value, jobject status)
{
	//DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDIO";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	//DIOJNI_LOG(logDEBUG) << "Value = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setDIO(*javaId, value, statusPtr);
	//DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    getDIO
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_getDIO
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	//DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getDIO";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getDIO(*javaId, statusPtr);
	//DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	//DIOJNI_LOG(logDEBUG) << "getDIOResult = " << (jint)returnValue;
	return returnValue;

}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    getDIODirection
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_getDIODirection
  (JNIEnv *env, jclass, jobject id, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getDIODirection (RR upd)";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getDIODirection(*javaId, statusPtr);
	//DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	DIOJNI_LOG(logDEBUG) << "getDIODirectionResult = " << (jbyte)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    pulse
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_pulse
  (JNIEnv *env, jclass, jobject id, jdouble value, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI pulse (RR upd)";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	//DIOJNI_LOG(logDEBUG) << "Value = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	pulse(*javaId, value, statusPtr);
	DIOJNI_LOG(logDEBUG) << "Did it work? Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    isPulsing
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_isPulsing
  (JNIEnv *env, jclass, jobject id, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI isPulsing (RR upd)";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//DIOJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//DIOJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = isPulsing(*javaId, statusPtr);
	//DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	DIOJNI_LOG(logDEBUG) << "isPulsingResult = " << (jbyte)returnValue;
	return returnValue;


}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    isAnyPulsing
 * Signature: (Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_isAnyPulsing
  (JNIEnv *env, jclass, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI isAnyPulsing (RR upd)";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte returnValue = isAnyPulsing( statusPtr );
	//DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	DIOJNI_LOG(logDEBUG) << "isAnyPulsingResult = " << (jbyte)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    getLoopTiming
 * Signature: (Ljava/nio/IntBuffer;)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_getLoopTiming
  (JNIEnv * env, jclass, jobject status)
{
	DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getLoopTimeing";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jshort returnValue = getLoopTiming( statusPtr );
	DIOJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	DIOJNI_LOG(logDEBUG) << "LoopTiming = " << returnValue;
	return returnValue;

}
