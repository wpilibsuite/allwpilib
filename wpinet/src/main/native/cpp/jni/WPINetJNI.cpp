// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../MulticastHandleManager.hpp"
#include "org_wpilib_net_WPINetJNI.h"
#include "wpi/net/MulticastServiceAnnouncer.hpp"
#include "wpi/net/MulticastServiceResolver.hpp"
#include "wpi/net/PortForwarder.hpp"
#include "wpi/net/UsbDeviceDetector.hpp"
#include "wpi/net/WebServer.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/UidVector.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

static JavaVM* jvm;
static JClass serviceDataCls;
static JGlobal<jobjectArray> serviceDataEmptyArray;
static JClass usbDeviceCls;
static JClass consumerCls;
static jmethodID usbDeviceConstructor;
static jmethodID consumerAccept;

namespace {

struct UsbDeviceDetectorJniStore {
  void Notify(bool connected, const wpi::net::UsbDevice& device) {
    JNIEnv* env;
    bool didAttachThread = false;
    int getEnvResult =
        jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (getEnvResult == JNI_EDETACHED) {
      JavaVMAttachArgs args{
          .version = JNI_VERSION_1_6,
          .name = const_cast<char*>("UsbDeviceDetector"),
          .group = nullptr,
      };
      if (jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env),
                                           &args) != JNI_OK) {
        return;
      }
      didAttachThread = true;
    } else if (getEnvResult != JNI_OK) {
      return;
    }

    JLocal<jstring> syspath{env, MakeJString(env, device.syspath)};
    JLocal<jstring> usbSyspath{env, MakeJString(env, device.usbSyspath)};
    JLocal<jstring> subsystem{env, MakeJString(env, device.subsystem)};
    JLocal<jstring> deviceType{env, MakeJString(env, device.deviceType)};
    JLocal<jstring> deviceNode{env, MakeJString(env, device.deviceNode)};
    JLocal<jstring> vendorId{env, MakeJString(env, device.vendorId)};
    JLocal<jstring> productId{env, MakeJString(env, device.productId)};
    JLocal<jstring> manufacturer{env, MakeJString(env, device.manufacturer)};
    JLocal<jstring> product{env, MakeJString(env, device.product)};
    JLocal<jstring> serialNumber{env, MakeJString(env, device.serialNumber)};
    JLocal<jobject> javaDevice{
        env,
        env->NewObject(usbDeviceCls, usbDeviceConstructor,
                       static_cast<jint>(device.port), syspath.obj(),
                       usbSyspath.obj(), subsystem.obj(), deviceType.obj(),
                       deviceNode.obj(), vendorId.obj(), productId.obj(),
                       manufacturer.obj(), product.obj(), serialNumber.obj())};

    env->CallVoidMethod(connected ? m_connected : m_disconnected,
                        consumerAccept, javaDevice.obj());
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
      env->ExceptionClear();
    }

    if (didAttachThread) {
      jvm->DetachCurrentThread();
    }
  }

  void Free(JNIEnv* env) {
    std::shared_ptr<wpi::net::UsbDeviceDetector> detectorToFree;
    {
      std::scoped_lock lock{mutex};
      detectorToFree = std::move(detector);
    }
    if (detectorToFree) {
      detectorToFree->Stop();
    }
    m_connected.free(env);
    m_disconnected.free(env);
  }

  std::shared_ptr<wpi::net::UsbDeviceDetector> GetDetector() {
    std::scoped_lock lock{mutex};
    return detector;
  }

  std::mutex mutex;
  JGlobal<jobject> m_connected;
  JGlobal<jobject> m_disconnected;
  std::shared_ptr<wpi::net::UsbDeviceDetector> detector;
};

struct UsbDeviceDetectorJniManager {
  std::mutex mutex;
  wpi::util::UidVector<int, 8> handleIds;
  wpi::util::DenseMap<size_t, std::shared_ptr<UsbDeviceDetectorJniStore>>
      detectors;
};

UsbDeviceDetectorJniManager& GetUsbDeviceDetectorJniManager() {
  static UsbDeviceDetectorJniManager manager;
  return manager;
}

std::shared_ptr<UsbDeviceDetectorJniStore> GetUsbDeviceDetectorJniStore(
    jint handle) {
  auto& manager = GetUsbDeviceDetectorJniManager();
  std::scoped_lock lock{manager.mutex};
  auto found = manager.detectors.find(handle);
  if (found == manager.detectors.end()) {
    return nullptr;
  }
  return found->second;
}

