/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Notifier.h"
#include <assert.h>
#include <jni.h>
#include <stdio.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include "HALUtil.h"
#include "HAL/cpp/Log.h"
#include "edu_wpi_first_wpilibj_hal_NotifierJNI.h"
#include "support/SafeThread.h"
#include "HAL/cpp/NotifierInternal.h"

using namespace frc;

// set the logging level
TLogLevel notifierJNILogLevel = logWARNING;

#define NOTIFIERJNI_LOG(level)     \
  if (level > notifierJNILogLevel) \
    ;                              \
  else                             \
  Log().Get(level)

// Thread where callbacks are actually performed.
//
// JNI's AttachCurrentThread() creates a Java Thread object on every
// invocation, which is both time inefficient and causes issues with Eclipse
// (which tries to keep a thread list up-to-date and thus gets swamped).
//
// Instead, this class attaches just once.  When a hardware notification
// occurs, a condition variable wakes up this thread and this thread actually
// makes the call into Java.
//
// We don't want to use a FIFO here. If the user code takes too long to
// process, we will just ignore the redundant wakeup.
class NotifierThreadJNI : public wpi::SafeThread {
 public:
  void Main();

  bool m_notify = false;
  jobject m_func = nullptr;
  jmethodID m_mid;
  uint64_t m_currentTime;
};

class NotifierJNI : public wpi::SafeThreadOwner<NotifierThreadJNI> {
 public:
  void SetFunc(JNIEnv *env, jobject func, jmethodID mid);

  void Notify(uint64_t currentTime) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_currentTime = currentTime;
    thr->m_notify = true;
    thr->m_cond.notify_one();
  }
};

void NotifierJNI::SetFunc(JNIEnv *env, jobject func, jmethodID mid) {
  auto thr = GetThread();
  if (!thr) return;
  // free global reference
  if (thr->m_func) env->DeleteGlobalRef(thr->m_func);
  // create global reference
  thr->m_func = env->NewGlobalRef(func);
  thr->m_mid = mid;
}

void NotifierThreadJNI::Main() {
  JNIEnv *env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_2;
  args.name = const_cast<char *>("Notifier");
  args.group = nullptr;
  jint rs =
      jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void **>(&env), &args);
  if (rs != JNI_OK) return;

  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    m_cond.wait(lock, [&] { return !m_active || m_notify; });
    if (!m_active) break;
    m_notify = false;
    if (!m_func) continue;
    jobject func = m_func;
    jmethodID mid = m_mid;
    uint64_t currentTime = m_currentTime;
    lock.unlock();  // don't hold mutex during callback execution
    env->CallVoidMethod(func, mid, static_cast<jlong>(currentTime));
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
      env->ExceptionClear();
    }
    lock.lock();
  }

  // free global reference
  if (m_func) env->DeleteGlobalRef(m_func);

  jvm->DetachCurrentThread();
}

void notifierHandler(uint64_t currentTimeInt, HAL_NotifierHandle handle) {
  int32_t status = 0;
  auto param = HAL_GetNotifierParam(handle, &status);
  if (param == nullptr) return;
  (static_cast<NotifierJNI*>(param))->Notify(currentTimeInt);
}

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    initializeNotifier
 * Signature: (Ljava/lang/Runnable;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_NotifierJNI_initializeNotifier(
    JNIEnv *env, jclass, jobject func) {
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI initializeNotifier";

  jclass cls = env->GetObjectClass(func);
  if (cls == 0) {
    NOTIFIERJNI_LOG(logERROR) << "Error getting java class";
    assert(false);
    return 0;
  }
  jmethodID mid = env->GetMethodID(cls, "apply", "(J)V");
  if (mid == 0) {
    NOTIFIERJNI_LOG(logERROR) << "Error getting java method ID";
    assert(false);
    return 0;
  }

  // each notifier runs in its own thread; this is so if one takes too long
  // to execute, it doesn't keep the others from running
  NotifierJNI *notify = new NotifierJNI;
  notify->Start();
  notify->SetFunc(env, func, mid);
  int32_t status = 0;
  HAL_NotifierHandle notifierHandle = 
      HAL_InitializeNotifierNonThreadedUnsafe(notifierHandler, notify, &status);

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;

  if (notifierHandle <= 0 || !CheckStatusForceThrow(env, status)) {
    // something went wrong in HAL, clean up
    delete notify;
  }

  return (jint)notifierHandle;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    cleanNotifier
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_NotifierJNI_cleanNotifier(
    JNIEnv *env, jclass, jint notifierHandle) {
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI cleanNotifier";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  int32_t status = 0;
  NotifierJNI *notify =
      (NotifierJNI *)HAL_GetNotifierParam((HAL_NotifierHandle)notifierHandle, &status);
  HAL_CleanNotifier((HAL_NotifierHandle)notifierHandle, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  delete notify;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    updateNotifierAlarm
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_NotifierJNI_updateNotifierAlarm(
    JNIEnv *env, jclass cls, jint notifierHandle, jlong triggerTime) {
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI updateNotifierAlarm";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  NOTIFIERJNI_LOG(logDEBUG) << "triggerTime = " << triggerTime;

  int32_t status = 0;
  HAL_UpdateNotifierAlarm((HAL_NotifierHandle)notifierHandle, (uint64_t)triggerTime, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_NotifierJNI
 * Method:    stopNotifierAlarm
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_NotifierJNI_stopNotifierAlarm(
    JNIEnv *env, jclass cls, jint notifierHandle) {
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI stopNotifierAlarm";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  int32_t status = 0;
  HAL_StopNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
