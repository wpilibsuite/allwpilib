/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALUtil.h"

#include <assert.h>
#include <errno.h>
#include <jni.h>

#include <cstring>
#include <string>

#include "FRC_NetworkCommunication/CANSessionMux.h"
#include "HAL/HAL.h"
#include "Log.h"
#include "edu_wpi_first_wpilibj_hal_HALUtil.h"

// set the logging level
TLogLevel halUtilLogLevel = logWARNING;

#define HALUTIL_LOG(level)     \
  if (level > halUtilLogLevel) \
    ;                          \
  else                         \
  Log().Get(level)

#define kRioStatusOffset -63000
#define kRioStatusSuccess 0
#define kRIOStatusBufferInvalidSize (kRioStatusOffset - 80)
#define kRIOStatusOperationTimedOut -52007
#define kRIOStatusFeatureNotSupported (kRioStatusOffset - 193)
#define kRIOStatusResourceNotInitialized -52010

JavaVM *jvm = nullptr;
static jclass throwableCls = nullptr;
static jclass stackTraceElementCls = nullptr;
static jclass runtimeExCls = nullptr;
static jclass illegalArgExCls = nullptr;
static jclass boundaryExCls = nullptr;
static jclass canInvalidBufferExCls = nullptr;
static jclass canMessageNotFoundExCls = nullptr;
static jclass canMessageNotAllowedExCls = nullptr;
static jclass canNotInitializedExCls = nullptr;
static jclass uncleanStatusExCls = nullptr;

static void GetStackTrace(JNIEnv *env, std::string &res, std::string &func) {
  // create a throwable
  static jmethodID constructorId = nullptr;
  if (!constructorId)
    constructorId = env->GetMethodID(throwableCls, "<init>", "()V");
  jobject throwable = env->NewObject(throwableCls, constructorId);

  // retrieve information from the exception.
  // get method id
  // getStackTrace returns an array of StackTraceElement
  static jmethodID getStackTraceId = nullptr;
  if (!getStackTraceId)
    getStackTraceId = env->GetMethodID(throwableCls, "getStackTrace",
                                       "()[Ljava/lang/StackTraceElement;");

  // call getStackTrace
  jobjectArray stackTrace =
      (jobjectArray)env->CallObjectMethod(throwable, getStackTraceId);

  if (!stackTrace) return;

  // get length of the array
  jsize stackTraceLength = env->GetArrayLength(stackTrace);

  // get toString methodId of StackTraceElement class
  static jmethodID toStringId = nullptr;
  if (!toStringId)
    toStringId = env->GetMethodID(stackTraceElementCls, "toString",
                                  "()Ljava/lang/String;");

  bool haveLoc = false;
  for (jsize i = 0; i < stackTraceLength; i++) {
    // add the result of toString method of each element in the result
    jobject curStackTraceElement = env->GetObjectArrayElement(stackTrace, i);

    // call to string on the object
    jstring stackElementString =
        (jstring)env->CallObjectMethod(curStackTraceElement, toStringId);

    if (!stackElementString) {
      env->DeleteLocalRef(stackTrace);
      env->DeleteLocalRef(curStackTraceElement);
      return;
    }

    // add a line to res
    // res += " at ";
    const char *tmp = env->GetStringUTFChars(stackElementString, nullptr);
    res += tmp;
    res += '\n';

    // func is caller of immediate caller (if there was one)
    // or, if we see it, the first user function
    if (i == 1)
      func = tmp;
    else if (i > 1 && !haveLoc &&
             std::strncmp(tmp, "edu.wpi.first.wpilibj", 21) != 0) {
      func = tmp;
      haveLoc = true;
    }
    env->ReleaseStringUTFChars(stackElementString, tmp);

    env->DeleteLocalRef(curStackTraceElement);
    env->DeleteLocalRef(stackElementString);
  }

  // release java resources
  env->DeleteLocalRef(stackTrace);
}

void ReportError(JNIEnv *env, int32_t status, bool do_throw) {
  if (status == 0) return;
  const char *message = getHALErrorMessage(status);
  if (do_throw && status < 0) {
    char *buf = new char[strlen(message) + 30];
    sprintf(buf, " Code: %d. %s", status, message);
    env->ThrowNew(runtimeExCls, buf);
    delete[] buf;
  } else {
    std::string stack = " at ";
    std::string func;
    GetStackTrace(env, stack, func);
    HALSendError(1, status, 0, message, func.c_str(), stack.c_str(), 1);
  }
}

