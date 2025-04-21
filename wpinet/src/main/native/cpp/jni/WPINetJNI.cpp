// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/jni_util.h>

#include "../MulticastHandleManager.h"
#include "edu_wpi_first_net_WPINetJNI.h"
#include "wpinet/MulticastServiceAnnouncer.h"
#include "wpinet/MulticastServiceResolver.h"
#include "wpinet/PortForwarder.h"
#include "wpinet/WebServer.h"

using namespace wpi::java;

static JClass serviceDataCls;
static JGlobal<jobjectArray> serviceDataEmptyArray;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  serviceDataCls = JClass{env, "edu/wpi/first/net/ServiceData"};
  if (!serviceDataCls) {
    return JNI_ERR;
  }

  serviceDataEmptyArray = JGlobal<jobjectArray>{
      env, env->NewObjectArray(0, serviceDataCls, nullptr)};
  if (serviceDataEmptyArray == nullptr) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }

  serviceDataEmptyArray.free(env);
  serviceDataCls.free(env);
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    addPortForwarder
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_addPortForwarder
  (JNIEnv* env, jclass, jint port, jstring remoteHost, jint remotePort)
{
  wpi::PortForwarder::GetInstance().Add(static_cast<unsigned int>(port),
                                        JStringRef{env, remoteHost}.str(),
                                        static_cast<unsigned int>(remotePort));
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    removePortForwarder
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_removePortForwarder
  (JNIEnv* env, jclass, jint port)
{
  wpi::PortForwarder::GetInstance().Remove(port);
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    startWebServer
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_startWebServer
  (JNIEnv* env, jclass, jint port, jstring path)
{
  wpi::WebServer::GetInstance().Start(static_cast<unsigned int>(port),
                                      JStringRef{env, path}.str());
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    stopWebServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_stopWebServer
  (JNIEnv* env, jclass, jint port)
{
  wpi::WebServer::GetInstance().Stop(port);
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    createMulticastServiceAnnouncer
 * Signature: (Ljava/lang/String;Ljava/lang/String;I[Ljava/lang/Object;[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_net_WPINetJNI_createMulticastServiceAnnouncer
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
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    freeMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_freeMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.announcers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    startMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_startMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Start();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    stopMulticastServiceAnnouncer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_stopMulticastServiceAnnouncer
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Stop();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    getMulticastServiceAnnouncerHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_net_WPINetJNI_getMulticastServiceAnnouncerHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  return announcer->HasImplementation();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    createMulticastServiceResolver
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_net_WPINetJNI_createMulticastServiceResolver
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
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    freeMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_freeMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.resolvers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    startMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_startMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Start();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    stopMulticastServiceResolver
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_net_WPINetJNI_stopMulticastServiceResolver
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Stop();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    getMulticastServiceResolverHasImplementation
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_net_WPINetJNI_getMulticastServiceResolverHasImplementation
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->HasImplementation();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    getMulticastServiceResolverEventHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_net_WPINetJNI_getMulticastServiceResolverEventHandle
  (JNIEnv* env, jclass, jint handle)
{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->GetEventHandle();
}

/*
 * Class:     edu_wpi_first_net_WPINetJNI
 * Method:    getMulticastServiceResolverData
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_net_WPINetJNI_getMulticastServiceResolverData
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
