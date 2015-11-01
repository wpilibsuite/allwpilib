#include <jni.h>
#include "Log.hpp"
#include "HAL/HAL.hpp"

#include "edu_wpi_first_wpilibj_hal_SolenoidJNI.h"

#include "HALUtil.h"

TLogLevel solenoidJNILogLevel = logERROR;

#define SOLENOIDJNI_LOG(level) \
    if (level > solenoidJNILogLevel) ; \
    else Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    initializeSolenoidPort
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_initializeSolenoidPort
  (JNIEnv *env, jclass, jlong port_pointer)
{
	SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI initializeSolenoidPort";
	
	SOLENOIDJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)port_pointer;
	char *aschars = (char *)port_pointer;
	SOLENOIDJNI_LOG(logDEBUG) << '\t' << (int)aschars[0] << '\t' << (int)aschars[1] << std::endl;
	
	int32_t status = 0;
	void* solenoid_port_pointer = initializeSolenoidPort((void*)port_pointer, &status);
	
	SOLENOIDJNI_LOG(logDEBUG) << "Status = " << status;
	SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Pointer = " << solenoid_port_pointer;
	
	int *asints = (int *)solenoid_port_pointer;
	SOLENOIDJNI_LOG(logDEBUG) << '\t' << asints[0] << '\t' << asints[1] << std::endl;

	CheckStatus(env, status);
	return (jlong)solenoid_port_pointer;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPortWithModule
 * Signature: (BB)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPortWithModule
  (JNIEnv *env, jclass, jbyte module, jbyte channel)
{
	void* port_pointer = getPortWithModule(module, channel);
	
	return (jlong)port_pointer;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    setSolenoid
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_setSolenoid
  (JNIEnv *env, jclass, jlong solenoid_port, jboolean value)
{
	SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI SetSolenoid";
	
	SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Pointer = " << (void*)solenoid_port;
	
	int32_t status = 0;
	setSolenoid((void*)solenoid_port, value, &status);
	CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getSolenoid
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getSolenoid
  (JNIEnv *env, jclass, jlong solenoid_port)
{
	int32_t status = 0;
	jboolean val = getSolenoid((void*)solenoid_port, &status);
	CheckStatus(env, status);
	return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidBlackList
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidBlackList
  (JNIEnv *env, jclass, jlong solenoid_port)
{
	int32_t status = 0;
	jint val = getPCMSolenoidBlackList((void*)solenoid_port, &status);
	CheckStatus(env, status);
	return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageStickyFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidVoltageStickyFault
  (JNIEnv *env, jclass, jlong solenoid_port)
{
	int32_t status = 0;
	bool val = getPCMSolenoidVoltageStickyFault((void*)solenoid_port, &status);
	CheckStatus(env, status);
	return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidVoltageFault
  (JNIEnv *env, jclass, jlong solenoid_port)
{
	int32_t status = 0;
	bool val = getPCMSolenoidVoltageFault((void*)solenoid_port, &status);
	CheckStatus(env, status);
	return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    clearAllPCMStickyFaults
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_clearAllPCMStickyFaults
  (JNIEnv *env, jclass, jlong solenoid_port)
{
	int32_t status = 0;
	clearAllPCMStickyFaults_sol((void*)solenoid_port, &status);
	CheckStatus(env, status);
}

}  // extern "C"
