// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "UsbCameraProperty.h"

#include "ComPtr.h"

using namespace cs;

UsbCameraProperty::UsbCameraProperty(const wpi::Twine& name_,
                                     tagVideoProcAmpProperty tag, bool autoProp,
                                     IAMVideoProcAmp* pProcAmp, bool* isValid)
    : PropertyImpl{autoProp ? name_ + "_auto" : name_} {
  this->tagVideoProc = tag;
  this->isControlProperty = false;
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
  if (!pProcAmp)
    return true;

  lock.unlock();
  long newValue = 0, paramFlag = 0;  // NOLINT(runtime/int)
  if (SUCCEEDED(pProcAmp->Get(tagVideoProc, &newValue, &paramFlag))) {
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
  if (!pProcAmp)
    return true;

  lock.unlock();
  if (SUCCEEDED(
          pProcAmp->Set(tagVideoProc, newValue, VideoProcAmp_Flags_Manual))) {
    lock.lock();
    return true;
  }

  return false;
}

UsbCameraProperty::UsbCameraProperty(const wpi::Twine& name_,
                                     tagCameraControlProperty tag,
                                     bool autoProp, IAMCameraControl* pProcAmp,
                                     bool* isValid)
    : PropertyImpl{autoProp ? name_ + "_auto" : name_} {
  this->tagCameraControl = tag;
  this->isControlProperty = true;
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
                                  IAMCameraControl* pProcAmp) {
  if (!pProcAmp)
    return true;

  lock.unlock();
  long newValue = 0, paramFlag = 0;  // NOLINT(runtime/int)
  if (SUCCEEDED(pProcAmp->Get(tagCameraControl, &newValue, &paramFlag))) {
    lock.lock();
    value = newValue;
    return true;
  }

  return false;
}
bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  IAMCameraControl* pProcAmp) const {
  return DeviceSet(lock, pProcAmp, value);
}
bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  IAMCameraControl* pProcAmp,
                                  int newValue) const {
  if (!pProcAmp)
    return true;

  lock.unlock();
  if (SUCCEEDED(pProcAmp->Set(tagCameraControl, newValue,
                              CameraControl_Flags_Manual))) {
    lock.lock();
    return true;
  }

  return false;
}

bool UsbCameraProperty::DeviceGet(std::unique_lock<wpi::mutex>& lock,
                                  IMFSourceReader* sourceReader) {
  if (!sourceReader)
    return true;

  if (isControlProperty) {
    ComPtr<IAMCameraControl> pProcAmp;
    if (SUCCEEDED(sourceReader->GetServiceForStream(
            (DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL,
            IID_PPV_ARGS(pProcAmp.GetAddressOf())))) {
      return DeviceGet(lock, pProcAmp.Get());
    } else {
      return false;
    }
  } else {
    ComPtr<IAMVideoProcAmp> pProcAmp;
    if (SUCCEEDED(sourceReader->GetServiceForStream(
            (DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL,
            IID_PPV_ARGS(pProcAmp.GetAddressOf())))) {
      return DeviceGet(lock, pProcAmp.Get());
    } else {
      return false;
    }
  }
}
bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  IMFSourceReader* sourceReader) const {
  return DeviceSet(lock, sourceReader, value);
}
bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  IMFSourceReader* sourceReader,
                                  int newValue) const {
  if (!sourceReader)
    return true;

  if (isControlProperty) {
    ComPtr<IAMCameraControl> pProcAmp;
    if (SUCCEEDED(sourceReader->GetServiceForStream(
            (DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL,
            IID_PPV_ARGS(pProcAmp.GetAddressOf())))) {
      return DeviceSet(lock, pProcAmp.Get(), newValue);
    } else {
      return false;
    }
  } else {
    ComPtr<IAMVideoProcAmp> pProcAmp;
    if (SUCCEEDED(sourceReader->GetServiceForStream(
            (DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL,
            IID_PPV_ARGS(pProcAmp.GetAddressOf())))) {
      return DeviceSet(lock, pProcAmp.Get(), newValue);
    } else {
      return false;
    }
  }
}
