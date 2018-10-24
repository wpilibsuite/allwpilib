/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_USBCAMERAPROPERTY_H_
#define CSCORE_USBCAMERAPROPERTY_H_

#include <linux/videodev2.h>

#include <memory>

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

#ifdef VIDIOC_QUERY_EXT_CTRL
  explicit UsbCameraProperty(const struct v4l2_query_ext_ctrl& ctrl);
#endif
  explicit UsbCameraProperty(const struct v4l2_queryctrl& ctrl);

  static std::unique_ptr<UsbCameraProperty> DeviceQuery(int fd, __u32* id);

  bool DeviceGet(std::unique_lock<wpi::mutex>& lock, int fd);
  bool DeviceSet(std::unique_lock<wpi::mutex>& lock, int fd) const;
  bool DeviceSet(std::unique_lock<wpi::mutex>& lock, int fd, int newValue,
                 const wpi::Twine& newValueStr) const;

  // If this is a device (rather than software) property
  bool device{true};

  // If this is a percentage (rather than raw) property
  bool percentage{false};

  // If not 0, index of corresponding raw/percentage property
  int propPair{0};

  unsigned id{0};  // implementation-level id
  int type{0};     // implementation type, not CS_PropertyKind!
};

}  // namespace cs

#endif  // CSCORE_USBCAMERAPROPERTY_H_
