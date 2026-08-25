// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/RoboRioSim.hpp"

#include <memory>
#include <string>

#include "wpi/hal/simulation/RoboRioData.h"
#include "wpi/util/string.hpp"

using namespace wpi;
using namespace wpi::sim;

std::unique_ptr<CallbackStore> RoboRioSim::RegisterVInVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioVInVoltageCallback);
  store->SetUid(HALSIM_RegisterRoboRioVInVoltageCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

wpi::units::volts<> RoboRioSim::GetVInVoltage() {
  return wpi::units::volts<>{HALSIM_GetRoboRioVInVoltage()};
}

void RoboRioSim::SetVInVoltage(wpi::units::volts<> vInVoltage) {
  HALSIM_SetRoboRioVInVoltage(vInVoltage.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserVoltage3V3Callback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserVoltage3V3Callback);
  store->SetUid(HALSIM_RegisterRoboRioUserVoltage3V3Callback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

wpi::units::volts<> RoboRioSim::GetUserVoltage3V3() {
  return wpi::units::volts<>{HALSIM_GetRoboRioUserVoltage3V3()};
}

void RoboRioSim::SetUserVoltage3V3(wpi::units::volts<> userVoltage3V3) {
  HALSIM_SetRoboRioUserVoltage3V3(userVoltage3V3.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserCurrent3V3Callback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserCurrent3V3Callback);
  store->SetUid(HALSIM_RegisterRoboRioUserCurrent3V3Callback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

wpi::units::amperes<> RoboRioSim::GetUserCurrent3V3() {
  return wpi::units::amperes<>{HALSIM_GetRoboRioUserCurrent3V3()};
}

void RoboRioSim::SetUserCurrent3V3(wpi::units::amperes<> userCurrent3V3) {
  HALSIM_SetRoboRioUserCurrent3V3(userCurrent3V3.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserActive3V3Callback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserActive3V3Callback);
  store->SetUid(HALSIM_RegisterRoboRioUserActive3V3Callback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RoboRioSim::GetUserActive3V3() {
  return HALSIM_GetRoboRioUserActive3V3();
}

void RoboRioSim::SetUserActive3V3(bool userActive3V3) {
  HALSIM_SetRoboRioUserActive3V3(userActive3V3);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserFaults3V3Callback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserFaults3V3Callback);
  store->SetUid(HALSIM_RegisterRoboRioUserFaults3V3Callback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int RoboRioSim::GetUserFaults3V3() {
  return HALSIM_GetRoboRioUserFaults3V3();
}

void RoboRioSim::SetUserFaults3V3(int userFaults3V3) {
  HALSIM_SetRoboRioUserFaults3V3(userFaults3V3);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterBrownoutVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioBrownoutVoltageCallback);
  store->SetUid(HALSIM_RegisterRoboRioBrownoutVoltageCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

wpi::units::volts<> RoboRioSim::GetBrownoutVoltage() {
  return wpi::units::volts<>{HALSIM_GetRoboRioBrownoutVoltage()};
}

void RoboRioSim::SetBrownoutVoltage(wpi::units::volts<> vInVoltage) {
  HALSIM_SetRoboRioBrownoutVoltage(vInVoltage.value());
}

std::unique_ptr<CallbackStore>
RoboRioSim::RegisterBrownoutRecoveryVoltageCallback(NotifyCallback callback,
                                                    bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioBrownoutRecoveryVoltageCallback);
  store->SetUid(HALSIM_RegisterRoboRioBrownoutRecoveryVoltageCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

wpi::units::volts<> RoboRioSim::GetBrownoutRecoveryVoltage() {
  return wpi::units::volts<>{HALSIM_GetRoboRioBrownoutRecoveryVoltage()};
}

void RoboRioSim::SetBrownoutRecoveryVoltage(
    wpi::units::volts<> brownoutRecoveryVoltage) {
  HALSIM_SetRoboRioBrownoutRecoveryVoltage(brownoutRecoveryVoltage.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterCPUTempCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioCPUTempCallback);
  store->SetUid(HALSIM_RegisterRoboRioCPUTempCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

wpi::units::celsius<> RoboRioSim::GetCPUTemp() {
  return wpi::units::celsius<>{HALSIM_GetRoboRioCPUTemp()};
}

void RoboRioSim::SetCPUTemp(wpi::units::celsius<> cpuTemp) {
  HALSIM_SetRoboRioCPUTemp(cpuTemp.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterTeamNumberCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioTeamNumberCallback);
  store->SetUid(HALSIM_RegisterRoboRioTeamNumberCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int32_t RoboRioSim::GetTeamNumber() {
  return HALSIM_GetRoboRioTeamNumber();
}

void RoboRioSim::SetTeamNumber(int32_t teamNumber) {
  HALSIM_SetRoboRioTeamNumber(teamNumber);
}

std::string RoboRioSim::GetSerialNumber() {
  WPI_String serialNum;
  HALSIM_GetRoboRioSerialNumber(&serialNum);
  std::string serial{wpi::util::to_string_view(&serialNum)};
  WPI_FreeString(&serialNum);
  return serial;
}

void RoboRioSim::SetSerialNumber(std::string_view serialNumber) {
  auto str = wpi::util::make_string(serialNumber);
  HALSIM_SetRoboRioSerialNumber(&str);
}

std::string RoboRioSim::GetComments() {
  WPI_String comments;
  HALSIM_GetRoboRioComments(&comments);
  std::string serial{wpi::util::to_string_view(&comments)};
  WPI_FreeString(&comments);
  return serial;
}

void RoboRioSim::SetComments(std::string_view comments) {
  auto str = wpi::util::make_string(comments);
  HALSIM_SetRoboRioComments(&str);
}

void RoboRioSim::ResetData() {
  HALSIM_ResetRoboRioData();
}