void ShutdownUsbDeviceDetectors(JNIEnv* env) {
  auto& manager = GetUsbDeviceDetectorJniManager();
  std::vector<std::shared_ptr<UsbDeviceDetectorJniStore>> detectors;
  {
    std::scoped_lock lock{manager.mutex};
    for (auto&& [handle, detector] : manager.detectors) {
      if (detector) {
        detectors.emplace_back(std::move(detector));
        manager.handleIds.erase(handle);
      }
    }
  }
  for (auto&& detector : detectors) {
    detector->Free(env);
  }
}

}  // namespace

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  serviceDataCls = JClass{env, "org/wpilib/net/ServiceData"};
  if (!serviceDataCls) {
    return JNI_ERR;
  }

  serviceDataEmptyArray = JGlobal<jobjectArray>{
      env, env->NewObjectArray(0, serviceDataCls, nullptr)};
  if (serviceDataEmptyArray == nullptr) {
    return JNI_ERR;
  }

  usbDeviceCls = JClass{env, "org/wpilib/net/UsbDevice"};
  consumerCls = JClass{env, "java/util/function/Consumer"};
  if (!usbDeviceCls || !consumerCls) {
    return JNI_ERR;
  }
  usbDeviceConstructor = env->GetMethodID(
      usbDeviceCls, "<init>",
      "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/"
      "String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/"
      "lang/String;Ljava/lang/String;Ljava/lang/String;)V");
  consumerAccept =
      env->GetMethodID(consumerCls, "accept", "(Ljava/lang/Object;)V");
  if (!usbDeviceConstructor || !consumerAccept) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }

  ShutdownUsbDeviceDetectors(env);
  serviceDataEmptyArray.free(env);
  serviceDataCls.free(env);
  usbDeviceCls.free(env);
  consumerCls.free(env);
  jvm = nullptr;
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    addPortForwarder
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_addPortForwarder
  (JNIEnv* env, jclass, jint port, jstring remoteHost, jint remotePort)
{
  wpi::net::PortForwarder::GetInstance().Add(
      static_cast<unsigned int>(port), JStringRef{env, remoteHost}.str(),
      static_cast<unsigned int>(remotePort));
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    removePortForwarder
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_removePortForwarder
  (JNIEnv* env, jclass, jint port)
{
  wpi::net::PortForwarder::GetInstance().Remove(port);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    startWebServer
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_startWebServer
  (JNIEnv* env, jclass, jint port, jstring path)
{
  wpi::net::WebServer::GetInstance().Start(static_cast<unsigned int>(port),
                                           JStringRef{env, path}.str());
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    stopWebServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_stopWebServer
  (JNIEnv* env, jclass, jint port)
{
  wpi::net::WebServer::GetInstance().Stop(port);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    createMulticastServiceAnnouncer
 * Signature: (Ljava/lang/String;Ljava/lang/String;I[Ljava/lang/Object;[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_net_WPINetJNI_createMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jstring serviceName, jstring serviceType, jint port,
   jobjectArray keys, jobjectArray values)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};

  JStringRef serviceNameRef{env, serviceName};
  JStringRef serviceTypeRef{env, serviceType};

  wpi::util::SmallVector<std::pair<std::string, std::string>, 8> txtVec;

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

  auto announcer = std::make_unique<wpi::net::MulticastServiceAnnouncer>(
      serviceNameRef.str(), serviceTypeRef.str(), port, txtVec);

  size_t index = manager.handleIds.emplace_back(1);

  manager.announcers[index] = std::move(announcer);

  return static_cast<jint>(index);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    freeMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_freeMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.announcers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    startMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_startMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Start();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    stopMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_stopMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Stop();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    getMulticastServiceAnnouncerHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_net_WPINetJNI_getMulticastServiceAnnouncerHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  return announcer->HasImplementation();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    createMulticastServiceResolver
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_net_WPINetJNI_createMulticastServiceResolver
  (JNIEnv* env, jclass, jstring serviceType)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  JStringRef serviceTypeRef{env, serviceType};

  auto resolver = std::make_unique<wpi::net::MulticastServiceResolver>(
      serviceTypeRef.str());

  size_t index = manager.handleIds.emplace_back(2);

  manager.resolvers[index] = std::move(resolver);

  return static_cast<jint>(index);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    freeMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_freeMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.resolvers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    startMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_startMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Start();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    stopMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_stopMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Stop();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    getMulticastServiceResolverHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_net_WPINetJNI_getMulticastServiceResolverHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->HasImplementation();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    getMulticastServiceResolverEventHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_net_WPINetJNI_getMulticastServiceResolverEventHandle
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::net::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->GetEventHandle();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    getMulticastServiceResolverData
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_net_WPINetJNI_getMulticastServiceResolverData
  (JNIEnv* env, jclass, jint handle)
{
  static jmethodID constructor =
      env->GetMethodID(serviceDataCls, "<init>",
                       "(JILjava/lang/String;Ljava/lang/String;[Ljava/lang/"
                       "String;[Ljava/lang/String;)V");
  auto& manager = wpi::net::GetMulticastManager();
  std::vector<wpi::net::MulticastServiceResolver::ServiceData> allData;
  {
    std::scoped_lock lock{manager.mutex};
    auto& resolver = manager.resolvers[handle];
    allData = resolver->GetData();
  }
  if (allData.empty()) {
    return serviceDataEmptyArray;
  }

  jobjectArray returnData =
      env->NewObjectArray(allData.size(), serviceDataCls, nullptr);

  size_t index = 0;
  for (auto&& data : allData) {
    JLocal<jstring> serviceName{env, MakeJString(env, data.serviceName)};
    JLocal<jstring> hostName{env, MakeJString(env, data.hostName)};

    wpi::util::SmallVector<std::string_view, 8> keysRef;
    wpi::util::SmallVector<std::string_view, 8> valuesRef;

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

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    createUsbDeviceDetector
 * Signature: (ILjava/util/function/Consumer;Ljava/util/function/Consumer;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_net_WPINetJNI_createUsbDeviceDetector
  (JNIEnv* env, jclass, jint port, jobject connected, jobject disconnected)
{
  if (port < 0 || port > 3) {
    JClass illegalArgumentCls{env, "java/lang/IllegalArgumentException"};
    env->ThrowNew(illegalArgumentCls, "USB port must be in the range 0-3");
    illegalArgumentCls.free(env);
    return -1;
  }

  auto store = std::make_shared<UsbDeviceDetectorJniStore>();
  store->m_connected = JGlobal<jobject>{env, connected};
  store->m_disconnected = JGlobal<jobject>{env, disconnected};
  auto storePtr = store.get();
  store->detector = std::make_shared<wpi::net::UsbDeviceDetector>(
      port,
      [storePtr](wpi::net::UsbDevice device) {
        storePtr->Notify(true, device);
      },
      [storePtr](wpi::net::UsbDevice device) {
        storePtr->Notify(false, device);
      });

  auto& manager = GetUsbDeviceDetectorJniManager();
  std::scoped_lock lock{manager.mutex};
  size_t index = manager.handleIds.emplace_back(1);
  manager.detectors[index] = std::move(store);
  return static_cast<jint>(index);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    freeUsbDeviceDetector
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_freeUsbDeviceDetector
  (JNIEnv* env, jclass, jint handle)
{
  std::shared_ptr<UsbDeviceDetectorJniStore> store;
  {
    auto& manager = GetUsbDeviceDetectorJniManager();
    std::scoped_lock lock{manager.mutex};
    auto found = manager.detectors.find(handle);
    if (found == manager.detectors.end() || !found->second) {
      return;
    }
    store = std::move(found->second);
    manager.handleIds.erase(handle);
  }
  store->Free(env);
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    startUsbDeviceDetector
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_net_WPINetJNI_startUsbDeviceDetector
  (JNIEnv* env, jclass, jint handle)
{
  auto store = GetUsbDeviceDetectorJniStore(handle);
  if (!store) {
    return false;
  }
  auto detector = store->GetDetector();
  return detector && detector->Start();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    stopUsbDeviceDetector
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_net_WPINetJNI_stopUsbDeviceDetector
  (JNIEnv* env, jclass, jint handle)
{
  auto store = GetUsbDeviceDetectorJniStore(handle);
  if (!store) {
    return;
  }
  auto detector = store->GetDetector();
  if (detector) {
    detector->Stop();
  }
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    isUsbDeviceDetectorRunning
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_net_WPINetJNI_isUsbDeviceDetectorRunning
  (JNIEnv* env, jclass, jint handle)
{
  auto store = GetUsbDeviceDetectorJniStore(handle);
  if (!store) {
    return false;
  }
  auto detector = store->GetDetector();
  return detector && detector->IsRunning();
}

/*
 * Class:     org_wpilib_net_WPINetJNI
 * Method:    getUsbDeviceDetectorHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_net_WPINetJNI_getUsbDeviceDetectorHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto store = GetUsbDeviceDetectorJniStore(handle);
  if (!store) {
    return false;
  }
  auto detector = store->GetDetector();
  return detector && detector->HasImplementation();
}

}  // extern "C"
