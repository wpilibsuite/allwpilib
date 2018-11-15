/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <atomic>
#include <cassert>
#include <thread>

#include <wpi/SafeThread.h>
#include <wpi/mutex.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_InterruptJNI.h"
#include "hal/Interrupts.h"
#include "hal/cpp/Log.h"

using namespace frc;

TLogLevel interruptJNILogLevel = logERROR;

#define INTERRUPTJNI_LOG(level)     \
  if (level > interruptJNILogLevel) \
    ;                               \
  else                              \
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
class InterruptThreadJNI : public wpi::SafeThread {
 public:
  void Main();

  bool m_notify = false;
  uint32_t m_mask = 0;
  jobject m_func = nullptr;
  jmethodID m_mid;
  jobject m_param = nullptr;
};

class InterruptJNI : public wpi::SafeThreadOwner<InterruptThreadJNI> {
 public:
  void SetFunc(JNIEnv* env, jobject func, jmethodID mid, jobject param);

  void Notify(uint32_t mask) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_notify = true;
    thr->m_mask = mask;
    thr->m_cond.notify_one();
  }
};

void InterruptJNI::SetFunc(JNIEnv* env, jobject func, jmethodID mid,
                           jobject param) {
  auto thr = GetThread();
  if (!thr) return;
  // free global references
  if (thr->m_func) env->DeleteGlobalRef(thr->m_func);
  if (thr->m_param) env->DeleteGlobalRef(thr->m_param);
  // create global references
  thr->m_func = env->NewGlobalRef(func);
  thr->m_param = param ? env->NewGlobalRef(param) : nullptr;
  thr->m_mid = mid;
}

void InterruptThreadJNI::Main() {
  JNIEnv* env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_2;
  args.name = const_cast<char*>("Interrupt");
  args.group = nullptr;
  jint rs = GetJVM()->AttachCurrentThreadAsDaemon(
      reinterpret_cast<void**>(&env), &args);
  if (rs != JNI_OK) return;

  std::unique_lock<wpi::mutex> lock(m_mutex);
  while (m_active) {
    m_cond.wait(lock, [&] { return !m_active || m_notify; });
    if (!m_active) break;
    m_notify = false;
    if (!m_func) continue;
    jobject func = m_func;
    jmethodID mid = m_mid;
    uint32_t mask = m_mask;
    jobject param = m_param;
    lock.unlock();  // don't hold mutex during callback execution
    env->CallVoidMethod(func, mid, static_cast<jint>(mask), param);
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
      env->ExceptionClear();
    }
    lock.lock();
  }

  // free global references
  if (m_func) env->DeleteGlobalRef(m_func);
  if (m_param) env->DeleteGlobalRef(m_param);

  GetJVM()->DetachCurrentThread();
}

void interruptHandler(uint32_t mask, void* param) {
  static_cast<InterruptJNI*>(param)->Notify(mask);
}

