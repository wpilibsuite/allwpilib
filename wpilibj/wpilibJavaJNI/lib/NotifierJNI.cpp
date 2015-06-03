#include <jni.h>
#include <assert.h>
#include <functional>
#include <stdio.h>
#include "Log.hpp"
#include "edu_wpi_first_wpilibj_hal_NotifierJNI.h"
#include "HAL/Notifier.hpp"

// set the logging level
TLogLevel notifierJNILogLevel = logWARNING;

#define NOTIFIERJNI_LOG(level) \
    if (level > notifierJNILogLevel) ; \
    else Log().Get(level)

// The jvm object is necessary in order to attach new threads (ie,
// notifierHandler), to the JVM.
static JavaVM *jvm;

static const int kPtrSize = sizeof(void*);

// Utility functions which convert back and forth between pointers and Java
// ByteBuffers.

jint* GetStatusPtr(JNIEnv *env, jobject status) {
  return (jint*)env->GetDirectBufferAddress(status);
}

jobject PtrToByteBuf(JNIEnv *env, void *ptr) {
  // Stores a pointer into a byte buffer of the appropriate length.
  return env->NewDirectByteBuffer(ptr, kPtrSize);
}

void *ByteBufToPtr(JNIEnv *env, jobject bytebuf) {
  void * ptr = (void*)env->GetDirectBufferAddress(bytebuf);
  return ptr;
}

// These two are used to pass information to the notifierHandler without using
// up function parameters.
// See below for more information.
static jobject func_global;
static jmethodID mid_global;

// The arguments are unused by the HAL Notifier; they just satisfy a particular
// function signature.
void notifierHandler(uint32_t mask, void* param) {
  jobject handler_obj = func_global;
  jmethodID mid = mid_global;

	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI interruptHandler";

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
	NOTIFIERJNI_LOG(logDEBUG) << "Leaving NOTIFIERJNI interruptHandler";
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    initializeNotifierJVM
 * Signature: (Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_NotifierJNI_initializeNotifierJVM
  (JNIEnv *env, jclass, jobject status)
{
	//This method should be called once to setup the JVM
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI initializeNotifierJVM";
	jint * statusPtr = GetStatusPtr(env, status);
	NOTIFIERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jint rs = env->GetJavaVM(&jvm);
	assert (rs == JNI_OK);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    initializeNotifier
 * Signature: (Ljava/lang/Runnable;Ljava/nio/IntBuffer;)Ljava/lang/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_NotifierJNI_initializeNotifier
  (JNIEnv *env, jclass, jobject func, jobject status)
{
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI initializeNotifier";
	jint * statusPtr = GetStatusPtr(env, status);
	NOTIFIERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

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

	void *notifierPtr = initializeNotifier(notifierHandler, statusPtr);

	NOTIFIERJNI_LOG(logDEBUG) << "Notifier Ptr = " << notifierPtr;
	NOTIFIERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

	return PtrToByteBuf(env, notifierPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    cleanNotifier
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL
    Java_edu_wpi_first_wpilibj_hal_NotifierJNI_cleanNotifier(JNIEnv *env, jclass, jobject notifierPtr, jobject status) {
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI cleanNotifier";

  void *ptr = ByteBufToPtr(env, notifierPtr);
	NOTIFIERJNI_LOG(logDEBUG) << "Notifier Ptr = " << ptr;

  jint *statusPtr = GetStatusPtr(env, status);
	NOTIFIERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

  cleanNotifier(ptr, statusPtr);
	NOTIFIERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    updateNotifierAlarm
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL
    Java_edu_wpi_first_wpilibj_hal_NotifierJNI_updateNotifierAlarm(
        JNIEnv *env, jclass cls, jobject notifierPtr, jint triggerTime,
        jobject status) {
	NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI updateNotifierAlarm";

  void *ptr = ByteBufToPtr(env, notifierPtr);
	NOTIFIERJNI_LOG(logDEBUG) << "Notifier Ptr = " << ptr;

  jint *statusPtr = GetStatusPtr(env, status);
	NOTIFIERJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
  NOTIFIERJNI_LOG(logDEBUG) << "triggerTime Ptr = " << &triggerTime;

  updateNotifierAlarm(ptr, (uint32_t)triggerTime, statusPtr);
	NOTIFIERJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}
