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
#include "MockData/RoboRioData.h"

namespace hal {
class RoboRioData {
 public:
  int32_t RegisterFPGAButtonCallback(HAL_NotifyCallback callback, void* param,
                                     HAL_Bool initialNotify);
  void CancelFPGAButtonCallback(int32_t uid);
  void InvokeFPGAButtonCallback(HAL_Value value);
  HAL_Bool GetFPGAButton();
  void SetFPGAButton(HAL_Bool fPGAButton);

  int32_t RegisterVInVoltageCallback(HAL_NotifyCallback callback, void* param,
                                     HAL_Bool initialNotify);
  void CancelVInVoltageCallback(int32_t uid);
  void InvokeVInVoltageCallback(HAL_Value value);
  double GetVInVoltage();
  void SetVInVoltage(double vInVoltage);

  int32_t RegisterVInCurrentCallback(HAL_NotifyCallback callback, void* param,
                                     HAL_Bool initialNotify);
  void CancelVInCurrentCallback(int32_t uid);
  void InvokeVInCurrentCallback(HAL_Value value);
  double GetVInCurrent();
  void SetVInCurrent(double vInCurrent);

  int32_t RegisterUserVoltage6VCallback(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelUserVoltage6VCallback(int32_t uid);
  void InvokeUserVoltage6VCallback(HAL_Value value);
  double GetUserVoltage6V();
  void SetUserVoltage6V(double userVoltage6V);

  int32_t RegisterUserCurrent6VCallback(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelUserCurrent6VCallback(int32_t uid);
  void InvokeUserCurrent6VCallback(HAL_Value value);
  double GetUserCurrent6V();
  void SetUserCurrent6V(double userCurrent6V);

  int32_t RegisterUserActive6VCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify);
  void CancelUserActive6VCallback(int32_t uid);
  void InvokeUserActive6VCallback(HAL_Value value);
  HAL_Bool GetUserActive6V();
  void SetUserActive6V(HAL_Bool userActive6V);

  int32_t RegisterUserVoltage5VCallback(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelUserVoltage5VCallback(int32_t uid);
  void InvokeUserVoltage5VCallback(HAL_Value value);
  double GetUserVoltage5V();
  void SetUserVoltage5V(double userVoltage5V);

  int32_t RegisterUserCurrent5VCallback(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelUserCurrent5VCallback(int32_t uid);
  void InvokeUserCurrent5VCallback(HAL_Value value);
  double GetUserCurrent5V();
  void SetUserCurrent5V(double userCurrent5V);

  int32_t RegisterUserActive5VCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify);
  void CancelUserActive5VCallback(int32_t uid);
  void InvokeUserActive5VCallback(HAL_Value value);
  HAL_Bool GetUserActive5V();
  void SetUserActive5V(HAL_Bool userActive5V);

  int32_t RegisterUserVoltage3V3Callback(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
  void CancelUserVoltage3V3Callback(int32_t uid);
  void InvokeUserVoltage3V3Callback(HAL_Value value);
  double GetUserVoltage3V3();
  void SetUserVoltage3V3(double userVoltage3V3);

  int32_t RegisterUserCurrent3V3Callback(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
  void CancelUserCurrent3V3Callback(int32_t uid);
  void InvokeUserCurrent3V3Callback(HAL_Value value);
  double GetUserCurrent3V3();
  void SetUserCurrent3V3(double userCurrent3V3);

  int32_t RegisterUserActive3V3Callback(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelUserActive3V3Callback(int32_t uid);
  void InvokeUserActive3V3Callback(HAL_Value value);
  HAL_Bool GetUserActive3V3();
  void SetUserActive3V3(HAL_Bool userActive3V3);

  int32_t RegisterUserFaults6VCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify);
  void CancelUserFaults6VCallback(int32_t uid);
  void InvokeUserFaults6VCallback(HAL_Value value);
  int32_t GetUserFaults6V();
  void SetUserFaults6V(int32_t userFaults6V);

  int32_t RegisterUserFaults5VCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify);
  void CancelUserFaults5VCallback(int32_t uid);
  void InvokeUserFaults5VCallback(HAL_Value value);
  int32_t GetUserFaults5V();
  void SetUserFaults5V(int32_t userFaults5V);

  int32_t RegisterUserFaults3V3Callback(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
  void CancelUserFaults3V3Callback(int32_t uid);
  void InvokeUserFaults3V3Callback(HAL_Value value);
  int32_t GetUserFaults3V3();
  void SetUserFaults3V3(int32_t userFaults3V3);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_fPGAButton{false};
  std::shared_ptr<NotifyListenerVector> m_fPGAButtonCallbacks = nullptr;
  std::atomic<double> m_vInVoltage{0.0};
  std::shared_ptr<NotifyListenerVector> m_vInVoltageCallbacks = nullptr;
  std::atomic<double> m_vInCurrent{0.0};
  std::shared_ptr<NotifyListenerVector> m_vInCurrentCallbacks = nullptr;
  std::atomic<double> m_userVoltage6V{6.0};
  std::shared_ptr<NotifyListenerVector> m_userVoltage6VCallbacks = nullptr;
  std::atomic<double> m_userCurrent6V{0.0};
  std::shared_ptr<NotifyListenerVector> m_userCurrent6VCallbacks = nullptr;
  std::atomic<HAL_Bool> m_userActive6V{false};
  std::shared_ptr<NotifyListenerVector> m_userActive6VCallbacks = nullptr;
  std::atomic<double> m_userVoltage5V{5.0};
  std::shared_ptr<NotifyListenerVector> m_userVoltage5VCallbacks = nullptr;
  std::atomic<double> m_userCurrent5V{0.0};
  std::shared_ptr<NotifyListenerVector> m_userCurrent5VCallbacks = nullptr;
  std::atomic<HAL_Bool> m_userActive5V{false};
  std::shared_ptr<NotifyListenerVector> m_userActive5VCallbacks = nullptr;
  std::atomic<double> m_userVoltage3V3{3.3};
  std::shared_ptr<NotifyListenerVector> m_userVoltage3V3Callbacks = nullptr;
  std::atomic<double> m_userCurrent3V3{0.0};
  std::shared_ptr<NotifyListenerVector> m_userCurrent3V3Callbacks = nullptr;
  std::atomic<HAL_Bool> m_userActive3V3{false};
  std::shared_ptr<NotifyListenerVector> m_userActive3V3Callbacks = nullptr;
  std::atomic<int32_t> m_userFaults6V{0};
  std::shared_ptr<NotifyListenerVector> m_userFaults6VCallbacks = nullptr;
  std::atomic<int32_t> m_userFaults5V{0};
  std::shared_ptr<NotifyListenerVector> m_userFaults5VCallbacks = nullptr;
  std::atomic<int32_t> m_userFaults3V3{0};
  std::shared_ptr<NotifyListenerVector> m_userFaults3V3Callbacks = nullptr;
};
extern RoboRioData* SimRoboRioData;
}  // namespace hal
