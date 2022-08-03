// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "edu_wpi_first_util_WPIUtilJNI.h"
#include "wpi/Synchronization.h"
#include "wpi/jni_util.h"
#include "wpi/timestamp.h"

using namespace wpi::java;

static bool mockTimeEnabled = false;
static uint64_t mockNow = 0;

static JException interruptedEx;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  interruptedEx = JException(env, "java/lang/InterruptedException");
  if (!interruptedEx) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }

  interruptedEx.free(env);
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
  mockTimeEnabled = true;
  wpi::SetNowImpl([] { return mockNow; });
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
  JIntArrayRef handlesArr{env, handles};
  wpi::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(handlesArr.size());
  wpi::span<const WPI_Handle> handlesArr2{
      reinterpret_cast<const WPI_Handle*>(handlesArr.array().data()),
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
  JIntArrayRef handlesArr{env, handles};
  wpi::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(handlesArr.size());
  wpi::span<const WPI_Handle> handlesArr2{
      reinterpret_cast<const WPI_Handle*>(handlesArr.array().data()),
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
 * Method:    createMulticastServiceAnnouncer
 * Signature: (Ljava/lang/String;Ljava/lang/String;I[Ljava/lang/Object;[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_createMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jstring serviceName, jstring serviceType, jint port,
   jobjectArray keys, jobjectArray values)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};

  JStringRef serviceNameRef{env, serviceName};
  JStringRef serviceTypeRef{env, serviceType};

  wpi::SmallVector<std::pair<std::string, std::string>, 8> txtVec;

  if (keys != nullptr && values != nullptr) {
    size_t keysLen = env->GetArrayLength(keys);

    txtVec.reserve(keysLen);
    for (size_t i = 0; i < keysLen; i++) {
      JLocal<jstring> key{
          env, static_cast<jstring>(env->GetObjectArrayElement(keys, i))};
      JLocal<jstring> value{
          env, static_cast<jstring>(env->GetObjectArrayElement(values, i))};

      txtVec.emplace_back(std::pair<std::string, std::string>{
          JStringRef{env, key}.str(), JStringRef{env, value}.str()});
    }
  }

  auto announcer = std::make_unique<wpi::MulticastServiceAnnouncer>(
      serviceNameRef.str(), serviceTypeRef.str(), port, txtVec);

  size_t index = manager.handleIds.emplace_back(1);

  manager.announcers[index] = std::move(announcer);

  return static_cast<jint>(index);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    freeMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_freeMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.announcers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    startMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_startMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Start();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    stopMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_stopMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Stop();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    getMulticastServiceAnnouncerHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_getMulticastServiceAnnouncerHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  return announcer->HasImplementation();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    createMulticastServiceResolver
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_createMulticastServiceResolver
  (JNIEnv* env, jclass, jstring serviceType)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  JStringRef serviceTypeRef{env, serviceType};

  auto resolver =
      std::make_unique<wpi::MulticastServiceResolver>(serviceTypeRef.str());

  size_t index = manager.handleIds.emplace_back(2);

  manager.resolvers[index] = std::move(resolver);

  return static_cast<jint>(index);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    freeMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_freeMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.resolvers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    startMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_startMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Start();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    stopMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_stopMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Stop();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    getMulticastServiceResolverHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_getMulticastServiceResolverHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->HasImplementation();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    getMulticastServiceResolverEventHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_getMulticastServiceResolverEventHandle
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->GetEventHandle();
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    getMulticastServiceResolverData
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_getMulticastServiceResolverData
  (JNIEnv* env, jclass, jint handle)
{
  static jmethodID constructor =
      env->GetMethodID(serviceDataCls, "<init>",
                       "(JILjava/lang/String;Ljava/lang/String;[Ljava/lang/"
                       "String;[Ljava/lang/String;)V");
  auto& manager = wpi::GetMulticastManager();
  std::vector<wpi::MulticastServiceResolver::ServiceData> allData;
  {
    std::scoped_lock lock{manager.mutex};
    auto& resolver = manager.resolvers[handle];
    allData = resolver->GetData();
  }
  if (allData.empty()) {
    return serviceDataEmptyArray;
  }

  JLocal<jobjectArray> returnData{
      env, env->NewObjectArray(allData.size(), serviceDataCls, nullptr)};

  for (auto&& data : allData) {
    JLocal<jstring> serviceName{env, MakeJString(env, data.serviceName)};
    JLocal<jstring> hostName{env, MakeJString(env, data.hostName)};

    wpi::SmallVector<std::string_view, 8> keysRef;
    wpi::SmallVector<std::string_view, 8> valuesRef;

    size_t index = 0;
    for (auto&& txt : data.txt) {
      keysRef.emplace_back(txt.first);
      valuesRef.emplace_back(txt.second);
    }

    JLocal<jobjectArray> keys{env, MakeJStringArray(env, keysRef)};
    JLocal<jobjectArray> values{env, MakeJStringArray(env, valuesRef)};

    JLocal<jobject> dataItem{
        env, env->NewObject(serviceDataCls, constructor,
                            static_cast<jlong>(data.ipv4Address),
                            static_cast<jint>(data.port), serviceName.obj(),
                            hostName.obj(), keys.obj(), values.obj())};

    env->SetObjectArrayElement(returnData, index, dataItem);
    index++;
  }

  return returnData;
}

}  // extern "C"
