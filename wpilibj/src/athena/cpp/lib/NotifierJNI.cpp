#include <jni.h>
#include <assert.h>
#include <functional>
#include <stdio.h>
#include "Log.hpp"
#include "edu_wpi_first_wpilibj_hal_NotifierJNI.h"
#include "HAL/Notifier.hpp"
#include "HALUtil.h"

// set the logging level
TLogLevel notifierJNILogLevel = logWARNING;

#define NOTIFIERJNI_LOG(level) \
    if (level > notifierJNILogLevel) ; \
    else Log().Get(level)

// These two are used to pass information to the notifierHandler without using
// up function parameters.
// See below for more information.
static jobject func_global;
static jmethodID mid_global;

// The arguments are unused by the HAL Notifier; they just satisfy a particular
// function signature.
void notifierHandler(uint32_t currentTimeInt, void* param) {
  jobject handler_obj = func_global;
  jmethodID mid = mid_global;

	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI notifierHandler";

	//Because this is a callback in a new thread we must attach it to the JVM.
	JNIEnv *env;
  jint rs;
  // Check to see if we are already part of a JVM thread or if we need to attach
  // ourselves.
  int getEnvStat = jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
  if (getEnvStat == JNI_EDETACHED) {
    rs = jvm->AttachCurrentThread((void**)&env, NULL);
    assert (rs == JNI_OK);
  }
	NOTIFIERJNI_LOG(logDEBUG) << "Attached to thread. Object is: " << handler_obj;

  // Actuall call the user function.
	env->CallVoidMethod(handler_obj, mid);
	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}

  // Only detach if we needed to attach oursleves in the first place.
  if (getEnvStat == JNI_EDETACHED) {
    rs = jvm->DetachCurrentThread();
    assert (rs == JNI_OK);
  }
	NOTIFIERJNI_LOG(logDEBUG) << "Leaving NOTIFIERJNI notifierHandler";
}

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    initializeNotifier
 * Signature: (Ljava/lang/Runnable;)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_NotifierJNI_initializeNotifier
  (JNIEnv *env, jclass, jobject func)
{
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI initializeNotifier";

  jclass cls = env->GetObjectClass(func);
  jmethodID mid = env->GetMethodID(cls, "run", "()V");

  // In order to pass the user's Runnable to the notifierHandler, we have to use
  // something other than the function arguments (because the function arguments
  // are dictated by the callback format). As such, we instead use a couple of
  // global variables to pass around the object reference.
  // This is not ideal, but the only other option that came to mind was to use
  // lambda function captures, but, unfortunately, it turns out that using
  // captures in a lambda changes the function signature such that it can no
  // long be used as a standared C-style function pointer.
  // Need to set as global ref to avoid seg faults when referring to it later.
  func_global = env->NewGlobalRef(func);
  mid_global = mid;

	int32_t status = 0;
	void *notifierPtr = initializeNotifier(notifierHandler, nullptr, &status);

	NOTIFIERJNI_LOG(logDEBUG) << "Notifier Ptr = " << notifierPtr;
	NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;

	CheckStatus(env, status);
	return (jlong)notifierPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    cleanNotifier
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_NotifierJNI_cleanNotifier
  (JNIEnv *env, jclass, jlong notifierPtr)
{
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI cleanNotifier";

	NOTIFIERJNI_LOG(logDEBUG) << "Notifier Ptr = " << (void*)notifierPtr;

  int32_t status = 0;
  cleanNotifier((void*)notifierPtr, &status);
	NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    updateNotifierAlarm
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_NotifierJNI_updateNotifierAlarm
  (JNIEnv *env, jclass cls, jlong notifierPtr, jint triggerTime)
{
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI updateNotifierAlarm";

	NOTIFIERJNI_LOG(logDEBUG) << "Notifier Ptr = " << (void*)notifierPtr;

  NOTIFIERJNI_LOG(logDEBUG) << "triggerTime Ptr = " << &triggerTime;

  int32_t status = 0;
  updateNotifierAlarm((void*)notifierPtr, (uint32_t)triggerTime, &status);
	NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
}

}  // extern "C"
