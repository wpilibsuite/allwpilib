// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WPIUtilJNI.hpp"

#include <jni.h>

#include "org_wpilib_util_AlertDataJNI.h"
#include "org_wpilib_util_WPIUtilJNI.h"
#include "wpi/util/Alert.h"
#include "wpi/util/RawFrame.hpp"
#include "wpi/util/RuntimeCheck.h"
#include "wpi/util/Synchronization.hpp"
#include "wpi/util/jni_util.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.hpp"

static_assert(WPI_ALERT_HIGH == org_wpilib_util_AlertDataJNI_LEVEL_HIGH);
static_assert(WPI_ALERT_MEDIUM == org_wpilib_util_AlertDataJNI_LEVEL_MEDIUM);
static_assert(WPI_ALERT_LOW == org_wpilib_util_AlertDataJNI_LEVEL_LOW);

using namespace wpi::util::java;

static bool mockTimeEnabled = false;
static uint64_t mockNow = 0;

static JException illegalArgEx;
static JException indexOobEx;
static JException interruptedEx;
static JException ioEx;
static JException nullPointerEx;
static JException msvcRuntimeEx;
static JClass alertEx;
static JClass alertInfoCls;

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgEx},
    {"java/lang/IndexOutOfBoundsException", &indexOobEx},
    {"java/lang/InterruptedException", &interruptedEx},
    {"java/io/IOException", &ioEx},
    {"java/lang/NullPointerException", &nullPointerEx},
    {"org/wpilib/util/runtime/MsvcRuntimeException", &msvcRuntimeEx}};

static bool CheckAlertStatus(JNIEnv* env, int32_t status) {
  if (status == 0) {
    return true;
  }
  static jmethodID func =
      env->GetMethodID(alertEx, "<init>", "(Ljava/lang/String;I)V");
  jobject exception;
  if (status == ALERT_ALREADY_ALLOCATED) {
    exception = env->NewObject(
        alertEx, func, MakeJString(env, "Alert already allocated"), status);
  } else {
    exception = env->NewObject(
        alertEx, func, MakeJString(env, "Alert operation failed"), status);
  }
  env->Throw(static_cast<jthrowable>(exception));
  return false;
}

void wpi::util::ThrowIllegalArgumentException(JNIEnv* env,
                                              std::string_view msg) {
  illegalArgEx.Throw(env, msg);
}

void wpi::util::ThrowIndexOobException(JNIEnv* env, std::string_view msg) {
  indexOobEx.Throw(env, msg);
}

void wpi::util::ThrowIOException(JNIEnv* env, std::string_view msg) {
  ioEx.Throw(env, msg);
}

