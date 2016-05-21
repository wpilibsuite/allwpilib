/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "Log.hpp"

#include "HAL/Interrupts.hpp"
#include "HALUtil.h"
#include "SafeThread.h"
#include "edu_wpi_first_wpilibj_hal_InterruptJNI.h"

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
class InterruptThreadJNI : public SafeThread {
 public:
  void Main();

  bool m_notify = false;
  uint32_t m_mask = 0;
  jobject m_func = nullptr;
  jmethodID m_mid;
  jobject m_param = nullptr;
};

class InterruptJNI : public SafeThreadOwner<InterruptThreadJNI> {
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
  jint rs = jvm->AttachCurrentThreadAsDaemon((void**)&env, &args);
  if (rs != JNI_OK) return;

  std::unique_lock<std::mutex> lock(m_mutex);
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
    env->CallVoidMethod(func, mid, (jint)mask, param);
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
      env->ExceptionClear();
    }
    lock.lock();
  }

  // free global references
  if (m_func) env->DeleteGlobalRef(m_func);
  if (m_param) env->DeleteGlobalRef(m_param);

  jvm->DetachCurrentThread();
}

void interruptHandler(uint32_t mask, void* param) {
  ((InterruptJNI*)param)->Notify(mask);
}

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    initializeInterrupts
 * Signature: (IZ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_initializeInterrupts(
    JNIEnv* env, jclass, jint interruptIndex, jboolean watcher) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI initializeInterrupts";
  INTERRUPTJNI_LOG(logDEBUG) << "interruptIndex = " << interruptIndex;
  INTERRUPTJNI_LOG(logDEBUG) << "watcher = " << (bool)watcher;

  int32_t status = 0;
  void* interrupt = initializeInterrupts(interruptIndex, watcher, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << interrupt;
  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
  return (jlong)interrupt;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    cleanInterrupts
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_cleanInterrupts(
    JNIEnv* env, jclass, jlong interrupt_pointer) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI cleanInterrupts";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

  int32_t status = 0;
  cleanInterrupts((void*)interrupt_pointer, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    waitForInterrupt
 * Signature: (JD)V
 */
JNIEXPORT int JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_waitForInterrupt(
    JNIEnv* env, jclass, jlong interrupt_pointer, jdouble timeout,
    jboolean ignorePrevious) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI waitForInterrupt";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

  int32_t status = 0;
  int result = waitForInterrupt((void*)interrupt_pointer, timeout,
                                ignorePrevious, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    enableInterrupts
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_enableInterrupts(
    JNIEnv* env, jclass, jlong interrupt_pointer) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI enableInterrupts";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

  int32_t status = 0;
  enableInterrupts((void*)interrupt_pointer, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    disableInterrupts
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_disableInterrupts(
    JNIEnv* env, jclass, jlong interrupt_pointer) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI disableInterrupts";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

  int32_t status = 0;
  disableInterrupts((void*)interrupt_pointer, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    readRisingTimestamp
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_readRisingTimestamp(
    JNIEnv* env, jclass, jlong interrupt_pointer) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI readRisingTimestamp";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

  int32_t status = 0;
  jdouble timeStamp = readRisingTimestamp((void*)interrupt_pointer, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return timeStamp;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    readFallingTimestamp
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_readFallingTimestamp(
    JNIEnv* env, jclass, jlong interrupt_pointer) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI readFallingTimestamp";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

  int32_t status = 0;
  jdouble timeStamp = readFallingTimestamp((void*)interrupt_pointer, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return timeStamp;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    requestInterrupts
 * Signature: (JBIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_requestInterrupts(
    JNIEnv* env, jclass, jlong interrupt_pointer, jbyte routing_module,
    jint routing_pin, jboolean routing_analog_trigger) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI requestInterrupts";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;
  INTERRUPTJNI_LOG(logDEBUG) << "routing module = " << (jint)routing_module;
  INTERRUPTJNI_LOG(logDEBUG) << "routing pin = " << routing_pin;
  INTERRUPTJNI_LOG(logDEBUG) << "routing analog trigger = "
                             << (jint)routing_analog_trigger;

  int32_t status = 0;
  requestInterrupts((void*)interrupt_pointer, (uint8_t)routing_module,
                    (uint32_t)routing_pin, routing_analog_trigger, &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    attachInterruptHandler
 * Signature:
 * (JLedu/wpi/first/wpilibj/hal/InterruptJNI/InterruptHandlerFunction;Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_attachInterruptHandler(
    JNIEnv* env, jclass, jlong interrupt_pointer, jobject handler,
    jobject param) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI attachInterruptHandler";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

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
  attachInterruptHandler((void*)interrupt_pointer, interruptHandler, intr,
                         &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    setInterruptUpSourceEdge
 * Signature: (JZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_InterruptJNI_setInterruptUpSourceEdge(
    JNIEnv* env, jclass, jlong interrupt_pointer, jboolean risingEdge,
    jboolean fallingEdge) {
  INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI setInterruptUpSourceEdge";
  INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;
  INTERRUPTJNI_LOG(logDEBUG) << "Rising Edge = " << (bool)risingEdge;
  INTERRUPTJNI_LOG(logDEBUG) << "Falling Edge = " << (bool)fallingEdge;

  int32_t status = 0;
  setInterruptUpSourceEdge((void*)interrupt_pointer, risingEdge, fallingEdge,
                           &status);

  INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
