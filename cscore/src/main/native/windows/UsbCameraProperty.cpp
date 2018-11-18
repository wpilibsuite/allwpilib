/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "UsbCameraProperty.h"

using namespace cs;

UsbCameraProperty::UsbCameraProperty(const wpi::Twine& name_,
                                     tagVideoProcAmpProperty tag, bool autoProp,
                                     IAMVideoProcAmp* pProcAmp, bool* isValid)
    : PropertyImpl{autoProp ? name_ + "_auto" : name_} {
  this->tag = tag;
  this->isAutoProp = autoProp;
  long paramVal, paramFlag;  // NOLINT(runtime/int)
  HRESULT hr;
  long minVal, maxVal, stepVal;  // NOLINT(runtime/int)
  hr = pProcAmp->GetRange(tag, &minVal, &maxVal, &stepVal, &paramVal,
                          &paramFlag);  // Unable to get the property, trying to
                                        // return default value
  if (SUCCEEDED(hr)) {
    minimum = minVal;
    maximum = maxVal;
    hasMaximum = true;
    hasMinimum = true;
    defaultValue = paramVal;
    step = stepVal;
    value = paramVal;
    propKind = CS_PropertyKind::CS_PROP_INTEGER;
    *isValid = true;
  } else {
    *isValid = false;
  }
}

bool UsbCameraProperty::DeviceGet(std::unique_lock<wpi::mutex>& lock,
                                  IAMVideoProcAmp* pProcAmp) {
  if (!pProcAmp) return true;

  lock.unlock();
  long newValue = 0, paramFlag = 0;  // NOLINT(runtime/int)
  if (SUCCEEDED(pProcAmp->Get(tag, &newValue, &paramFlag))) {
    lock.lock();
    value = newValue;
    return true;
  }

  return false;
}
bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  IAMVideoProcAmp* pProcAmp) const {
  return DeviceSet(lock, pProcAmp, value);
}
bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  IAMVideoProcAmp* pProcAmp,
                                  int newValue) const {
  if (!pProcAmp) return true;

  lock.unlock();
  if (SUCCEEDED(pProcAmp->Set(tag, newValue, VideoProcAmp_Flags_Manual))) {
    lock.lock();
    return true;
  }

  return false;
}
