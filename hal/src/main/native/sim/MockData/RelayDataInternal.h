/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <memory>

#include <support/mutex.h>

#include "MockData/NotifyListenerVector.h"
#include "MockData/RelayData.h"

namespace hal {
class RelayData {
 public:
  int32_t RegisterInitializedForwardCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify);
  void CancelInitializedForwardCallback(int32_t uid);
  void InvokeInitializedForwardCallback(HAL_Value value);
  HAL_Bool GetInitializedForward();
  void SetInitializedForward(HAL_Bool initializedForward);

  int32_t RegisterInitializedReverseCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify);
  void CancelInitializedReverseCallback(int32_t uid);
  void InvokeInitializedReverseCallback(HAL_Value value);
  HAL_Bool GetInitializedReverse();
  void SetInitializedReverse(HAL_Bool initializedReverse);

  int32_t RegisterForwardCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelForwardCallback(int32_t uid);
  void InvokeForwardCallback(HAL_Value value);
  HAL_Bool GetForward();
  void SetForward(HAL_Bool forward);

  int32_t RegisterReverseCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelReverseCallback(int32_t uid);
  void InvokeReverseCallback(HAL_Value value);
  HAL_Bool GetReverse();
  void SetReverse(HAL_Bool reverse);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initializedForward{false};
  std::shared_ptr<NotifyListenerVector> m_initializedForwardCallbacks = nullptr;
  std::atomic<HAL_Bool> m_initializedReverse{false};
  std::shared_ptr<NotifyListenerVector> m_initializedReverseCallbacks = nullptr;
  std::atomic<HAL_Bool> m_forward{false};
  std::shared_ptr<NotifyListenerVector> m_forwardCallbacks = nullptr;
  std::atomic<HAL_Bool> m_reverse{false};
  std::shared_ptr<NotifyListenerVector> m_reverseCallbacks = nullptr;
};
extern RelayData* SimRelayData;
}  // namespace hal
