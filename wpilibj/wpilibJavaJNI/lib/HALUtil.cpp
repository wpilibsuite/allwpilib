#include <jni.h>
#include <assert.h>
#include "Log.hpp"
#include "edu_wpi_first_wpilibj_hal_HALUtil.h"
#include "HAL/HAL.hpp"

// set the logging level
TLogLevel halUtilLogLevel = logDEBUG;

#define HALUTIL_LOG(level) \
    if (level > halUtilLogLevel) ; \
    else Log().Get(level)


//
// indicate JNI version support desired
//
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
	// set our logging level
	Log::ReportingLevel() = logDEBUG;
	return JNI_VERSION_1_6;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    initializeMutex
 * Signature: (I)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_initializeMutexNormal
(JNIEnv * env, jclass)
{
	HALUTIL_LOG(logDEBUG) << "Calling HALUtil initializeMutex";
	MUTEX_ID* mutexPtr = (MUTEX_ID*)new unsigned char[4];
	*mutexPtr = initializeMutexNormal();
	HALUTIL_LOG(logDEBUG) << "Mutex Ptr = " << *mutexPtr;
	return env->NewDirectByteBuffer( mutexPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    deleteMutex
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_deleteMutex
(JNIEnv * env, jclass, jobject id )
{
	HALUTIL_LOG(logDEBUG) << "Calling HALUtil deleteMutex";
	MUTEX_ID* javaId = (MUTEX_ID*)env->GetDirectBufferAddress(id);
	HALUTIL_LOG(logDEBUG) << "Mutex Ptr = " << *javaId;
	deleteMutex( *javaId );
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    takeMutex
 * Signature: (Ljava/nio/ByteBuffer;I)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_takeMutex
(JNIEnv * env, jclass, jobject id)
{
	//HALUTIL_LOG(logDEBUG) << "Calling HALUtil takeMutex";
	MUTEX_ID* javaId = (MUTEX_ID*)env->GetDirectBufferAddress(id);
	//HALUTIL_LOG(logDEBUG) << "Mutex Ptr = " << *javaId;
	jbyte returnValue = takeMutex(*javaId);
	//HALUTIL_LOG(logDEBUG) << "Take Result = " << (void*)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGAVersion
 * Signature: (Ljava/nio/IntBuffer;)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGAVersion
  (JNIEnv * env, jclass, jobject status)
{
	HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGAVersion";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jshort returnValue = getFPGAVersion( statusPtr );
	HALUTIL_LOG(logDEBUG) << "Status = " << *statusPtr;
	HALUTIL_LOG(logDEBUG) << "FPGAVersion = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGARevision
 * Signature: (Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGARevision
  (JNIEnv * env, jclass, jobject status)
{
	HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGARevision";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jint returnValue = getFPGARevision( statusPtr );
	HALUTIL_LOG(logDEBUG) << "Status = " << *statusPtr;
	HALUTIL_LOG(logDEBUG) << "FPGARevision = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGATime
 * Signature: (Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGATime
  (JNIEnv * env, jclass, jobject status)
{
	//HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGATime";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jlong returnValue = getFPGATime( statusPtr );
	//HALUTIL_LOG(logDEBUG) << "Status = " << *statusPtr;
	//HALUTIL_LOG(logDEBUG) << "FPGATime = " << returnValue;
	return returnValue;

}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getHALErrorMessage
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_getHALErrorMessage
  (JNIEnv * paramEnv, jclass, jint paramId)
{
	const char * msg = getHALErrorMessage(paramId);
	HALUTIL_LOG(logDEBUG) << "Calling HALUtil getHALErrorMessage id=" << paramId << " msg=" << msg;
	return paramEnv->NewStringUTF(msg);
}
