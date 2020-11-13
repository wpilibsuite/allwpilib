/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SimDeviceDataJNI.h"

#include <jni.h>

#include <functional>
#include <string>
#include <utility>

#include <wpi/UidVector.h>
#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "edu_wpi_first_hal_simulation_SimDeviceDataJNI.h"
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
  void CallJava(JNIEnv* env, jobject callobj) const;
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
  void CallJava(JNIEnv* env, jobject callobj) const;

 private:
  std::pair<jlong, jdouble> ToValue12() const;
};

}  // namespace

jobject DeviceInfo::MakeJava(JNIEnv* env) const {
  static jmethodID func =
      env->GetMethodID(simDeviceInfoCls, "<init>", "(Ljava/lang/String;I)V");
  return env->NewObject(simDeviceInfoCls, func, MakeJString(env, name),
                        (jint)handle);
}

void DeviceInfo::CallJava(JNIEnv* env, jobject callobj) const {
  env->CallVoidMethod(callobj, simDeviceCallbackCallback,
                      MakeJString(env, name), (jint)handle);
}

std::pair<jlong, jdouble> ValueInfo::ToValue12() const {
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
  auto [value1, value2] = ToValue12();
  return env->NewObject(simValueInfoCls, func, MakeJString(env, name),
                        (jint)handle, (jboolean)readonly, (jint)value.type,
                        value1, value2);
}

void ValueInfo::CallJava(JNIEnv* env, jobject callobj) const {
  auto [value1, value2] = ToValue12();
  env->CallVoidMethod(callobj, simValueCallbackCallback, MakeJString(env, name),
                      (jint)handle, (jboolean)readonly, (jint)value.type,
                      value1, value2);
}

namespace {

class CallbackStore {
 public:
  explicit CallbackStore(JNIEnv* env, jobject obj) : m_call{env, obj} {}
  ~CallbackStore() {
    if (m_cancelCallback) m_cancelCallback();
  }

  void SetCancel(std::function<void()> cancelCallback) {
    m_cancelCallback = std::move(cancelCallback);
  }
  void Free(JNIEnv* env) { m_call.free(env); }
  jobject Get() const { return m_call; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  std::function<void()> m_cancelCallback;
};

class CallbackThreadJNI : public wpi::SafeThread {
 public:
  void Main();

  using DeviceCalls =
      std::vector<std::pair<std::weak_ptr<CallbackStore>, DeviceInfo>>;
  DeviceCalls m_deviceCalls;
  using ValueCalls =
      std::vector<std::pair<std::weak_ptr<CallbackStore>, ValueInfo>>;
  ValueCalls m_valueCalls;

  wpi::UidVector<std::shared_ptr<CallbackStore>, 4> m_callbacks;
};

class CallbackJNI {
 public:
  static CallbackJNI& GetInstance() {
    static CallbackJNI inst;
    return inst;
  }
  void SendDevice(int32_t callback, DeviceInfo info);
  void SendValue(int32_t callback, ValueInfo info);

  std::pair<int32_t, std::shared_ptr<CallbackStore>> AllocateCallback(
      JNIEnv* env, jobject obj);

  void FreeCallback(JNIEnv* env, int32_t uid);

 private:
  CallbackJNI() { m_owner.Start(); }

  wpi::SafeThreadOwner<CallbackThreadJNI> m_owner;
};

}  // namespace

void CallbackThreadJNI::Main() {
  JNIEnv* env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_2;
  args.name = const_cast<char*>("SimDeviceCallback");
  args.group = nullptr;
  jint rs = sim::GetJVM()->AttachCurrentThreadAsDaemon(
      reinterpret_cast<void**>(&env), &args);
  if (rs != JNI_OK) return;

  DeviceCalls deviceCalls;
  ValueCalls valueCalls;

  std::unique_lock lock(m_mutex);
  while (m_active) {
    m_cond.wait(lock);
    if (!m_active) break;

    deviceCalls.swap(m_deviceCalls);
    valueCalls.swap(m_valueCalls);

    lock.unlock();  // don't hold mutex during callback execution

    for (auto&& call : deviceCalls) {
      if (auto store = call.first.lock()) {
        if (jobject callobj = store->Get()) {
          call.second.CallJava(env, callobj);
          if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
          }
        }
      }
    }

    for (auto&& call : valueCalls) {
      if (auto store = call.first.lock()) {
        if (jobject callobj = store->Get()) {
          call.second.CallJava(env, callobj);
          if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
          }
        }
      }
    }

    deviceCalls.clear();
    valueCalls.clear();

    lock.lock();
  }

  // free global references
  for (auto&& callback : m_callbacks) callback->Free(env);

  sim::GetJVM()->DetachCurrentThread();
}

