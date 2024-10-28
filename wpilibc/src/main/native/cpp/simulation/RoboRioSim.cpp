// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/RoboRioSim.h"

#include <memory>
#include <string>

#include <hal/simulation/RoboRioData.h>

using namespace frc;
using namespace frc::sim;

std::unique_ptr<CallbackStore> RoboRioSim::RegisterFPGAButtonCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioFPGAButtonCallback);
  store->SetUid(HALSIM_RegisterRoboRioFPGAButtonCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RoboRioSim::GetFPGAButton() {
  return HALSIM_GetRoboRioFPGAButton();
}

void RoboRioSim::SetFPGAButton(bool fPGAButton) {
  HALSIM_SetRoboRioFPGAButton(fPGAButton);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterVInVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioVInVoltageCallback);
  store->SetUid(HALSIM_RegisterRoboRioVInVoltageCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::volt_t RoboRioSim::GetVInVoltage() {
  return units::volt_t{HALSIM_GetRoboRioVInVoltage()};
}

void RoboRioSim::SetVInVoltage(units::volt_t vInVoltage) {
  HALSIM_SetRoboRioVInVoltage(vInVoltage.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterVInCurrentCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioVInCurrentCallback);
  store->SetUid(HALSIM_RegisterRoboRioVInCurrentCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::ampere_t RoboRioSim::GetVInCurrent() {
  return units::ampere_t{HALSIM_GetRoboRioVInCurrent()};
}

void RoboRioSim::SetVInCurrent(units::ampere_t vInCurrent) {
  HALSIM_SetRoboRioVInCurrent(vInCurrent.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserVoltage6VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserVoltage6VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserVoltage6VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::volt_t RoboRioSim::GetUserVoltage6V() {
  return units::volt_t{HALSIM_GetRoboRioUserVoltage6V()};
}

void RoboRioSim::SetUserVoltage6V(units::volt_t userVoltage6V) {
  HALSIM_SetRoboRioUserVoltage6V(userVoltage6V.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserCurrent6VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserCurrent6VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserCurrent6VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::ampere_t RoboRioSim::GetUserCurrent6V() {
  return units::ampere_t{HALSIM_GetRoboRioUserCurrent6V()};
}

void RoboRioSim::SetUserCurrent6V(units::ampere_t userCurrent6V) {
  HALSIM_SetRoboRioUserCurrent6V(userCurrent6V.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserActive6VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserActive6VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserActive6VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RoboRioSim::GetUserActive6V() {
  return HALSIM_GetRoboRioUserActive6V();
}

void RoboRioSim::SetUserActive6V(bool userActive6V) {
  HALSIM_SetRoboRioUserActive6V(userActive6V);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserVoltage5VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserVoltage5VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserVoltage5VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::volt_t RoboRioSim::GetUserVoltage5V() {
  return units::volt_t{HALSIM_GetRoboRioUserVoltage5V()};
}

void RoboRioSim::SetUserVoltage5V(units::volt_t userVoltage5V) {
  HALSIM_SetRoboRioUserVoltage5V(userVoltage5V.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserCurrent5VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserCurrent5VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserCurrent5VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::ampere_t RoboRioSim::GetUserCurrent5V() {
  return units::ampere_t{HALSIM_GetRoboRioUserCurrent5V()};
}

void RoboRioSim::SetUserCurrent5V(units::ampere_t userCurrent5V) {
  HALSIM_SetRoboRioUserCurrent5V(userCurrent5V.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserActive5VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserActive5VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserActive5VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RoboRioSim::GetUserActive5V() {
  return HALSIM_GetRoboRioUserActive5V();
}

void RoboRioSim::SetUserActive5V(bool userActive5V) {
  HALSIM_SetRoboRioUserActive5V(userActive5V);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserVoltage3V3Callback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserVoltage3V3Callback);
  store->SetUid(HALSIM_RegisterRoboRioUserVoltage3V3Callback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::volt_t RoboRioSim::GetUserVoltage3V3() {
  return units::volt_t{HALSIM_GetRoboRioUserVoltage3V3()};
}

void RoboRioSim::SetUserVoltage3V3(units::volt_t userVoltage3V3) {
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

units::ampere_t RoboRioSim::GetUserCurrent3V3() {
  return units::ampere_t{HALSIM_GetRoboRioUserCurrent3V3()};
}

void RoboRioSim::SetUserCurrent3V3(units::ampere_t userCurrent3V3) {
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

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserFaults6VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserFaults6VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserFaults6VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int RoboRioSim::GetUserFaults6V() {
  return HALSIM_GetRoboRioUserFaults6V();
}

void RoboRioSim::SetUserFaults6V(int userFaults6V) {
  HALSIM_SetRoboRioUserFaults6V(userFaults6V);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterUserFaults5VCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioUserFaults5VCallback);
  store->SetUid(HALSIM_RegisterRoboRioUserFaults5VCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int RoboRioSim::GetUserFaults5V() {
  return HALSIM_GetRoboRioUserFaults5V();
}

void RoboRioSim::SetUserFaults5V(int userFaults5V) {
  HALSIM_SetRoboRioUserFaults5V(userFaults5V);
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

units::volt_t RoboRioSim::GetBrownoutVoltage() {
  return units::volt_t{HALSIM_GetRoboRioBrownoutVoltage()};
}

void RoboRioSim::SetBrownoutVoltage(units::volt_t vInVoltage) {
  HALSIM_SetRoboRioBrownoutVoltage(vInVoltage.value());
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterCPUTempCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioCPUTempCallback);
  store->SetUid(HALSIM_RegisterRoboRioCPUTempCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::celsius_t RoboRioSim::GetCPUTemp() {
  return units::celsius_t{HALSIM_GetRoboRioCPUTemp()};
}

void RoboRioSim::SetCPUTemp(units::celsius_t cpuTemp) {
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
  std::string serial{wpi::to_string_view(&serialNum)};
  WPI_FreeString(&serialNum);
  return serial;
}

void RoboRioSim::SetSerialNumber(std::string_view serialNumber) {
  auto str = wpi::make_string(serialNumber);
  HALSIM_SetRoboRioSerialNumber(&str);
}

std::string RoboRioSim::GetComments() {
  WPI_String comments;
  HALSIM_GetRoboRioComments(&comments);
  std::string serial{wpi::to_string_view(&comments)};
  WPI_FreeString(&comments);
  return serial;
}

void RoboRioSim::SetComments(std::string_view comments) {
  auto str = wpi::make_string(comments);
  HALSIM_SetRoboRioComments(&str);
}

std::unique_ptr<CallbackStore> RoboRioSim::RegisterRadioLEDStateCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelRoboRioRadioLEDStateCallback);
  store->SetUid(HALSIM_RegisterRoboRioRadioLEDStateCallback(
      &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

RadioLEDState RoboRioSim::GetRadioLEDState() {
  return static_cast<RadioLEDState>(HALSIM_GetRoboRioRadioLEDState());
}

void RoboRioSim::SetRadioLEDState(RadioLEDState state) {
  HALSIM_SetRoboRioRadioLEDState(static_cast<HAL_RadioLEDState>(state));
}

void RoboRioSim::ResetData() {
  HALSIM_ResetRoboRioData();
}
