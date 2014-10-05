#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_CounterJNI.h"

#include "HAL/Digital.hpp"

// set the logging level
TLogLevel counterJNILogLevel = logWARNING;

#define COUNTERJNI_LOG(level) \
    if (level > counterJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    initializeCounter
 * Signature: (ILjava/nio/IntBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_initializeCounter
  (JNIEnv * env, jclass, jint mode, jobject index, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI initializeCounter";
	COUNTERJNI_LOG(logDEBUG) << "Mode = " << mode;
	jint * indexPtr = (jint*)env->GetDirectBufferAddress(index);
	COUNTERJNI_LOG(logDEBUG) << "Index Ptr = " << indexPtr;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	void** counterPtr = (void**)new unsigned char[4];
	*counterPtr = initializeCounter((Mode)mode, (uint32_t*)indexPtr, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Index = " << *indexPtr;
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	COUNTERJNI_LOG(logDEBUG) << "COUNTER Ptr = " << *counterPtr;
	return env->NewDirectByteBuffer( counterPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    freeCounter
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_freeCounter
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI freeCounter";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	freeCounter(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterAverageSize
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterAverageSize
  (JNIEnv * env, jclass, jobject id, jint value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterAverageSize";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "AverageSize = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterAverageSize(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpSource
 * Signature: (Ljava/nio/ByteBuffer;BIBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpSource
  (JNIEnv * env, jclass, jobject id, jint pin, jbyte analogTrigger, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpSource";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "Pin = " << pin;
	COUNTERJNI_LOG(logDEBUG) << "AnalogTrigger = " << (jint)analogTrigger;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterUpSource(*javaId, pin, analogTrigger, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpSourceEdge
 * Signature: (Ljava/nio/ByteBuffer;BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpSourceEdge
  (JNIEnv * env, jclass, jobject id, jbyte valueRise, jbyte valueFall, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpSourceEdge";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "Rise = " << (jint)valueRise;
	COUNTERJNI_LOG(logDEBUG) << "Fall = " << (jint)valueFall;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterUpSourceEdge(*javaId, valueRise, valueFall, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    clearCounterUpSource
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_clearCounterUpSource
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI clearCounterUpSource";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	clearCounterUpSource(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterDownSource
 * Signature: (Ljava/nio/ByteBuffer;BIBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterDownSource
  (JNIEnv * env, jclass, jobject id, jint pin, jbyte analogTrigger, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterDownSource";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "Pin = " << pin;
	COUNTERJNI_LOG(logDEBUG) << "AnalogTrigger = " << (jint)analogTrigger;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterDownSource(*javaId, pin, analogTrigger, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterDownSourceEdge
 * Signature: (Ljava/nio/ByteBuffer;BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterDownSourceEdge
  (JNIEnv * env, jclass, jobject id, jbyte valueRise, jbyte valueFall, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterDownSourceEdge";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "Rise = " << (jint)valueRise;
	COUNTERJNI_LOG(logDEBUG) << "Fall = " << (jint)valueFall;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterDownSourceEdge(*javaId, valueRise, valueFall, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    clearCounterDownSource
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_clearCounterDownSource
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI clearCounterDownSource";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	clearCounterDownSource(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpDownMode
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpDownMode
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpDownMode";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterUpDownMode(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterExternalDirectionMode
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterExternalDirectionMode
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterExternalDirectionMode";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterExternalDirectionMode(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterSemiPeriodMode
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterSemiPeriodMode
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterSemiPeriodMode";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "SemiPeriodMode = " << (jint)value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterSemiPeriodMode(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterPulseLengthMode
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterPulseLengthMode
  (JNIEnv * env, jclass, jobject id, jdouble value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterPulseLengthMode";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "PulseLengthMode = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterPulseLengthMode(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterSamplesToAverage
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterSamplesToAverage
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterSamplesToAverage";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jint returnValue = getCounterSamplesToAverage(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	COUNTERJNI_LOG(logDEBUG) << "getCounterSamplesToAverageResult = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterSamplesToAverage
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterSamplesToAverage
  (JNIEnv * env, jclass, jobject id, jint value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterSamplesToAverage";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "SamplesToAverage = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterSamplesToAverage(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    resetCounter
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_resetCounter
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI resetCounter";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	resetCounter(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounter
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounter
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	//COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounter";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	//COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jint returnValue = getCounter(*javaId, statusPtr);
	//COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	//COUNTERJNI_LOG(logDEBUG) << "getCounterResult = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterPeriod
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterPeriod
  (JNIEnv *env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterPeriod";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jdouble returnValue = getCounterPeriod(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	COUNTERJNI_LOG(logDEBUG) << "getCounterPeriodResult = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterMaxPeriod
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterMaxPeriod
  (JNIEnv * env, jclass, jobject id, jdouble value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterMaxPeriod";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "MaxPeriod = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterMaxPeriod(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpdateWhenEmpty
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpdateWhenEmpty
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterMaxPeriod";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "UpdateWhenEmpty = " << (jint)value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterUpdateWhenEmpty(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterStopped
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterStopped
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterStopped";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getCounterStopped(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	COUNTERJNI_LOG(logDEBUG) << "getCounterStoppedResult = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterDirection
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterDirection
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterDirection";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jbyte returnValue = getCounterDirection(*javaId, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	COUNTERJNI_LOG(logDEBUG) << "getCounterDirectionResult = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterReverseDirection
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterReverseDirection
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterReverseDirection";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << *javaId;
	COUNTERJNI_LOG(logDEBUG) << "ReverseDirection = " << (jint)value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	COUNTERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setCounterReverseDirection(*javaId, value, statusPtr);
	COUNTERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}