void CallbackJNI::SendDevice(int32_t callback, DeviceInfo info) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_deviceCalls.emplace_back(thr->m_callbacks[callback - 1],
                                  std::move(info));
  thr->m_cond.notify_one();
}

void CallbackJNI::SendValue(int32_t callback, ValueInfo info) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_valueCalls.emplace_back(thr->m_callbacks[callback - 1],
                                 std::move(info));
  thr->m_cond.notify_one();
}

std::pair<int32_t, std::shared_ptr<CallbackStore>>
CallbackJNI::AllocateCallback(JNIEnv* env, jobject obj) {
  auto thr = m_owner.GetThread();
  if (!thr) return std::pair(0, nullptr);
  auto store = std::make_shared<CallbackStore>(env, obj);
  return std::pair(thr->m_callbacks.emplace_back(store) + 1, store);
}

void CallbackJNI::FreeCallback(JNIEnv* env, int32_t uid) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  if (uid <= 0 || static_cast<uint32_t>(uid) > thr->m_callbacks.size()) return;
  uid--;
  auto store = std::move(thr->m_callbacks[uid]);
  thr->m_callbacks.erase(uid);
  store->Free(env);
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
  auto [uid, store] =
      CallbackJNI::GetInstance().AllocateCallback(env, callback);
  int32_t cuid = HALSIM_RegisterSimDeviceCreatedCallback(
      JStringRef{env, prefix}.c_str(),
      reinterpret_cast<void*>(static_cast<intptr_t>(uid)),
      [](const char* name, void* param, HAL_SimDeviceHandle handle) {
        int32_t uid = reinterpret_cast<intptr_t>(param);
        CallbackJNI::GetInstance().SendDevice(uid, DeviceInfo{name, handle});
      },
      initialNotify);
  store->SetCancel([cuid] { HALSIM_CancelSimDeviceCreatedCallback(cuid); });
  return uid;
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
  CallbackJNI::GetInstance().FreeCallback(env, uid);
}

/*
 * Class:     edu_wpi_first_hal_simulation_SimDeviceDataJNI
 * Method:    registerSimDeviceFreedCallback
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_SimDeviceDataJNI_registerSimDeviceFreedCallback
  (JNIEnv* env, jclass, jstring prefix, jobject callback)
{
  auto [uid, store] =
      CallbackJNI::GetInstance().AllocateCallback(env, callback);
  int32_t cuid = HALSIM_RegisterSimDeviceFreedCallback(
      JStringRef{env, prefix}.c_str(),
      reinterpret_cast<void*>(static_cast<intptr_t>(uid)),
      [](const char* name, void* param, HAL_SimDeviceHandle handle) {
        int32_t uid = reinterpret_cast<intptr_t>(param);
        CallbackJNI::GetInstance().SendDevice(uid, DeviceInfo{name, handle});
      });
  store->SetCancel([cuid] { HALSIM_CancelSimDeviceFreedCallback(cuid); });
  return uid;
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
  CallbackJNI::GetInstance().FreeCallback(env, uid);
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
  auto [uid, store] =
      CallbackJNI::GetInstance().AllocateCallback(env, callback);
  int32_t cuid = HALSIM_RegisterSimValueCreatedCallback(
      device, reinterpret_cast<void*>(static_cast<intptr_t>(uid)),
      [](const char* name, void* param, HAL_SimValueHandle handle,
         HAL_Bool readonly, const HAL_Value* value) {
        int32_t uid = reinterpret_cast<intptr_t>(param);
        CallbackJNI::GetInstance().SendValue(
            uid, ValueInfo{name, handle, static_cast<bool>(readonly), *value});
      },
      initialNotify);
  store->SetCancel([cuid] { HALSIM_CancelSimValueCreatedCallback(cuid); });
  return uid;
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
  CallbackJNI::GetInstance().FreeCallback(env, uid);
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
  auto [uid, store] =
      CallbackJNI::GetInstance().AllocateCallback(env, callback);
  int32_t cuid = HALSIM_RegisterSimValueChangedCallback(
      handle, reinterpret_cast<void*>(static_cast<intptr_t>(uid)),
      [](const char* name, void* param, HAL_SimValueHandle handle,
         HAL_Bool readonly, const HAL_Value* value) {
        int32_t uid = reinterpret_cast<intptr_t>(param);
        CallbackJNI::GetInstance().SendValue(
            uid, ValueInfo{name, handle, static_cast<bool>(readonly), *value});
      },
      initialNotify);
  store->SetCancel([cuid] { HALSIM_CancelSimValueChangedCallback(cuid); });
  return uid;
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
  CallbackJNI::GetInstance().FreeCallback(env, uid);
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

}  // extern "C"
