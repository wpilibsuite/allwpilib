// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WPIUtilJNI.h"

#include <jni.h>

#include "edu_wpi_first_util_WPIUtilJNI.h"
#include "wpi/DataLog.h"
#include "wpi/FileLogger.h"
#include "wpi/RawFrame.h"
#include "wpi/RuntimeCheck.h"
#include "wpi/Synchronization.h"
#include "wpi/jni_util.h"
#include "wpi/print.h"
#include "wpi/timestamp.h"

using namespace wpi::java;

static bool mockTimeEnabled = false;
static uint64_t mockNow = 0;

static JException illegalArgEx;
static JException indexOobEx;
static JException interruptedEx;
static JException ioEx;
static JException nullPointerEx;
static JException msvcRuntimeEx;

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgEx},
    {"java/lang/IndexOutOfBoundsException", &indexOobEx},
    {"java/lang/InterruptedException", &interruptedEx},
    {"java/io/IOException", &ioEx},
    {"java/lang/NullPointerException", &nullPointerEx},
    {"edu/wpi/first/util/MsvcRuntimeException", &msvcRuntimeEx}};

void wpi::ThrowIllegalArgumentException(JNIEnv* env, std::string_view msg) {
  illegalArgEx.Throw(env, msg);
}

void wpi::ThrowIndexOobException(JNIEnv* env, std::string_view msg) {
  indexOobEx.Throw(env, msg);
}

void wpi::ThrowIOException(JNIEnv* env, std::string_view msg) {
  ioEx.Throw(env, msg);
}

