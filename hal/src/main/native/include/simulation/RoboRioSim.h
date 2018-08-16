/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include <memory>
#include <utility>

#include "CallbackStore.h"
#include "mockdata/RoboRioData.h"

namespace frc {
namespace sim {
class RoboRioSim {
 public:
  explicit RoboRioSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterFPGAButtonCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioFPGAButtonCallback);
    store->SetUid(HALSIM_RegisterRoboRioFPGAButtonCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetFPGAButton() const { return HALSIM_GetRoboRioFPGAButton(m_index); }

  void SetFPGAButton(bool fPGAButton) {
    HALSIM_SetRoboRioFPGAButton(m_index, fPGAButton);
  }

  std::unique_ptr<CallbackStore> RegisterVInVoltageCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioVInVoltageCallback);
    store->SetUid(HALSIM_RegisterRoboRioVInVoltageCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetVInVoltage() const { return HALSIM_GetRoboRioVInVoltage(m_index); }

  void SetVInVoltage(double vInVoltage) {
    HALSIM_SetRoboRioVInVoltage(m_index, vInVoltage);
  }

  std::unique_ptr<CallbackStore> RegisterVInCurrentCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioVInCurrentCallback);
    store->SetUid(HALSIM_RegisterRoboRioVInCurrentCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetVInCurrent() const { return HALSIM_GetRoboRioVInCurrent(m_index); }

  void SetVInCurrent(double vInCurrent) {
    HALSIM_SetRoboRioVInCurrent(m_index, vInCurrent);
  }

  std::unique_ptr<CallbackStore> RegisterUserVoltage6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserVoltage6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserVoltage6VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetUserVoltage6V() const {
    return HALSIM_GetRoboRioUserVoltage6V(m_index);
  }

  void SetUserVoltage6V(double userVoltage6V) {
    HALSIM_SetRoboRioUserVoltage6V(m_index, userVoltage6V);
  }

  std::unique_ptr<CallbackStore> RegisterUserCurrent6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserCurrent6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserCurrent6VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetUserCurrent6V() const {
    return HALSIM_GetRoboRioUserCurrent6V(m_index);
  }

  void SetUserCurrent6V(double userCurrent6V) {
    HALSIM_SetRoboRioUserCurrent6V(m_index, userCurrent6V);
  }

  std::unique_ptr<CallbackStore> RegisterUserActive6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserActive6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserActive6VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetUserActive6V() const {
    return HALSIM_GetRoboRioUserActive6V(m_index);
  }

  void SetUserActive6V(bool userActive6V) {
    HALSIM_SetRoboRioUserActive6V(m_index, userActive6V);
  }

  std::unique_ptr<CallbackStore> RegisterUserVoltage5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserVoltage5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserVoltage5VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetUserVoltage5V() const {
    return HALSIM_GetRoboRioUserVoltage5V(m_index);
  }

  void SetUserVoltage5V(double userVoltage5V) {
    HALSIM_SetRoboRioUserVoltage5V(m_index, userVoltage5V);
  }

  std::unique_ptr<CallbackStore> RegisterUserCurrent5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserCurrent5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserCurrent5VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetUserCurrent5V() const {
    return HALSIM_GetRoboRioUserCurrent5V(m_index);
  }

  void SetUserCurrent5V(double userCurrent5V) {
    HALSIM_SetRoboRioUserCurrent5V(m_index, userCurrent5V);
  }

  std::unique_ptr<CallbackStore> RegisterUserActive5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserActive5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserActive5VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetUserActive5V() const {
    return HALSIM_GetRoboRioUserActive5V(m_index);
  }

  void SetUserActive5V(bool userActive5V) {
    HALSIM_SetRoboRioUserActive5V(m_index, userActive5V);
  }

  std::unique_ptr<CallbackStore> RegisterUserVoltage3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserVoltage3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserVoltage3V3Callback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetUserVoltage3V3() const {
    return HALSIM_GetRoboRioUserVoltage3V3(m_index);
  }

  void SetUserVoltage3V3(double userVoltage3V3) {
    HALSIM_SetRoboRioUserVoltage3V3(m_index, userVoltage3V3);
  }

  std::unique_ptr<CallbackStore> RegisterUserCurrent3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserCurrent3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserCurrent3V3Callback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetUserCurrent3V3() const {
    return HALSIM_GetRoboRioUserCurrent3V3(m_index);
  }

  void SetUserCurrent3V3(double userCurrent3V3) {
    HALSIM_SetRoboRioUserCurrent3V3(m_index, userCurrent3V3);
  }

  std::unique_ptr<CallbackStore> RegisterUserActive3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserActive3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserActive3V3Callback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetUserActive3V3() const {
    return HALSIM_GetRoboRioUserActive3V3(m_index);
  }

  void SetUserActive3V3(bool userActive3V3) {
    HALSIM_SetRoboRioUserActive3V3(m_index, userActive3V3);
  }

  std::unique_ptr<CallbackStore> RegisterUserFaults6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserFaults6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserFaults6VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetUserFaults6V() const { return HALSIM_GetRoboRioUserFaults6V(m_index); }

  void SetUserFaults6V(int userFaults6V) {
    HALSIM_SetRoboRioUserFaults6V(m_index, userFaults6V);
  }

  std::unique_ptr<CallbackStore> RegisterUserFaults5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserFaults5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserFaults5VCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetUserFaults5V() const { return HALSIM_GetRoboRioUserFaults5V(m_index); }

  void SetUserFaults5V(int userFaults5V) {
    HALSIM_SetRoboRioUserFaults5V(m_index, userFaults5V);
  }

  std::unique_ptr<CallbackStore> RegisterUserFaults3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRoboRioUserFaults3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserFaults3V3Callback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetUserFaults3V3() const {
    return HALSIM_GetRoboRioUserFaults3V3(m_index);
  }

  void SetUserFaults3V3(int userFaults3V3) {
    HALSIM_SetRoboRioUserFaults3V3(m_index, userFaults3V3);
  }

  void ResetData() { HALSIM_ResetRoboRioData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
#endif  // __FRC_ROBORIO__
