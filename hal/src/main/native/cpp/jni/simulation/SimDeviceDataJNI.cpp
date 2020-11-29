/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SimDeviceDataJNI.h"

#include <jni.h>

#include <utility>

#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "edu_wpi_first_hal_simulation_SimDeviceDataJNI.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "hal/simulation/SimDeviceData.h"

using namespace hal;
using namespace wpi::java;

static JClass simDeviceInfoCls;
static JClass simValueInfoCls;
static JClass simDeviceCallbackCls;
static JClass simValueCallbackCls;
static jmethodID simDeviceCallbackCallback;
static jmethodID simValueCallbackCallback;

namespace {

struct DeviceInfo {
  DeviceInfo(const char* name_, HAL_SimDeviceHandle handle_)
      : name{name_}, handle{handle_} {}
  std::string name;
  HAL_SimValueHandle handle;

  jobject MakeJava(JNIEnv* env) const;
};

struct ValueInfo {
  ValueInfo(const char* name_, HAL_SimValueHandle handle_, bool readonly_,
            const HAL_Value& value_)
      : name{name_}, handle{handle_}, readonly{readonly_}, value{value_} {}
  std::string name;
  HAL_SimValueHandle handle;
  bool readonly;
  HAL_Value value;

  jobject MakeJava(JNIEnv* env) const;
};

}  // namespace

jobject DeviceInfo::MakeJava(JNIEnv* env) const {
  static jmethodID func =
      env->GetMethodID(simDeviceInfoCls, "<init>", "(Ljava/lang/String;I)V");
  return env->NewObject(simDeviceInfoCls, func, MakeJString(env, name),
                        (jint)handle);
}

static std::pair<jlong, jdouble> ToValue12(const HAL_Value& value) {
  jlong value1 = 0;
  jdouble value2 = 0.0;
  switch (value.type) {
    case HAL_BOOLEAN:
      value1 = value.data.v_boolean;
      break;
    case HAL_DOUBLE:
      value2 = value.data.v_double;
      break;
    case HAL_ENUM:
      value1 = value.data.v_enum;
      break;
    case HAL_INT:
      value1 = value.data.v_int;
      break;
    case HAL_LONG:
      value1 = value.data.v_long;
      break;
    default:
      break;
  }
  return std::pair(value1, value2);
}

jobject ValueInfo::MakeJava(JNIEnv* env) const {
  static jmethodID func =
      env->GetMethodID(simValueInfoCls, "<init>", "(Ljava/lang/String;IZIJD)V");
  auto [value1, value2] = ToValue12(value);
  return env->NewObject(simValueInfoCls, func, MakeJString(env, name),
                        (jint)handle, (jboolean)readonly, (jint)value.type,
                        value1, value2);
}

namespace {

class DeviceCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj) { m_call = JGlobal<jobject>(env, obj); }
  void performCallback(const char* name, HAL_SimDeviceHandle handle);
  void free(JNIEnv* env) { m_call.free(env); }
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

class ValueCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj) { m_call = JGlobal<jobject>(env, obj); }
  void performCallback(const char* name, HAL_SimValueHandle handle,
                       bool readonly, const HAL_Value& value);
  void free(JNIEnv* env) { m_call.free(env); }
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

}  // namespace

void DeviceCallbackStore::performCallback(const char* name,
                                          HAL_SimDeviceHandle handle) {
  JNIEnv* env;
  JavaVM* vm = sim::GetJVM();
  bool didAttachThread = false;
  int tryGetEnv = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  if (tryGetEnv == JNI_EDETACHED) {
    // Thread not attached
    didAttachThread = true;
    if (vm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != 0) {
      // Failed to attach, log and return
      wpi::outs() << "Failed to attach\n";
      wpi::outs().flush();
      return;
    }
  } else if (tryGetEnv == JNI_EVERSION) {
    wpi::outs() << "Invalid JVM Version requested\n";
    wpi::outs().flush();
  }

  env->CallVoidMethod(m_call, simDeviceCallbackCallback, MakeJString(env, name),
                      (jint)handle);

  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }

  if (didAttachThread) {
    vm->DetachCurrentThread();
  }
}