void wpi::ThrowNullPointerException(JNIEnv* env, std::string_view msg) {
  nullPointerEx.Throw(env, msg);
}

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  for (auto& c : exceptions) {
    *c.cls = JException(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }

  for (auto& c : exceptions) {
    c.cls->free(env);
  }
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    checkMsvcRuntime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_checkMsvcRuntime
  (JNIEnv* env, jclass)
{
  uint32_t foundMajor;
  uint32_t foundMinor;
  uint32_t expectedMajor;
  uint32_t expectedMinor;
  WPI_String runtimePath;

  if (!WPI_IsRuntimeValid(&foundMajor, &foundMinor, &expectedMajor,
                          &expectedMinor, &runtimePath)) {
    static jmethodID ctor =
        env->GetMethodID(msvcRuntimeEx, "<init>", "(IIIILjava/lang/String;)V");
    jstring jmsvcruntime = MakeJString(env, wpi::to_string_view(&runtimePath));
    jobject exception =
        env->NewObject(msvcRuntimeEx, ctor, foundMajor, foundMinor,
                       expectedMajor, expectedMinor, jmsvcruntime);
    WPI_FreeString(&runtimePath);
    env->Throw(static_cast<jthrowable>(exception));
  }
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    writeStderr
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_writeStderr
  (JNIEnv* env, jclass, jstring str)
{
  wpi::print(stderr, "{}", JStringRef{env, str}.str());
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    enableMockTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_enableMockTime
  (JNIEnv*, jclass)
{
#ifdef __FRC_ROBORIO__
  wpi::print(stderr, "WPIUtil: Mocking time is not available on the Rio\n");
#else
  mockTimeEnabled = true;
  wpi::SetNowImpl([] { return mockNow; });
#endif
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    disableMockTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_disableMockTime
  (JNIEnv*, jclass)
{
  mockTimeEnabled = false;
  wpi::SetNowImpl(nullptr);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    setMockTime
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_setMockTime
  (JNIEnv*, jclass, jlong time)
{
  mockNow = time;
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_now
  (JNIEnv*, jclass)
{
  if (mockTimeEnabled) {
    return mockNow;
  } else {
    return wpi::Now();
  }
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    getSystemTime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_getSystemTime
  (JNIEnv*, jclass)
{
  return wpi::GetSystemTime();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    createEvent
 * Signature: (ZZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_createEvent
  (JNIEnv*, jclass, jboolean manualReset, jboolean initialState)
{
  return wpi::CreateEvent(manualReset, initialState);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    destroyEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_destroyEvent
  (JNIEnv*, jclass, jint eventHandle)
{
  wpi::DestroyEvent(eventHandle);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    setEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_setEvent
  (JNIEnv*, jclass, jint eventHandle)
{
  wpi::SetEvent(eventHandle);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    resetEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_resetEvent
  (JNIEnv*, jclass, jint eventHandle)
{
  wpi::ResetEvent(eventHandle);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    createSemaphore
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_createSemaphore
  (JNIEnv*, jclass, jint initialCount, jint maximumCount)
{
  return wpi::CreateSemaphore(initialCount, maximumCount);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    destroySemaphore
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_destroySemaphore
  (JNIEnv*, jclass, jint semHandle)
{
  wpi::DestroySemaphore(semHandle);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    releaseSemaphore
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_releaseSemaphore
  (JNIEnv*, jclass, jint semHandle, jint releaseCount)
{
  return wpi::ReleaseSemaphore(semHandle, releaseCount);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    waitForObject
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_waitForObject
  (JNIEnv* env, jclass, jint handle)
{
  if (!wpi::WaitForObject(handle)) {
    interruptedEx.Throw(env, "WaitForObject interrupted");
  }
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    waitForObjectTimeout
 * Signature: (ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_waitForObjectTimeout
  (JNIEnv* env, jclass, jint handle, jdouble timeout)
{
  bool timedOut;
  if (!wpi::WaitForObject(handle, timeout, &timedOut) && !timedOut) {
    interruptedEx.Throw(env, "WaitForObject interrupted");
    return false;
  }
  return timedOut;
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    waitForObjects
 * Signature: ([I)[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_waitForObjects
  (JNIEnv* env, jclass, jintArray handles)
{
  JSpan<const jint> handlesArr{env, handles};
  wpi::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(handlesArr.size());
  std::span<const WPI_Handle> handlesArr2{
      reinterpret_cast<const WPI_Handle*>(handlesArr.data()),
      handlesArr.size()};

  auto signaled = wpi::WaitForObjects(handlesArr2, signaledBuf);
  if (signaled.empty()) {
    interruptedEx.Throw(env, "WaitForObjects interrupted");
    return nullptr;
  }
  return MakeJIntArray(env, signaled);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    waitForObjectsTimeout
 * Signature: ([ID)[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_waitForObjectsTimeout
  (JNIEnv* env, jclass, jintArray handles, jdouble timeout)
{
  JSpan<const jint> handlesArr{env, handles};
  wpi::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(handlesArr.size());
  std::span<const WPI_Handle> handlesArr2{
      reinterpret_cast<const WPI_Handle*>(handlesArr.data()),
      handlesArr.size()};

  bool timedOut;
  auto signaled =
      wpi::WaitForObjects(handlesArr2, signaledBuf, timeout, &timedOut);
  if (signaled.empty() && !timedOut) {
    interruptedEx.Throw(env, "WaitForObjects interrupted");
    return nullptr;
  }
  return MakeJIntArray(env, signaled);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    allocateRawFrame
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_allocateRawFrame
  (JNIEnv*, jclass)
{
  return reinterpret_cast<jlong>(new wpi::RawFrame);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    freeRawFrame
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_freeRawFrame
  (JNIEnv*, jclass, jlong frame)
{
  delete reinterpret_cast<wpi::RawFrame*>(frame);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    getRawFrameDataPtr
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_getRawFrameDataPtr
  (JNIEnv* env, jclass, jlong frame)
{
  auto* f = reinterpret_cast<wpi::RawFrame*>(frame);
  if (!f) {
    wpi::ThrowNullPointerException(env, "frame is null");
    return 0;
  }
  return reinterpret_cast<jlong>(f->data);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    setRawFrameData
 * Signature: (JLjava/lang/Object;IIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_setRawFrameData
  (JNIEnv* env, jclass, jlong frame, jobject data, jint size, jint width,
   jint height, jint stride, jint pixelFormat)
{
  auto* f = reinterpret_cast<wpi::RawFrame*>(frame);
  if (!f) {
    wpi::ThrowNullPointerException(env, "frame is null");
    return;
  }
  auto buf = env->GetDirectBufferAddress(data);
  if (!buf) {
    wpi::ThrowNullPointerException(env, "data is null");
    return;
  }
  // there's no way to free a passed-in direct byte buffer
  f->SetData(buf, size, env->GetDirectBufferCapacity(data), nullptr,
             [](void*, void*, size_t) {});
  f->width = width;
  f->height = height;
  f->stride = stride;
  f->pixelFormat = pixelFormat;
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    setRawFrameTime
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_setRawFrameTime
  (JNIEnv* env, jclass, jlong frame, jlong time, jint timeSource)
{
  auto* f = reinterpret_cast<wpi::RawFrame*>(frame);
  if (!f) {
    wpi::ThrowNullPointerException(env, "frame is null");
    return;
  }
  f->timestamp = time;
  f->timestampSrc = timeSource;
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    setRawFrameInfo
 * Signature: (JIIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_setRawFrameInfo
  (JNIEnv* env, jclass, jlong frame, jint size, jint width, jint height,
   jint stride, jint pixelFormat)
{
  auto* f = reinterpret_cast<wpi::RawFrame*>(frame);
  if (!f) {
    wpi::ThrowNullPointerException(env, "frame is null");
    return;
  }
  f->width = width;
  f->height = height;
  f->stride = stride;
  f->pixelFormat = pixelFormat;
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    createFileLogger
 * Signature: (Ljava/lang/String;JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_createFileLogger
  (JNIEnv* env, jclass, jstring file, jlong log, jstring key)
{
  if (!file) {
    wpi::ThrowNullPointerException(env, "file is null");
    return 0;
  }
  auto* f = reinterpret_cast<wpi::log::DataLog*>(log);
  if (!f) {
    wpi::ThrowNullPointerException(env, "log is null");
    return 0;
  }
  if (!key) {
    wpi::ThrowNullPointerException(env, "key is null");
    return 0;
  }
  return reinterpret_cast<jlong>(
      new wpi::FileLogger{JStringRef{env, file}, *f, JStringRef{env, key}});
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    freeFileLogger
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_freeFileLogger
  (JNIEnv* env, jclass, jlong fileTail)
{
  delete reinterpret_cast<wpi::FileLogger*>(fileTail);
}
}  // extern "C"
