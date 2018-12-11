/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SpiReadAutoReceiveBufferCallbackStore.h"

#include <jni.h>

#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "hal/Types.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "mockdata/HAL_Value.h"
#include "mockdata/NotifyListener.h"

using namespace wpi::java;
using namespace sim;

static hal::UnlimitedHandleResource<
    SIM_JniHandle, SpiReadAutoReceiveBufferCallbackStore,
    hal::HAL_HandleEnum::SimulationJni>* callbackHandles;

namespace sim {
void InitializeSpiBufferStore() {
  static hal::UnlimitedHandleResource<SIM_JniHandle,
                                      SpiReadAutoReceiveBufferCallbackStore,
                                      hal::HAL_HandleEnum::SimulationJni>
      cb;
  callbackHandles = &cb;
}
}  // namespace sim

void SpiReadAutoReceiveBufferCallbackStore::create(JNIEnv* env, jobject obj) {
  m_call = JGlobal<jobject>(env, obj);
}

int32_t SpiReadAutoReceiveBufferCallbackStore::performCallback(
    const char* name, uint32_t* buffer, int32_t numToRead) {
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
      return -1;
    }
  } else if (tryGetEnv == JNI_EVERSION) {
    wpi::outs() << "Invalid JVM Version requested\n";
    wpi::outs().flush();
  }

  auto toCallbackArr = MakeJIntArray(
      env, wpi::ArrayRef<uint32_t>{buffer, static_cast<size_t>(numToRead)});

  jint ret = env->CallIntMethod(m_call, sim::GetBufferCallback(),
                                MakeJString(env, name), toCallbackArr,
                                (jint)numToRead);

  jint* fromCallbackArr = reinterpret_cast<jint*>(
      env->GetPrimitiveArrayCritical(toCallbackArr, nullptr));

  for (int i = 0; i < ret; i++) {
    buffer[i] = fromCallbackArr[i];
  }

  env->ReleasePrimitiveArrayCritical(toCallbackArr, fromCallbackArr, JNI_ABORT);

  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }

  if (didAttachThread) {
    vm->DetachCurrentThread();
  }
  return ret;
}

void SpiReadAutoReceiveBufferCallbackStore::free(JNIEnv* env) {
  m_call.free(env);
}

SIM_JniHandle sim::AllocateSpiBufferCallback(
    JNIEnv* env, jint index, jobject callback,
    RegisterSpiBufferCallbackFunc createCallback) {
  auto callbackStore =
      std::make_shared<SpiReadAutoReceiveBufferCallbackStore>();

  auto handle = callbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param, uint32_t* buffer,
                         int32_t numToRead, int32_t* outputCount) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle handle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = callbackHandles->Get(handle);
    if (!data) return;

    *outputCount = data->performCallback(name, buffer, numToRead);
  };

  auto id = createCallback(index, callbackFunc, handleAsVoidPtr);

  callbackStore->setCallbackId(id);

  return handle;
}

void sim::FreeSpiBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                                FreeSpiBufferCallbackFunc freeCallback) {
  auto callback = callbackHandles->Free(handle);
  freeCallback(index, callback->getCallbackId());
  callback->free(env);
}