extern "C" {

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    initializeInterrupts
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_InterruptJNI_initializeInterrupts
  (JNIEnv* env, jclass, jboolean watcher)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI initializeInterrupts";
  INTERRUPTJNI_LOG(logDEBUG) << "watcher = " << static_cast<bool>(watcher);

  int32_t status = 0;
  HAL_InterruptHandle interrupt = HAL_InitializeInterrupts(watcher, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Handle = " << interrupt;
  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatusForceThrow(env, status);
  return (jint)interrupt;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    cleanInterrupts
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_cleanInterrupts
  (JNIEnv* env, jclass, jint interruptHandle)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI cleanInterrupts";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  int32_t status = 0;
  auto param =
      HAL_CleanInterrupts((HAL_InterruptHandle)interruptHandle, &status);
  if (param) {
    delete static_cast<InterruptJNI*>(param);
  }

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  // ignore status, as an invalid handle just needs to be ignored.
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    waitForInterrupt
 * Signature: (IDZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_InterruptJNI_waitForInterrupt
  (JNIEnv* env, jclass, jint interruptHandle, jdouble timeout,
   jboolean ignorePrevious)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI waitForInterrupt";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  int32_t status = 0;
  int32_t result = HAL_WaitForInterrupt((HAL_InterruptHandle)interruptHandle,
                                        timeout, ignorePrevious, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    enableInterrupts
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_enableInterrupts
  (JNIEnv* env, jclass, jint interruptHandle)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI enableInterrupts";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  int32_t status = 0;
  HAL_EnableInterrupts((HAL_InterruptHandle)interruptHandle, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    disableInterrupts
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_disableInterrupts
  (JNIEnv* env, jclass, jint interruptHandle)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI disableInterrupts";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  int32_t status = 0;
  HAL_DisableInterrupts((HAL_InterruptHandle)interruptHandle, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    readInterruptRisingTimestamp
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_InterruptJNI_readInterruptRisingTimestamp
  (JNIEnv* env, jclass, jint interruptHandle)
{
  INTERRUPTJNI_LOG(logDEBUG)
      << "Calling INTERRUPTJNI readInterruptRisingTimestamp";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  int32_t status = 0;
  jlong timeStamp = HAL_ReadInterruptRisingTimestamp(
      (HAL_InterruptHandle)interruptHandle, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return timeStamp;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    readInterruptFallingTimestamp
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_InterruptJNI_readInterruptFallingTimestamp
  (JNIEnv* env, jclass, jint interruptHandle)
{
  INTERRUPTJNI_LOG(logDEBUG)
      << "Calling INTERRUPTJNI readInterruptFallingTimestamp";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  int32_t status = 0;
  jlong timeStamp = HAL_ReadInterruptFallingTimestamp(
      (HAL_InterruptHandle)interruptHandle, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return timeStamp;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    requestInterrupts
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_requestInterrupts
  (JNIEnv* env, jclass, jint interruptHandle, jint digitalSourceHandle,
   jint analogTriggerType)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI requestInterrupts";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;
  INTERRUPTJNI_LOG(logDEBUG) << "digitalSourceHandle = " << digitalSourceHandle;
  INTERRUPTJNI_LOG(logDEBUG) << "analogTriggerType = " << analogTriggerType;

  int32_t status = 0;
  HAL_RequestInterrupts((HAL_InterruptHandle)interruptHandle,
                        (HAL_Handle)digitalSourceHandle,
                        (HAL_AnalogTriggerType)analogTriggerType, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    attachInterruptHandler
 * Signature: (ILjava/lang/Object;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_attachInterruptHandler
  (JNIEnv* env, jclass, jint interruptHandle, jobject handler, jobject param)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI attachInterruptHandler";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;

  jclass cls = env->GetObjectClass(handler);
  INTERRUPTJNI_LOG(logDEBUG) << "class = " << cls;
  if (cls == 0) {
    INTERRUPTJNI_LOG(logERROR) << "Error getting java class";
    assert(false);
    return;
  }
  jmethodID mid = env->GetMethodID(cls, "apply", "(ILjava/lang/Object;)V");
  INTERRUPTJNI_LOG(logDEBUG) << "method = " << mid;
  if (mid == 0) {
    INTERRUPTJNI_LOG(logERROR) << "Error getting java method ID";
    assert(false);
    return;
  }

  InterruptJNI* intr = new InterruptJNI;
  intr->Start();
  intr->SetFunc(env, handler, mid, param);

  INTERRUPTJNI_LOG(logDEBUG) << "InterruptThreadJNI Ptr = " << intr;

  int32_t status = 0;
  HAL_AttachInterruptHandler((HAL_InterruptHandle)interruptHandle,
                             interruptHandler, intr, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    setInterruptUpSourceEdge
 * Signature: (IZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_setInterruptUpSourceEdge
  (JNIEnv* env, jclass, jint interruptHandle, jboolean risingEdge,
   jboolean fallingEdge)
{
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI setInterruptUpSourceEdge";
  INTERRUPTJNI_LOG(logDEBUG)
      << "Interrupt Handle = " << (HAL_InterruptHandle)interruptHandle;
  INTERRUPTJNI_LOG(logDEBUG)
      << "Rising Edge = " << static_cast<bool>(risingEdge);
  INTERRUPTJNI_LOG(logDEBUG)
      << "Falling Edge = " << static_cast<bool>(fallingEdge);

  int32_t status = 0;
  HAL_SetInterruptUpSourceEdge((HAL_InterruptHandle)interruptHandle, risingEdge,
                               fallingEdge, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
