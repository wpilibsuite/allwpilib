#include <jni.h>
#include "edu_wpi_first_wpilibj_hal_PowerJNI.h"
#include "HAL/Power.hpp"

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getVinVoltage
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getVinVoltage
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getVinVoltage(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getVinCurrent
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getVinCurrent
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getVinCurrent(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserVoltage6V
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserVoltage6V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserVoltage6V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrent6V
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrent6V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserCurrent6V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserActive6V
 * Signature: (Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserActive6V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserActive6V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrentFaults6V
 * Signature: (Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrentFaults6V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserCurrentFaults6V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserVoltage5V
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserVoltage5V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserVoltage5V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrent5V
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrent5V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserCurrent5V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserActive5V
 * Signature: (Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserActive5V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserActive5V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrentFaults5V
 * Signature: (Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrentFaults5V
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserCurrentFaults5V(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserVoltage3V3
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserVoltage3V3
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserVoltage3V3(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrent3V3
 * Signature: (Ljava/nio/IntBuffer;)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrent3V3
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserCurrent3V3(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserActive3V3
 * Signature: (Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserActive3V3
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserActive3V3(statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrentFaults3V3
 * Signature: (Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrentFaults3V3
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return getUserCurrentFaults3V3(statusPtr);
}