void ValueCallbackStore::performCallback(const char* name,
                                         HAL_SimValueHandle handle,
                                         bool readonly,
                                         const HAL_Value& value) {
  JNIEnv* env;
  JavaVM* vm = sim::GetJVM();
  bool didAttachThread = false;
  int tryGetEnv = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  if (tryGetEnv == JNI_EDETACHED) {
    // Thread not attached
    didAttachThread = true;
    if (vm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != 0) {
      // Failed to attach, log and return
      wpi::outs() << "Failed to attach\n";
      wpi::outs().flush();
      return;
    }
  } else if (tryGetEnv == JNI_EVERSION) {
    wpi::outs() << "Invalid JVM Version requested\n";
    wpi::outs().flush();
  }

  auto [value1, value2] = ToValue12(value);
  env->CallVoidMethod(m_call, simValueCallbackCallback, MakeJString(env, name),
                      (jint)handle, (jboolean)readonly, (jint)value.type,
                      value1, value2);

  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }

  if (didAttachThread) {
    vm->DetachCurrentThread();
  }
}

static hal::UnlimitedHandleResource<SIM_JniHandle, DeviceCallbackStore,
                                    hal::HAL_HandleEnum::SimulationJni>*
    deviceCallbackHandles;

namespace {
typedef int32_t (*RegisterDeviceCallbackFunc)(const char* prefix, void* param,
                                              HALSIM_SimDeviceCallback callback,
                                              HAL_Bool initialNotify);
typedef void (*FreeDeviceCallbackFunc)(int32_t uid);
}  // namespace

static SIM_JniHandle AllocateDeviceCallback(
    JNIEnv* env, const char* prefix, jobject callback, jboolean initialNotify,
    RegisterDeviceCallbackFunc createCallback) {
  auto callbackStore = std::make_shared<DeviceCallbackStore>();

  auto handle = deviceCallbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param,
                         HAL_SimDeviceHandle handle) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle jnihandle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = deviceCallbackHandles->Get(jnihandle);
    if (!data) return;

    data->performCallback(name, handle);
  };

  auto id =
      createCallback(prefix, handleAsVoidPtr, callbackFunc, initialNotify);

  callbackStore->setCallbackId(id);

  return handle;
}

static void FreeDeviceCallback(JNIEnv* env, SIM_JniHandle handle,
                               FreeDeviceCallbackFunc freeCallback) {
  auto callback = deviceCallbackHandles->Free(handle);
  freeCallback(callback->getCallbackId());
  callback->free(env);
}

static hal::UnlimitedHandleResource<SIM_JniHandle, ValueCallbackStore,
                                    hal::HAL_HandleEnum::SimulationJni>*
    valueCallbackHandles;

namespace {
typedef void (*FreeValueCallbackFunc)(int32_t uid);
}  // namespace

template <typename THandle>
static SIM_JniHandle AllocateValueCallback(
    JNIEnv* env, THandle h, jobject callback, jboolean initialNotify,
    int32_t (*createCallback)(THandle handle, void* param,
                              HALSIM_SimValueCallback callback,
                              HAL_Bool initialNotify)) {
  auto callbackStore = std::make_shared<ValueCallbackStore>();

  auto handle = valueCallbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param,
                         HAL_SimValueHandle handle, HAL_Bool readonly,
                         const HAL_Value* value) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle jnihandle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = valueCallbackHandles->Get(jnihandle);
    if (!data) return;

    data->performCallback(name, handle, readonly, *value);
  };

  auto id = createCallback(h, handleAsVoidPtr, callbackFunc, initialNotify);

  callbackStore->setCallbackId(id);

  return handle;
}

static void FreeValueCallback(JNIEnv* env, SIM_JniHandle handle,
                              FreeValueCallbackFunc freeCallback) {
  auto callback = valueCallbackHandles->Free(handle);
  freeCallback(callback->getCallbackId());
  callback->free(env);
}