void ReportCANError(JNIEnv *env, int32_t status, int32_t message_id) {
  if (status >= 0) return;
  switch (status) {
    case kRioStatusSuccess:
      // Everything is ok... don't throw.
      break;
    case ERR_CANSessionMux_InvalidBuffer:
    case kRIOStatusBufferInvalidSize: {
      static jmethodID invalidBufConstruct = nullptr;
      if (!invalidBufConstruct)
        invalidBufConstruct =
            env->GetMethodID(canInvalidBufferExCls, "<init>", "()V");
      jobject exception =
          env->NewObject(canInvalidBufferExCls, invalidBufConstruct);
      env->Throw(static_cast<jthrowable>(exception));
      break;
    }
    case ERR_CANSessionMux_MessageNotFound:
    case kRIOStatusOperationTimedOut: {
      static jmethodID messageNotFoundConstruct = nullptr;
      if (!messageNotFoundConstruct)
        messageNotFoundConstruct =
            env->GetMethodID(canMessageNotFoundExCls, "<init>", "()V");
      jobject exception =
          env->NewObject(canMessageNotFoundExCls, messageNotFoundConstruct);
      env->Throw(static_cast<jthrowable>(exception));
      break;
    }
    case ERR_CANSessionMux_NotAllowed:
    case kRIOStatusFeatureNotSupported: {
      char buf[100];
      sprintf(buf, "MessageID = %d", message_id);
      env->ThrowNew(canMessageNotAllowedExCls, buf);
      break;
    }
    case ERR_CANSessionMux_NotInitialized:
    case kRIOStatusResourceNotInitialized: {
      static jmethodID notInitConstruct = nullptr;
      if (!notInitConstruct)
        notInitConstruct =
            env->GetMethodID(canNotInitializedExCls, "<init>", "()V");
      jobject exception =
          env->NewObject(canNotInitializedExCls, notInitConstruct);
      env->Throw(static_cast<jthrowable>(exception));
      break;
    }
    default: {
      char buf[100];
      sprintf(buf, "Fatal status code detected: %d", status);
      env->ThrowNew(uncleanStatusExCls, buf);
      break;
    }
  }
}

void ThrowIllegalArgumentException(JNIEnv *env, const char *msg) {
  env->ThrowNew(illegalArgExCls, msg);
}

void ThrowBoundaryException(JNIEnv *env, double value, double lower,
                            double upper) {
  static jmethodID getMessage = nullptr;
  if (!getMessage)
    getMessage = env->GetStaticMethodID(boundaryExCls, "getMessage",
                                        "(DDD)Ljava/lang/String;");

  static jmethodID constructor = nullptr;
  if (!constructor)
    constructor =
        env->GetMethodID(boundaryExCls, "<init>", "(Ljava/lang/String;)V");

  jobject msg =
      env->CallStaticObjectMethod(boundaryExCls, getMessage, (jdouble)value,
                                  (jdouble)lower, (jdouble)upper);
  jobject ex = env->NewObject(boundaryExCls, constructor, msg);
  env->Throw(static_cast<jthrowable>(ex));
}

