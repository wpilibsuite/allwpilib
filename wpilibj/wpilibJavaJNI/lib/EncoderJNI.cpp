#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_EncoderJNI.h"

#include "HAL/Digital.hpp"

// set the logging level
TLogLevel encoderJNILogLevel = logWARNING;

#define ENCODERJNI_LOG(level) \
    if (level > encoderJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    initializeEncoder
 * Signature: (BIBBIBBLjava/nio/IntBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_initializeEncoder
  (JNIEnv * env, jclass, jbyte port_a_module, jint port_a_pin, jbyte port_a_analog_trigger, jbyte port_b_module, jint port_b_pin, jbyte port_b_analog_trigger, jbyte reverseDirection, jobject index, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI initializeEncoder";
	ENCODERJNI_LOG(logDEBUG) << "Module A = " << (jint)port_a_module;
	ENCODERJNI_LOG(logDEBUG) << "Pin A = " << port_a_pin;
	ENCODERJNI_LOG(logDEBUG) << "Analog Trigger A = " << (jint)port_a_analog_trigger;
	ENCODERJNI_LOG(logDEBUG) << "Module B = " << (jint)port_b_module;
	ENCODERJNI_LOG(logDEBUG) << "Pin B = " << port_b_pin;
	ENCODERJNI_LOG(logDEBUG) << "Analog Trigger B = " << (jint)port_b_analog_trigger;
	ENCODERJNI_LOG(logDEBUG) << "Reverse direction = " << (jint)reverseDirection;
	jint * indexPtr = (jint*)env->GetDirectBufferAddress(index);
	ENCODERJNI_LOG(logDEBUG) << "Index Ptr = " << indexPtr;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	void** encoderPtr = (void**)new unsigned char[4];
	*encoderPtr = initializeEncoder(port_a_module, port_a_pin, port_a_analog_trigger,
							  port_b_module, port_b_pin, port_b_analog_trigger,
							  reverseDirection, indexPtr, statusPtr);

	ENCODERJNI_LOG(logDEBUG) << "Index = " << *indexPtr;
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ENCODERJNI_LOG(logDEBUG) << "ENCODER Ptr = " << *encoderPtr;
	return env->NewDirectByteBuffer( encoderPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    freeEncoder
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_freeEncoder
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI freeEncoder";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	freeEncoder(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    resetEncoder
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_resetEncoder
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI resetEncoder";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	resetEncoder(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoder
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoder
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoder";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jint returnValue = getEncoder(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ENCODERJNI_LOG(logDEBUG) << "getEncoderResult = " <<  returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderPeriod
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderPeriod
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderPeriod";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	double returnValue = getEncoderPeriod(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ENCODERJNI_LOG(logDEBUG) << "getEncoderPeriodResult = " <<  returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderMaxPeriod
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderMaxPeriod
  (JNIEnv * env, jclass, jobject id, jdouble value, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderMaxPeriod";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setEncoderMaxPeriod(*javaId, value, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderStopped
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderStopped
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderStopped";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getEncoderStopped(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ENCODERJNI_LOG(logDEBUG) << "getEncoderStoppedResult = " <<  returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderDirection
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderDirection
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderDirection";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getEncoderDirection(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ENCODERJNI_LOG(logDEBUG) << "getEncoderDirectionResult = " <<  returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderReverseDirection
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderReverseDirection
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderReverseDirection";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setEncoderReverseDirection(*javaId, value, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderSamplesToAverage
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderSamplesToAverage
  (JNIEnv * env, jclass, jobject id, jint value, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderSamplesToAverage";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setEncoderSamplesToAverage(*javaId, value, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderSamplesToAverage
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderSamplesToAverage
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jint returnValue = getEncoderSamplesToAverage(*javaId, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = " <<  returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderIndexSource
 * Signature: (Ljava/nio/ByteBuffer;IZZZLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderIndexSource
  (JNIEnv * env, jclass, jobject id, jint pin, jboolean analogTrigger, jboolean activeHigh, jboolean edgeSensitive, jobject status)
{
	ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderIndexSource";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ENCODERJNI_LOG(logDEBUG) << "Pin = " << pin;
	ENCODERJNI_LOG(logDEBUG) << "Analog Trigger = " << (analogTrigger?"true":"false");
	ENCODERJNI_LOG(logDEBUG) << "Active High = " << (activeHigh?"true":"false");
	ENCODERJNI_LOG(logDEBUG) << "Edge Sensitive = " << (edgeSensitive?"true":"false");
	ENCODERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setEncoderIndexSource(*javaId, pin, analogTrigger, activeHigh, edgeSensitive, statusPtr);
	ENCODERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}
