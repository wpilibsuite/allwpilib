#include "edu_wpi_first_wpilibj_hal_PDPJNI.h"
#include "HAL/PDP.hpp"

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTemperature
 * Signature: (Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTemperature
  (JNIEnv *env, jclass, jobject status)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPTemperature(status_ptr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPVoltage
 * Signature: (Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPVoltage
  (JNIEnv *env, jclass, jobject status)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPVoltage(status_ptr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPChannelCurrent
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPChannelCurrent
  (JNIEnv *env, jclass, jbyte channel, jobject status)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPChannelCurrent(channel, status_ptr);
}