extern "C" {

//
// indicate JNI version support desired and load classes
//
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  jvm = vm;

  // set our logging level
  Log::ReportingLevel() = logDEBUG;

  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  // Cache references to classes
  jclass local;

  local = env->FindClass("java/lang/Throwable");
  if (!local) return JNI_ERR;
  throwableCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!throwableCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("java/lang/StackTraceElement");
  if (!local) return JNI_ERR;
  stackTraceElementCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!stackTraceElementCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("java/lang/RuntimeException");
  if (!local) return JNI_ERR;
  runtimeExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!runtimeExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("java/lang/IllegalArgumentException");
  if (!local) return JNI_ERR;
  illegalArgExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!illegalArgExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("edu/wpi/first/wpilibj/util/BoundaryException");
  if (!local) return JNI_ERR;
  boundaryExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!boundaryExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("edu/wpi/first/wpilibj/can/CANInvalidBufferException");
  if (!local) return JNI_ERR;
  canInvalidBufferExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!canInvalidBufferExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local =
      env->FindClass("edu/wpi/first/wpilibj/can/CANMessageNotFoundException");
  if (!local) return JNI_ERR;
  canMessageNotFoundExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!canMessageNotFoundExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local =
      env->FindClass("edu/wpi/first/wpilibj/can/CANMessageNotAllowedException");
  if (!local) return JNI_ERR;
  canMessageNotAllowedExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!canMessageNotAllowedExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local =
      env->FindClass("edu/wpi/first/wpilibj/can/CANNotInitializedException");
  if (!local) return JNI_ERR;
  canNotInitializedExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!canNotInitializedExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("edu/wpi/first/wpilibj/util/UncleanStatusException");
  if (!local) return JNI_ERR;
  uncleanStatusExCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!uncleanStatusExCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;
  // Delete global references
  if (throwableCls) env->DeleteGlobalRef(throwableCls);
  if (stackTraceElementCls) env->DeleteGlobalRef(stackTraceElementCls);
  if (runtimeExCls) env->DeleteGlobalRef(runtimeExCls);
  if (illegalArgExCls) env->DeleteGlobalRef(illegalArgExCls);
  if (boundaryExCls) env->DeleteGlobalRef(boundaryExCls);
  if (canInvalidBufferExCls) env->DeleteGlobalRef(canInvalidBufferExCls);
  if (canMessageNotFoundExCls) env->DeleteGlobalRef(canMessageNotFoundExCls);
  if (canMessageNotAllowedExCls)
    env->DeleteGlobalRef(canMessageNotAllowedExCls);
  if (canNotInitializedExCls) env->DeleteGlobalRef(canNotInitializedExCls);
  if (uncleanStatusExCls) env->DeleteGlobalRef(uncleanStatusExCls);
  jvm = nullptr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    initializeMutex
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_initializeMutexNormal(
    JNIEnv *env, jclass) {
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil initializeMutex";
  MUTEX_ID mutex = initializeMutexNormal();
  HALUTIL_LOG(logDEBUG) << "Mutex Ptr = " << mutex;
  return (jlong)mutex;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    deleteMutex
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_deleteMutex(
    JNIEnv *env, jclass, jlong id) {
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil deleteMutex";
  HALUTIL_LOG(logDEBUG) << "Mutex Ptr = " << (MUTEX_ID)id;
  deleteMutex((MUTEX_ID)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    takeMutex
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_takeMutex(
    JNIEnv *env, jclass, jlong id) {
  // HALUTIL_LOG(logDEBUG) << "Calling HALUtil takeMutex";
  // HALUTIL_LOG(logDEBUG) << "Mutex Ptr = " << (MUTEX_ID)id;
  takeMutex((MUTEX_ID)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    initializeMultiWait
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_initializeMultiWait(
    JNIEnv *env, jclass) {
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil initializeMultiWait";
  MULTIWAIT_ID multiWait = initializeMultiWait();
  HALUTIL_LOG(logDEBUG) << "MultiWait Ptr = " << multiWait;
  return (jlong)multiWait;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    deleteMultiWait
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_deleteMultiWait(
    JNIEnv *env, jclass, jlong id) {
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil deleteMultiWait";
  HALUTIL_LOG(logDEBUG) << "MultiWait Ptr = " << (MULTIWAIT_ID)id;
  deleteMultiWait((MULTIWAIT_ID)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    takeMultiWait
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_takeMultiWait(
    JNIEnv *env, jclass, jlong multiWaitId, jlong mutexId) {
  takeMultiWait((MULTIWAIT_ID)multiWaitId, (MUTEX_ID)mutexId);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGAVersion
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGAVersion(JNIEnv *env, jclass) {
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGAVersion";
  int32_t status = 0;
  jshort returnValue = getFPGAVersion(&status);
  HALUTIL_LOG(logDEBUG) << "Status = " << status;
  HALUTIL_LOG(logDEBUG) << "FPGAVersion = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGARevision
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGARevision(JNIEnv *env, jclass) {
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGARevision";
  int32_t status = 0;
  jint returnValue = getFPGARevision(&status);
  HALUTIL_LOG(logDEBUG) << "Status = " << status;
  HALUTIL_LOG(logDEBUG) << "FPGARevision = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGATime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGATime(JNIEnv *env, jclass) {
  // HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGATime";
  int32_t status = 0;
  jlong returnValue = getFPGATime(&status);
  // HALUTIL_LOG(logDEBUG) << "Status = " << status;
  // HALUTIL_LOG(logDEBUG) << "FPGATime = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getFPGAButton
 * Signature: ()I
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_getFPGAButton(JNIEnv *env, jclass) {
  // HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGATime";
  int32_t status = 0;
  jboolean returnValue = getFPGAButton(&status);
  // HALUTIL_LOG(logDEBUG) << "Status = " << status;
  // HALUTIL_LOG(logDEBUG) << "FPGATime = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getHALErrorMessage
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_getHALErrorMessage(
    JNIEnv *paramEnv, jclass, jint paramId) {
  const char *msg = getHALErrorMessage(paramId);
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getHALErrorMessage id=" << paramId
                        << " msg=" << msg;
  return paramEnv->NewStringUTF(msg);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getHALErrno
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_HALUtil_getHALErrno(JNIEnv *, jclass) {
  return errno;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_HALUtil
 * Method:    getHALstrerror
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_first_wpilibj_hal_HALUtil_getHALstrerror(
    JNIEnv *env, jclass, jint errorCode) {
  const char *msg = strerror(errno);
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getHALstrerror errorCode="
                        << errorCode << " msg=" << msg;
  return env->NewStringUTF(msg);
}

}  // extern "C"
