#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_JNIWrapper.h"

#include "HAL/HAL.hpp"

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    getPortWithModule
 * Signature: (BB)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_JNIWrapper_getPortWithModule
  (JNIEnv * env, jclass, jbyte module, jbyte pin)
{
	//FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
	//FILE_LOG(logDEBUG) << "Module = " << (jint)module;
	//FILE_LOG(logDEBUG) << "Pin = " << (jint)pin;
	void** portPtr = (void**)new unsigned char[4];
	*portPtr = getPortWithModule(module,pin);
	//FILE_LOG(logDEBUG) << "Port Ptr = " << *portPtr;
	return env->NewDirectByteBuffer( portPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    getPort
 * Signature: (BB)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_JNIWrapper_getPort
  (JNIEnv * env, jclass, jbyte pin)
{
	//FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
	//FILE_LOG(logDEBUG) << "Module = " << (jint)module;
	//FILE_LOG(logDEBUG) << "Pin = " << (jint)pin;
	void** portPtr = (void**)new unsigned char[4];
	*portPtr = getPort(pin);
	//FILE_LOG(logDEBUG) << "Port Ptr = " << *portPtr;
	return env->NewDirectByteBuffer( portPtr, 4);
}
