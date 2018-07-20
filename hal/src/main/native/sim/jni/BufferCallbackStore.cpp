/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "BufferCallbackStore.h"

#include <jni.h>

#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "hal/Types.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "mockdata/HAL_Value.h"
#include "mockdata/NotifyListener.h"

using namespace wpi::java;
using namespace sim;

static hal::UnlimitedHandleResource<SIM_JniHandle, BufferCallbackStore,
                                    hal::HAL_HandleEnum::SimulationJni>*
    callbackHandles;

namespace sim {
void InitializeBufferStore() {
  static hal::UnlimitedHandleResource<SIM_JniHandle, BufferCallbackStore,
                                      hal::HAL_HandleEnum::SimulationJni>
      cb;
  callbackHandles = &cb;
}
}  // namespace sim

void BufferCallbackStore::create(JNIEnv* env, jobject obj) {
  m_call = JGlobal<jobject>(env, obj);
}

void BufferCallbackStore::performCallback(const char* name, uint8_t* buffer,
                                          uint32_t length) {
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

  auto toCallbackArr =
      MakeJByteArray(env, wpi::StringRef{reinterpret_cast<const char*>(buffer),
                                         static_cast<size_t>(length)});

  env->CallVoidMethod(m_call, sim::GetBufferCallback(), MakeJString(env, name),
                      toCallbackArr, (jint)length);

  jbyte* fromCallbackArr = reinterpret_cast<jbyte*>(
      env->GetPrimitiveArrayCritical(toCallbackArr, nullptr));

  for (size_t i = 0; i < length; i++) {
    buffer[i] = fromCallbackArr[i];
  }

  env->ReleasePrimitiveArrayCritical(toCallbackArr, fromCallbackArr, JNI_ABORT);

  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }

  if (didAttachThread) {
    vm->DetachCurrentThread();
  }
}

void BufferCallbackStore::free(JNIEnv* env) { m_call.free(env); }

SIM_JniHandle sim::AllocateBufferCallback(
    JNIEnv* env, jint index, jobject callback,
    RegisterBufferCallbackFunc createCallback) {
  auto callbackStore = std::make_shared<BufferCallbackStore>();

  auto handle = callbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param, uint8_t* buffer,
                         uint32_t length) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle handle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = callbackHandles->Get(handle);
    if (!data) return;

    data->performCallback(name, buffer, length);
  };

  auto id = createCallback(index, callbackFunc, handleAsVoidPtr);

  callbackStore->setCallbackId(id);

  return handle;
}

void sim::FreeBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                             FreeBufferCallbackFunc freeCallback) {
  auto callback = callbackHandles->Free(handle);
  freeCallback(index, callback->getCallbackId());
  callback->free(env);
}