namespace hal {
namespace sim {

bool InitializeSimDeviceDataJNI(JNIEnv* env) {
  simDeviceInfoCls = JClass(
      env, "edu/wpi/first/hal/simulation/SimDeviceDataJNI$SimDeviceInfo");
  if (!simDeviceInfoCls) return false;

  simValueInfoCls =
      JClass(env, "edu/wpi/first/hal/simulation/SimDeviceDataJNI$SimValueInfo");
  if (!simValueInfoCls) return false;

  simDeviceCallbackCls =
      JClass(env, "edu/wpi/first/hal/simulation/SimDeviceCallback");
  if (!simDeviceCallbackCls) return false;

  simDeviceCallbackCallback = env->GetMethodID(simDeviceCallbackCls, "callback",
                                               "(Ljava/lang/String;I)V");
  if (!simDeviceCallbackCallback) return false;

  simValueCallbackCls =
      JClass(env, "edu/wpi/first/hal/simulation/SimValueCallback");
  if (!simValueCallbackCls) return false;

  simValueCallbackCallback = env->GetMethodID(
      simValueCallbackCls, "callbackNative", "(Ljava/lang/String;IZIJD)V");
  if (!simValueCallbackCallback) return false;

  static hal::UnlimitedHandleResource<SIM_JniHandle, DeviceCallbackStore,
                                      hal::HAL_HandleEnum::SimulationJni>
      cbDevice;
  deviceCallbackHandles = &cbDevice;

  static hal::UnlimitedHandleResource<SIM_JniHandle, ValueCallbackStore,
                                      hal::HAL_HandleEnum::SimulationJni>
      cbValue;
  valueCallbackHandles = &cbValue;

  return true;
}

void FreeSimDeviceDataJNI(JNIEnv* env) {
  simDeviceInfoCls.free(env);
  simValueInfoCls.free(env);
  simDeviceCallbackCls.free(env);
  simValueCallbackCls.free(env);
}

}  // namespace sim
}  // namespace hal

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    setSimDeviceEnabled
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_setSimDeviceEnabled
  (JNIEnv* env, jclass, jstring prefix, jboolean enabled)
{
  HALSIM_SetSimDeviceEnabled(JStringRef{env, prefix}.c_str(), enabled);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    isSimDeviceEnabled
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_isSimDeviceEnabled
  (JNIEnv* env, jclass, jstring name)
{
  return HALSIM_IsSimDeviceEnabled(JStringRef{env, name}.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    registerSimDeviceCreatedCallback
 * Signature: (Ljava/lang/String;Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_registerSimDeviceCreatedCallback
  (JNIEnv* env, jclass, jstring prefix, jobject callback,
   jboolean initialNotify)
{
  return AllocateDeviceCallback(env, JStringRef{env, prefix}.c_str(), callback,
                                initialNotify,
                                &HALSIM_RegisterSimDeviceCreatedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    cancelSimDeviceCreatedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_cancelSimDeviceCreatedCallback
  (JNIEnv* env, jclass, jint uid)
{
  FreeDeviceCallback(env, uid, &HALSIM_CancelSimDeviceCreatedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    registerSimDeviceFreedCallback
 * Signature: (Ljava/lang/String;Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_registerSimDeviceFreedCallback
  (JNIEnv* env, jclass, jstring prefix, jobject callback,
   jboolean initialNotify)
{
  return AllocateDeviceCallback(env, JStringRef{env, prefix}.c_str(), callback,
                                initialNotify,
                                &HALSIM_RegisterSimDeviceFreedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    cancelSimDeviceFreedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_cancelSimDeviceFreedCallback
  (JNIEnv* env, jclass, jint uid)
{
  FreeDeviceCallback(env, uid, &HALSIM_CancelSimDeviceFreedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    getSimDeviceHandle
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_getSimDeviceHandle
  (JNIEnv* env, jclass, jstring name)
{
  return HALSIM_GetSimDeviceHandle(JStringRef{env, name}.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    getSimValueDeviceHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_getSimValueDeviceHandle
  (JNIEnv*, jclass, jint handle)
{
  return HALSIM_GetSimValueDeviceHandle(handle);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    enumerateSimDevices
 * Signature: (Ljava/lang/String;)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_enumerateSimDevices
  (JNIEnv* env, jclass, jstring prefix)
{
  // get values
  std::vector<DeviceInfo> arr;
  HALSIM_EnumerateSimDevices(
      JStringRef{env, prefix}.c_str(), &arr,
      [](const char* name, void* param, HAL_SimDeviceHandle handle) {
        auto arr = static_cast<std::vector<DeviceInfo>*>(param);
        arr->emplace_back(name, handle);
      });

  // convert to java
  size_t numElems = arr.size();
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), simDeviceInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < numElems; ++i) {
    JLocal<jobject> elem{env, arr[i].MakeJava(env)};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    registerSimValueCreatedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_registerSimValueCreatedCallback
  (JNIEnv* env, jclass, jint device, jobject callback, jboolean initialNotify)
{
  return AllocateValueCallback(env, static_cast<HAL_SimDeviceHandle>(device),
                               callback, initialNotify,
                               &HALSIM_RegisterSimValueCreatedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    cancelSimValueCreatedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_cancelSimValueCreatedCallback
  (JNIEnv* env, jclass, jint uid)
{
  FreeValueCallback(env, uid, &HALSIM_CancelSimValueCreatedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    registerSimValueChangedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_registerSimValueChangedCallback
  (JNIEnv* env, jclass, jint handle, jobject callback, jboolean initialNotify)
{
  return AllocateValueCallback(env, static_cast<HAL_SimValueHandle>(handle),
                               callback, initialNotify,
                               &HALSIM_RegisterSimValueChangedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    cancelSimValueChangedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_cancelSimValueChangedCallback
  (JNIEnv* env, jclass, jint uid)
{
  FreeValueCallback(env, uid, &HALSIM_CancelSimValueChangedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    getSimValueHandle
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_getSimValueHandle
  (JNIEnv* env, jclass, jint device, jstring name)
{
  return HALSIM_GetSimValueHandle(device, JStringRef{env, name}.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    enumerateSimValues
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_enumerateSimValues
  (JNIEnv* env, jclass, jint device)
{
  // get values
  std::vector<ValueInfo> arr;
  HALSIM_EnumerateSimValues(
      device, &arr,
      [](const char* name, void* param, HAL_SimValueHandle handle,
         HAL_Bool readonly, const HAL_Value* value) {
        auto arr = static_cast<std::vector<ValueInfo>*>(param);
        arr->emplace_back(name, handle, readonly, *value);
      });

  // convert to java
  size_t numElems = arr.size();
  jobjectArray jarr = env->NewObjectArray(arr.size(), simValueInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < numElems; ++i) {
    JLocal<jobject> elem{env, arr[i].MakeJava(env)};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    getSimValueEnumOptions
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_getSimValueEnumOptions
  (JNIEnv* env, jclass, jint handle)
{
  static JClass stringCls{env, "java/lang/String"};
  if (!stringCls) return nullptr;
  int32_t numElems = 0;
  const char** elems = HALSIM_GetSimValueEnumOptions(handle, &numElems);
  jobjectArray jarr = env->NewObjectArray(numElems, stringCls, nullptr);
  if (!jarr) return nullptr;
  for (int32_t i = 0; i < numElems; ++i) {
    JLocal<jstring> elem{env, MakeJString(env, elems[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    resetSimDeviceData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_resetSimDeviceData
  (JNIEnv*, jclass)
{
  HALSIM_ResetSimDeviceData();
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    getDisplayName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_getDisplayName
  (JNIEnv* env, jclass, jint handle)
{
  const char* displayName =
      HALSIM_GetSimDeviceDisplayName(static_cast<HAL_SimValueHandle>(handle));
  return wpi::java::MakeJString(env, displayName);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    setDisplayName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_setDisplayName
  (JNIEnv* env, jclass, jint handle, jstring displayName)
{
  wpi::java::JStringRef displayNameRef{env, displayName};
  HALSIM_SetSimDeviceDisplayName(static_cast<HAL_SimValueHandle>(handle),
                                 displayNameRef.c_str());
}

}  // extern "C"
