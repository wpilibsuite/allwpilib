#include <jni.h>
#include <assert.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_WatchdogJNI.h"

#include "HAL/Watchdog.h"
// set the logging level
TLogLevel watchdogJNILogLevel = logDEBUG;

#define WATCHDOGJNI_LOG(level) \
    if (level > watchdogJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    initializeWatchdog
 * Signature: (Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_initializeWatchdog
  (JNIEnv * env, jclass, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog initializeWatchdog";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	WATCHDOGJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jlong statusCapacity = env->GetDirectBufferCapacity(status);
	WATCHDOGJNI_LOG(logDEBUG) << "Status Capacity = " << statusCapacity;
	void** watchdogPtr = (void**)new unsigned char[4];
	*watchdogPtr = initializeWatchdog(statusPtr);
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *watchdogPtr;
	return env->NewDirectByteBuffer( watchdogPtr, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    cleanWatchdog
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_cleanWatchdog
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog cleanWatchdog";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	cleanWatchdog( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    feedWatchdog
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_feedWatchdog
  (JNIEnv * env, jclass, jobject id, jobject status )
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog feedWatchdog";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte returnValue = feedWatchdog( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "feedWatchdog return = " << (short)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    killWatchdog
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_killWatchdog
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog killWatchdog";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	killWatchdog( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    getWatchdogLastFed
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_getWatchdogLastFed
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog getWatchdogLastFed";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	double returnValue = getWatchdogLastFed( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "LastFed = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    getWatchdogExpiration
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_getWatchdogExpiration
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog getWatchdogExpiration";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	double returnValue = getWatchdogExpiration( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "Expiration = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    setWatchdogExpiration
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_setWatchdogExpiration
  (JNIEnv * env, jclass, jobject id, jdouble value, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog setWatchdogExpiration";
	WATCHDOGJNI_LOG(logDEBUG) << "Expiration = " << value;
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setWatchdogExpiration( *javaId, value, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    getWatchdogEnabled
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_getWatchdogEnabled
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog getWatchdogEnabled";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte returnValue = getWatchdogEnabled( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "Enabled = " << (short)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    setWatchdogEnabled
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_setWatchdogEnabled
  (JNIEnv * env, jclass, jobject id, jbyte value, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog setWatchdogEnabled";
	WATCHDOGJNI_LOG(logDEBUG) << "Enabled = " << (short)value;
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	setWatchdogEnabled( *javaId, value, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    isWatchdogAlive
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_isWatchdogAlive
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog isWatchdogAlive";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte returnValue = isWatchdogAlive( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "IsAlive = " << (short)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_WatchdogJNI
 * Method:    isWatchdogSystemActive
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_WatchdogJNI_isWatchdogSystemActive
  (JNIEnv * env, jclass, jobject id, jobject status)
{
	WATCHDOGJNI_LOG(logDEBUG) << "Calling Watchdog isWatchdogSystemActive";
	void ** javaId = (void**)env->GetDirectBufferAddress(id);
	WATCHDOGJNI_LOG(logDEBUG) << "Watchdog Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte returnValue = isWatchdogSystemActive( *javaId, statusPtr );
	WATCHDOGJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	WATCHDOGJNI_LOG(logDEBUG) << "IsActive = " << (short)returnValue;
	return returnValue;
}

