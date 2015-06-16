#include "edu_wpi_first_wpilibj_hal_PDPJNI.h"
#include "HAL/PDP.hpp"

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTemperature
 * Signature: (Ljava/nio/IntBuffer;)D
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_initializePDP
  (JNIEnv *, jclass, jint module)
{
	initializePDP(module);
}
  
/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTemperature
 * Signature: (Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTemperature
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPTemperature(status_ptr, module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPVoltage
 * Signature: (Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPVoltage
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPVoltage(status_ptr, module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPChannelCurrent
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPChannelCurrent
  (JNIEnv *env, jclass, jbyte channel, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPChannelCurrent(channel, status_ptr, module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTotalCurrent
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTotalCurrent
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPTotalCurrent(status_ptr, module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTotalPower
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTotalPower
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPTotalPower(status_ptr, module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    resetPDPTotalEnergy
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTotalEnergy
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	return getPDPTotalEnergy(status_ptr, module);
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    resetPDPTotalEnergy
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_resetPDPTotalEnergy
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	resetPDPTotalEnergy(status_ptr, module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    clearStickyFaults
 * Signature: (BLjava/nio/IntBuffer;)D
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_clearPDPStickyFaults
  (JNIEnv *env, jclass, jobject status, jint module)
{
	jint *status_ptr = (jint *)env->GetDirectBufferAddress(status);
	
	clearPDPStickyFaults(status_ptr, module);
}