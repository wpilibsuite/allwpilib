/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <Dshow.h>
#include <wpi/mutex.h>

#include "PropertyImpl.h"

namespace cs {

// Property data
class UsbCameraProperty : public PropertyImpl {
 public:
  UsbCameraProperty() = default;
  explicit UsbCameraProperty(const wpi::Twine& name_) : PropertyImpl{name_} {}

  // Software property constructor
  UsbCameraProperty(const wpi::Twine& name_, unsigned id_,
                    CS_PropertyKind kind_, int minimum_, int maximum_,
                    int step_, int defaultValue_, int value_)
      : PropertyImpl(name_, kind_, minimum_, maximum_, step_, defaultValue_,
                     value_),
        device{false},
        id{id_} {}

  // Normalized property constructor
  UsbCameraProperty(const wpi::Twine& name_, int rawIndex_,
                    const UsbCameraProperty& rawProp, int defaultValue_,
                    int value_)
      : PropertyImpl(name_, rawProp.propKind, 1, defaultValue_, value_),
        percentage{true},
        propPair{rawIndex_},
        id{rawProp.id},
        type{rawProp.type} {
    hasMinimum = true;
    minimum = 0;
    hasMaximum = true;
    maximum = 100;
  }

  UsbCameraProperty(const wpi::Twine& name_, tagVideoProcAmpProperty tag,
                    bool autoProp, IAMVideoProcAmp* pProcAmp, bool* isValid);

  bool DeviceGet(std::unique_lock<wpi::mutex>& lock, IAMVideoProcAmp* pProcAmp);
  bool DeviceSet(std::unique_lock<wpi::mutex>& lock,
                 IAMVideoProcAmp* pProcAmp) const;
  bool DeviceSet(std::unique_lock<wpi::mutex>& lock, IAMVideoProcAmp* pProcAmp,
                 int newValue) const;

  // If this is a device (rather than software) property
  bool device{true};
  bool isAutoProp{true};
  tagVideoProcAmpProperty tag;

  // If this is a percentage (rather than raw) property
  bool percentage{false};

  // If not 0, index of corresponding raw/percentage property
  int propPair{0};

  unsigned id{0};  // implementation-level id
  int type{0};     // implementation type, not CS_PropertyKind!
};
}  // namespace cs
