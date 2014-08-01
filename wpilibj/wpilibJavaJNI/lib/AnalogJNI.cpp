#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_AnalogJNI.h"

#include "HAL/Analog.hpp"

// set the logging level
TLogLevel analogJNILogLevel = logWARNING;

#define ANALOGJNI_LOG(level) \
	if (level > analogJNILogLevel) ; \
	else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogInputPort
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogInputPort
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	void** analogPtr = (void**)new unsigned char[4];
	*analogPtr = initializeAnalogInputPort(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *analogPtr;
	return env->NewDirectByteBuffer( analogPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogOutputPort
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogOutputPort
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	void** analogPtr = (void**)new unsigned char[4];
	*analogPtr = initializeAnalogOutputPort(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *analogPtr;
	return env->NewDirectByteBuffer( analogPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    checkAnalogModule
 * Signature: (B)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_checkAnalogModule
  (JNIEnv *, jclass, jbyte value)
{
	//ANALOGJNI_LOG(logDEBUG) << "Module = " << (jint)value;
	jbyte returnValue = checkAnalogModule( value );
	//ANALOGJNI_LOG(logDEBUG) << "checkAnalogModuleResult = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    checkAnalogInputChannel
 * Signature: (I)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_checkAnalogInputChannel
  (JNIEnv *, jclass, jint value)
{
	//ANALOGJNI_LOG(logDEBUG) << "Channel = " << value;
	jbyte returnValue = checkAnalogInputChannel( value );
	//ANALOGJNI_LOG(logDEBUG) << "checkAnalogChannelResult = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    checkAnalogOutputChannel
 * Signature: (I)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_checkAnalogOutputChannel
  (JNIEnv *, jclass, jint value)
{
	//ANALOGJNI_LOG(logDEBUG) << "Channel = " << value;
	jbyte returnValue = checkAnalogOutputChannel( value );
	//ANALOGJNI_LOG(logDEBUG) << "checkAnalogChannelResult = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogOutput
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogOutput
  (JNIEnv * env, jclass, jobject id, jdouble voltage, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setAnalogOutput(*javaId, voltage, statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOutput
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOutput
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getAnalogOutput(*javaId, statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogSampleRate
 * Signature: (DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogSampleRate
  (JNIEnv * env, jclass, jdouble value, jobject status)
{
	ANALOGJNI_LOG(logDEBUG) << "SampleRate = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setAnalogSampleRate( value, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogSampleRate
 * Signature: (Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogSampleRate
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	double returnValue = getAnalogSampleRate( statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "SampleRate = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogAverageBits
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogAverageBits
  (JNIEnv * env, jclass, jobject id, jint value, jobject status)
{
	ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << value;
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setAnalogAverageBits( *javaId, value, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageBits
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageBits
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jint returnValue = getAnalogAverageBits( *javaId, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogOversampleBits
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogOversampleBits
  (JNIEnv * env, jclass, jobject id, jint value, jobject status)
{
	ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << value;
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setAnalogOversampleBits( *javaId, value, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOversampleBits
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOversampleBits
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jint returnValue = getAnalogOversampleBits( *javaId, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogValue
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogValue
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jshort returnValue = getAnalogValue( *javaId, statusPtr );
	//ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	//ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageValue
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageValue
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jint returnValue = getAnalogAverageValue( *javaId, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AverageValue = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogVoltsToValue
  (JNIEnv * env, jclass, jobject id, jdouble voltageValue, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	ANALOGJNI_LOG(logDEBUG) << "VoltageValue = " << voltageValue;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jint returnValue = getAnalogVoltsToValue( *javaId, voltageValue, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogVoltage
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogVoltage
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	//ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jdouble returnValue = getAnalogVoltage( *javaId, statusPtr );
	//ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	//ANALOGJNI_LOG(logDEBUG) << "Voltage = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageVoltage
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jdouble returnValue = getAnalogAverageVoltage( *javaId, statusPtr );
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AverageVoltage = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogLSBWeight
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogLSBWeight
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	jint returnValue = getAnalogLSBWeight(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AnalogLSBWeight = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOffset
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOffset
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	jint returnValue = getAnalogOffset(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    isAccumulatorChannel
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_isAccumulatorChannel
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	ANALOGJNI_LOG(logDEBUG) << "isAccumulatorChannel";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	// isAccumulaotrChanel returns a boolean
	char vOut = isAccumulatorChannel(*javaId, statusPtr) ? 1 : 0;
	//The C++ equivalent of a jbyte is a char
	jbyte returnValue = vOut;
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initAccumulator
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initAccumulator
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	initAccumulator(*javaId, statusPtr);

	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    resetAccumulator
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_resetAccumulator
	(JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	resetAccumulator(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAccumulatorCenter
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAccumulatorCenter
  (JNIEnv * env, jclass, jobject id, jint center, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	setAccumulatorCenter(*javaId, center, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAccumulatorDeadband
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAccumulatorDeadband
  (JNIEnv * env, jclass, jobject id, jint deadband, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	setAccumulatorDeadband(*javaId, deadband, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorValue
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorValue
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	jlong returnValue = getAccumulatorValue(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AccumulatorValue = " << returnValue;

	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorCount
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorCount
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	jint returnValue = getAccumulatorCount(*javaId, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AccumulatorCount = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorOutput
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/LongBuffer;Ljava/nio/IntBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorOutput
  (JNIEnv * env, jclass, jobject id, jobject value, jobject count, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);

	jlong * valuePtr = (jlong*)env->GetDirectBufferAddress(value);
	uint32_t * countPtr = (uint32_t*)env->GetDirectBufferAddress(count);

	getAccumulatorOutput(*javaId, valuePtr, countPtr, statusPtr);

	ANALOGJNI_LOG(logDEBUG) << "Value = " << *valuePtr;
	ANALOGJNI_LOG(logDEBUG) << "Count = " << *countPtr;
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogTrigger
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogTrigger
  (JNIEnv * env, jclass, jobject id, jobject index, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;

	jint * indexPtr = (jint*)env->GetDirectBufferAddress(index);
	ANALOGJNI_LOG(logDEBUG) << "Index Ptr = " << indexPtr;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	void ** analogTriggerPtr = new void *;
	*analogTriggerPtr = initializeAnalogTrigger(*javaId, (uint32_t *)indexPtr, statusPtr);
	ANALOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	ANALOGJNI_LOG(logDEBUG) << "AnalogTrigger Ptr = " << *analogTriggerPtr;

	return env->NewDirectByteBuffer(analogTriggerPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    cleanAnalogTrigger
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_cleanAnalogTrigger
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	cleanAnalogTrigger( *javaId, statusPtr );

	delete javaId;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsRaw
 * Signature: (Ljava/nio/ByteBuffer;IILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerLimitsRaw
  (JNIEnv * env, jclass, jobject id, jint lower, jint upper, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	setAnalogTriggerLimitsRaw( *javaId, lower, upper, statusPtr );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsVoltage
 * Signature: (Ljava/nio/ByteBuffer;DDLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerLimitsVoltage
  (JNIEnv * env, jclass, jobject id, jdouble lower, jdouble upper, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	setAnalogTriggerLimitsVoltage( *javaId, lower, upper, statusPtr );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerAveraged
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerAveraged
  (JNIEnv * env, jclass, jobject id, jbyte averaged, jobject status){
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	setAnalogTriggerAveraged( *javaId, averaged, statusPtr );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerFiltered
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerFiltered
   (JNIEnv * env, jclass, jobject id, jbyte filtered, jobject status){
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	setAnalogTriggerFiltered( *javaId, filtered, statusPtr );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerInWindow
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerInWindow
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	return getAnalogTriggerInWindow( *javaId, statusPtr );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerTriggerState
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerTriggerState
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	return getAnalogTriggerTriggerState( *javaId, statusPtr );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerOutput
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerOutput
  (JNIEnv * env, jclass, jobject id, jint type, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	ANALOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	return getAnalogTriggerOutput( *javaId, (AnalogTriggerType)type, statusPtr )? 1 : 0;
}
