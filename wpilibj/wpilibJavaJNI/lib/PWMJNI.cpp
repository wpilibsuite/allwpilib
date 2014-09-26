#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_PWMJNI.h"

#include "HAL/Digital.hpp"

// set the logging level
TLogLevel pwmJNILogLevel = logWARNING;

#define PWMJNI_LOG(level) \
    if (level > pwmJNILogLevel) ; \
    else Log().Get(level)


/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    allocatePWMChannel
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_allocatePWMChannel
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling DIOJNI allocatePWMChannel";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
    jbyte returnValue = allocatePWMChannel(*javaId, statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	PWMJNI_LOG(logDEBUG) << "allocatePWMChannelResult = " << (jint)returnValue;
    return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    freePWMChannel
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_freePWMChannel
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling DIOJNI freePWMChannel";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "Port Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	freePWMChannel(*javaId, statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWM
 * Signature: (Ljava/nio/ByteBuffer;SLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWM
  (JNIEnv * env, jclass, jobject id, jshort value, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "DigitalPort Ptr = " << *javaId;
	PWMJNI_LOG(logDEBUG) << "PWM Value = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setPWM( *javaId, value, statusPtr );
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    getPWM
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_getPWM
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jshort returnValue = getPWM( *javaId, statusPtr );
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	PWMJNI_LOG(logDEBUG) << "Value = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    latchPWMZero
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_latchPWMZero
  (JNIEnv * env, jclass, jobject id, jobject status)
{
  	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	latchPWMZero( *javaId, statusPtr );
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMPeriodScale
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMPeriodScale
  (JNIEnv * env, jclass, jobject id, jint value, jobject status  )
{
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "DigitalPort Ptr = " << *javaId;
	PWMJNI_LOG(logDEBUG) << "PeriodScale Value = " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setPWMPeriodScale( *javaId, value, statusPtr );
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    allocatePWM
 * Signature: (Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_allocatePWM
  (JNIEnv * env, jclass, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling PWMJNI allocatePWM";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	void** pwmPtr = (void**)new unsigned char[4];
	*pwmPtr =  allocatePWM(statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << *pwmPtr;
	return env->NewDirectByteBuffer( pwmPtr, 4);

}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    freePWM
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_freePWM
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling PWMJNI freePWM";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	freePWM(*javaId, statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMRate
 * Signature: (DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMRate
  (JNIEnv * env, jclass, jdouble value, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMRate";
	PWMJNI_LOG(logDEBUG) << "Rate= " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setPWMRate(value, statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMDutyCycle
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMDutyCycle
  (JNIEnv * env, jclass, jobject id, jdouble value, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMDutyCycle";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << *javaId;
	PWMJNI_LOG(logDEBUG) << "DutyCycle= " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setPWMDutyCycle(*javaId, value, statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMOutputChannel
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMOutputChannel
  (JNIEnv * env, jclass, jobject id, jint value, jobject status)
{
	PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMOutputChannel";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << *javaId;
	PWMJNI_LOG(logDEBUG) << "Pin= " << value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	PWMJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	setPWMOutputChannel(*javaId, (uint32_t) value, statusPtr);
	PWMJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}
