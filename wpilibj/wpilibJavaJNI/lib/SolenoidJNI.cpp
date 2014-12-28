#include <jni.h>
#include "Log.hpp"
#include "HAL/HAL.hpp"

#include "edu_wpi_first_wpilibj_hal_SolenoidJNI.h"

TLogLevel solenoidJNILogLevel = logERROR;

#define SOLENOIDJNI_LOG(level) \
    if (level > solenoidJNILogLevel) ; \
    else Log().Get(level)

typedef void *VoidPointer;

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    initializeSolenoidPort
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_initializeSolenoidPort
  (JNIEnv *env, jclass, jobject port_pointer, jobject status)
{
	SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI initializeSolenoidPort";
	
	VoidPointer *port_pointer_pointer = (VoidPointer *)env->GetDirectBufferAddress(port_pointer);
	SOLENOIDJNI_LOG(logDEBUG) << "Port Ptr = " << *port_pointer_pointer;
	char *aschars = (char *)(*port_pointer_pointer);
	SOLENOIDJNI_LOG(logDEBUG) << '\t' << (int)aschars[0] << '\t' << (int)aschars[1] << std::endl;
	
	jint *status_pointer = (jint*)env->GetDirectBufferAddress(status);
	SOLENOIDJNI_LOG(logDEBUG) << "Status Ptr = " << status_pointer;
	
	VoidPointer *solenoid_port_pointer = new VoidPointer;
	*solenoid_port_pointer = initializeSolenoidPort(*port_pointer_pointer, status_pointer);
	
	SOLENOIDJNI_LOG(logDEBUG) << "Status = " << *status_pointer;
	SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Pointer = " << *solenoid_port_pointer;
	
	int *asints = (int *)(*solenoid_port_pointer);
	SOLENOIDJNI_LOG(logDEBUG) << '\t' << asints[0] << '\t' << asints[1] << std::endl;
	
	return env->NewDirectByteBuffer(solenoid_port_pointer, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPortWithModule
 * Signature: (BB)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPortWithModule
  (JNIEnv *env, jclass, jbyte module, jbyte channel)
{
	VoidPointer *port_pointer = new VoidPointer;
	*port_pointer = getPortWithModule(module, channel);
	
	return env->NewDirectByteBuffer(port_pointer, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    setSolenoid
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_setSolenoid
  (JNIEnv *env, jclass, jobject solenoid_port, jbyte value, jobject status)
{
	SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI SetSolenoid";
	
	VoidPointer *solenoid_port_pointer = (VoidPointer *)env->GetDirectBufferAddress(solenoid_port);
	SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Pointer = " << *solenoid_port_pointer;
	
	jint *status_pointer = (jint*)env->GetDirectBufferAddress(status);
	SOLENOIDJNI_LOG(logDEBUG) << "Status Ptr = " << status_pointer;
	
	setSolenoid(*solenoid_port_pointer, value, status_pointer);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getSolenoid
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getSolenoid
  (JNIEnv *env, jclass, jobject solenoid_port, jobject status)
{
	
	VoidPointer *solenoid_port_pointer = (VoidPointer *)env->GetDirectBufferAddress(solenoid_port);
	jint *status_pointer = (jint*)env->GetDirectBufferAddress(status);
	
	return getSolenoid(*solenoid_port_pointer, status_pointer);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidBlackList
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidBlackList
  (JNIEnv *env, jclass, jobject solenoid_port, jobject status)
{
	
	VoidPointer *solenoid_port_pointer = (VoidPointer *)env->GetDirectBufferAddress(solenoid_port);
	jint *status_pointer = (jint*)env->GetDirectBufferAddress(status);
	
	return getPCMSolenoidBlackList(*solenoid_port_pointer, status_pointer);
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageStickyFault
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidVoltageStickyFault
  (JNIEnv *env, jclass, jobject solenoid_port, jobject status)
{
	VoidPointer *solenoid_port_pointer = (VoidPointer *)env->GetDirectBufferAddress(solenoid_port);
	jint *status_pointer = (jint *)env->GetDirectBufferAddress(status);
	
	return getPCMSolenoidVoltageStickyFault(*solenoid_port_pointer, status_pointer);
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageFault
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidVoltageFault
  (JNIEnv *env, jclass, jobject solenoid_port, jobject status)
{
	VoidPointer *solenoid_port_pointer = (VoidPointer *)env->GetDirectBufferAddress(solenoid_port);
	jint *status_pointer = (jint *)env->GetDirectBufferAddress(status);
	
	return getPCMSolenoidVoltageFault(*solenoid_port_pointer, status_pointer);
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    clearAllPCMStickyFaults
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_clearAllPCMStickyFaults
  (JNIEnv *env, jclass, jobject solenoid_port, jobject status)
{
	VoidPointer *solenoid_port_pointer = (VoidPointer *)env->GetDirectBufferAddress(solenoid_port);
	jint *status_pointer = (jint *)env->GetDirectBufferAddress(status);
	
	clearAllPCMStickyFaults_sol(*solenoid_port_pointer, status_pointer);
}