void wpi::util::ThrowNullPointerException(JNIEnv* env, std::string_view msg) {
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

  alertInfoCls = JClass(env, "org/wpilib/util/AlertDataJNI$AlertInfo");
  if (!alertInfoCls) {
    return JNI_ERR;
  }

  alertEx = JClass(env, "org/wpilib/util/AlertException");
  if (!alertEx) {
    return JNI_ERR;
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
  alertInfoCls.free(env);
  alertEx.free(env);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    checkMsvcRuntime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_checkMsvcRuntime
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
    jstring jmsvcruntime =
        MakeJString(env, wpi::util::to_string_view(&runtimePath));
    jobject exception =
        env->NewObject(msvcRuntimeEx, ctor, foundMajor, foundMinor,
                       expectedMajor, expectedMinor, jmsvcruntime);
    WPI_FreeString(&runtimePath);
    env->Throw(static_cast<jthrowable>(exception));
  }
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    writeStderr
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_writeStderr
  (JNIEnv* env, jclass, jstring str)
{
  wpi::util::print(stderr, "{}", JStringRef{env, str}.str());
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    enableMockTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_enableMockTime
  (JNIEnv*, jclass)
{
#ifdef __FIRST_SYSTEMCORE__
  wpi::util::print(stderr,
                   "WPIUtil: Mocking time is not available on systemcore\n");
#else
  mockTimeEnabled = true;
  wpi::util::SetNowImpl([] { return mockNow; });
#endif
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    disableMockTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_disableMockTime
  (JNIEnv*, jclass)
{
  mockTimeEnabled = false;
  wpi::util::SetNowImpl(nullptr);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setMockTime
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setMockTime
  (JNIEnv*, jclass, jlong time)
{
  mockNow = time;
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_util_WPIUtilJNI_now
  (JNIEnv*, jclass)
{
  if (mockTimeEnabled) {
    return mockNow;
  } else {
    return wpi::util::Now();
  }
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    getSystemTime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_util_WPIUtilJNI_getSystemTime
  (JNIEnv*, jclass)
{
  return wpi::util::GetSystemTime();
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    createAlert
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_util_WPIUtilJNI_createAlert
  (JNIEnv* env, jclass, jstring group, jstring id, jstring text, jint level)
{
  wpi::util::java::JStringRef jgroup{env, group};
  wpi::util::java::JStringRef jid{env, id};
  wpi::util::java::JStringRef jtext{env, text};
  WPI_String wpiGroup = wpi::util::make_string(jgroup);
  WPI_String wpiId = wpi::util::make_string(jid);
  WPI_String wpiText = wpi::util::make_string(jtext);
  WPI_AlertHandle alertHandle = WPI_INVALID_HANDLE;
  int32_t status =
      WPI_CreateAlert(&wpiGroup, &wpiId, &wpiText, level, &alertHandle);
  if (!CheckAlertStatus(env, status) || alertHandle <= 0) {
    return 0;
  }
  return alertHandle;
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    destroyAlert
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_destroyAlert
  (JNIEnv*, jclass, jint alertHandle)
{
  if (alertHandle != WPI_INVALID_HANDLE) {
    WPI_DestroyAlert(alertHandle);
  }
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setAlertActive
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setAlertActive
  (JNIEnv* env, jclass, jint alertHandle, jboolean active)
{
  CheckAlertStatus(env, WPI_SetAlertActive(alertHandle, active));
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    isAlertActive
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_util_WPIUtilJNI_isAlertActive
  (JNIEnv* env, jclass, jint alertHandle)
{
  int32_t active = 0;
  CheckAlertStatus(env, WPI_IsAlertActive(alertHandle, &active));
  return active;
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setAlertText
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setAlertText
  (JNIEnv* env, jclass, jint alertHandle, jstring text)
{
  wpi::util::java::JStringRef jtext{env, text};
  WPI_String wpiText = wpi::util::make_string(jtext);
  CheckAlertStatus(env, WPI_SetAlertText(alertHandle, &wpiText));
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    getAlertText
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_util_WPIUtilJNI_getAlertText
  (JNIEnv* env, jclass, jint alertHandle)
{
  WPI_String text;
  if (!CheckAlertStatus(env, WPI_GetAlertText(alertHandle, &text))) {
    return nullptr;
  }
  jstring rv = MakeJString(env, wpi::util::to_string_view(&text));
  WPI_FreeString(&text);
  return rv;
}

static jobject MakeAlertInfoJava(JNIEnv* env, const WPI_AlertInfo& info) {
  static jmethodID func = env->GetMethodID(
      alertInfoCls, "<init>",
      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JI)V");
  return env->NewObject(
      alertInfoCls, func,
      MakeJString(env, wpi::util::to_string_view(&info.group)),
      MakeJString(env, wpi::util::to_string_view(&info.id)),
      MakeJString(env, wpi::util::to_string_view(&info.text)),
      static_cast<jlong>(info.activeStartTime), static_cast<jint>(info.level));
}

/*
 * Class:     org_wpilib_util_AlertDataJNI
 * Method:    getNumAlerts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_util_AlertDataJNI_getNumAlerts
  (JNIEnv*, jclass)
{
  return WPI_GetNumAlerts();
}

/*
 * Class:     org_wpilib_util_AlertDataJNI
 * Method:    getAlerts
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_util_AlertDataJNI_getAlerts
  (JNIEnv* env, jclass)
{
  int32_t allocLen = WPI_GetNumAlerts();
  if (allocLen <= 0) {
    return env->NewObjectArray(0, alertInfoCls, nullptr);
  }
  WPI_AlertInfo* arr = new WPI_AlertInfo[allocLen];
  int32_t len = WPI_GetAlerts(arr, allocLen);
  if (len <= 0) {
    delete[] arr;
    return env->NewObjectArray(0, alertInfoCls, nullptr);
  }

  jobjectArray ret = env->NewObjectArray(len, alertInfoCls, nullptr);
  for (int32_t i = 0; i < len; ++i) {
    env->SetObjectArrayElement(ret, i, MakeAlertInfoJava(env, arr[i]));
  }
  WPI_FreeAlerts(arr, len < allocLen ? len : allocLen);
  delete[] arr;
  return ret;
}

/*
 * Class:     org_wpilib_util_AlertDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_AlertDataJNI_resetData
  (JNIEnv*, jclass)
{
  WPI_ResetAlertData();
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    makeEvent
 * Signature: (ZZ)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_util_WPIUtilJNI_makeEvent
  (JNIEnv*, jclass, jboolean manualReset, jboolean initialState)
{
  return wpi::util::MakeEvent(manualReset, initialState);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    destroyEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_destroyEvent
  (JNIEnv*, jclass, jint eventHandle)
{
  wpi::util::DestroyEvent(eventHandle);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setEvent
  (JNIEnv*, jclass, jint eventHandle)
{
  wpi::util::SetEvent(eventHandle);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    resetEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_resetEvent
  (JNIEnv*, jclass, jint eventHandle)
{
  wpi::util::ResetEvent(eventHandle);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    makeSemaphore
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_util_WPIUtilJNI_makeSemaphore
  (JNIEnv*, jclass, jint initialCount, jint maximumCount)
{
  return wpi::util::MakeSemaphore(initialCount, maximumCount);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    destroySemaphore
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_destroySemaphore
  (JNIEnv*, jclass, jint semHandle)
{
  wpi::util::DestroySemaphore(semHandle);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    releaseSemaphore
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_util_WPIUtilJNI_releaseSemaphore
  (JNIEnv*, jclass, jint semHandle, jint releaseCount)
{
  return wpi::util::ReleaseSemaphore(semHandle, releaseCount);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    waitForObject
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_waitForObject
  (JNIEnv* env, jclass, jint handle)
{
  if (!wpi::util::WaitForObject(handle)) {
    interruptedEx.Throw(env, "WaitForObject interrupted");
  }
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    waitForObjectTimeout
 * Signature: (ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_util_WPIUtilJNI_waitForObjectTimeout
  (JNIEnv* env, jclass, jint handle, jdouble timeout)
{
  bool timedOut;
  if (!wpi::util::WaitForObject(handle, timeout, &timedOut) && !timedOut) {
    interruptedEx.Throw(env, "WaitForObject interrupted");
    return false;
  }
  return timedOut;
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    waitForObjects
 * Signature: ([I)[I
 */
JNIEXPORT jintArray JNICALL
Java_org_wpilib_util_WPIUtilJNI_waitForObjects
  (JNIEnv* env, jclass, jintArray handles)
{
  JSpan<const jint> handlesArr{env, handles};
  wpi::util::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(handlesArr.size());
  std::span<const WPI_Handle> handlesArr2{
      reinterpret_cast<const WPI_Handle*>(handlesArr.data()),
      handlesArr.size()};

  auto signaled = wpi::util::WaitForObjects(handlesArr2, signaledBuf);
  if (signaled.empty()) {
    interruptedEx.Throw(env, "WaitForObjects interrupted");
    return nullptr;
  }
  return MakeJIntArray(env, signaled);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    waitForObjectsTimeout
 * Signature: ([ID)[I
 */
JNIEXPORT jintArray JNICALL
Java_org_wpilib_util_WPIUtilJNI_waitForObjectsTimeout
  (JNIEnv* env, jclass, jintArray handles, jdouble timeout)
{
  JSpan<const jint> handlesArr{env, handles};
  wpi::util::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(handlesArr.size());
  std::span<const WPI_Handle> handlesArr2{
      reinterpret_cast<const WPI_Handle*>(handlesArr.data()),
      handlesArr.size()};

  bool timedOut;
  auto signaled =
      wpi::util::WaitForObjects(handlesArr2, signaledBuf, timeout, &timedOut);
  if (signaled.empty() && !timedOut) {
    interruptedEx.Throw(env, "WaitForObjects interrupted");
    return nullptr;
  }
  return MakeJIntArray(env, signaled);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    allocateRawFrame
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_util_WPIUtilJNI_allocateRawFrame
  (JNIEnv*, jclass)
{
  return reinterpret_cast<jlong>(new wpi::util::RawFrame);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    freeRawFrame
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_freeRawFrame
  (JNIEnv*, jclass, jlong frame)
{
  delete reinterpret_cast<wpi::util::RawFrame*>(frame);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    getRawFrameDataPtr
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_util_WPIUtilJNI_getRawFrameDataPtr
  (JNIEnv* env, jclass, jlong frame)
{
  auto* f = reinterpret_cast<wpi::util::RawFrame*>(frame);
  if (!f) {
    wpi::util::ThrowNullPointerException(env, "frame is null");
    return 0;
  }
  return reinterpret_cast<jlong>(f->data);
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setRawFrameData
 * Signature: (JLjava/lang/Object;IIIII)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setRawFrameData
  (JNIEnv* env, jclass, jlong frame, jobject data, jint size, jint width,
   jint height, jint stride, jint pixelFormat)
{
  auto* f = reinterpret_cast<wpi::util::RawFrame*>(frame);
  if (!f) {
    wpi::util::ThrowNullPointerException(env, "frame is null");
    return;
  }
  auto buf = env->GetDirectBufferAddress(data);
  if (!buf) {
    wpi::util::ThrowNullPointerException(env, "data is null");
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
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setRawFrameTime
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setRawFrameTime
  (JNIEnv* env, jclass, jlong frame, jlong time, jint timeSource)
{
  auto* f = reinterpret_cast<wpi::util::RawFrame*>(frame);
  if (!f) {
    wpi::util::ThrowNullPointerException(env, "frame is null");
    return;
  }
  f->timestamp = time;
  f->timestampSrc = timeSource;
}

/*
 * Class:     org_wpilib_util_WPIUtilJNI
 * Method:    setRawFrameInfo
 * Signature: (JIIIII)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_util_WPIUtilJNI_setRawFrameInfo
  (JNIEnv* env, jclass, jlong frame, jint size, jint width, jint height,
   jint stride, jint pixelFormat)
{
  auto* f = reinterpret_cast<wpi::util::RawFrame*>(frame);
  if (!f) {
    wpi::util::ThrowNullPointerException(env, "frame is null");
    return;
  }
  f->width = width;
  f->height = height;
  f->stride = stride;
  f->pixelFormat = pixelFormat;
}
}  // extern "C"
