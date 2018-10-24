/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "UsbCameraProperty.h"

#include <wpi/STLExtras.h>
#include <wpi/SmallString.h>

#include "UsbUtil.h"

using namespace cs;

static int GetIntCtrlIoctl(int fd, unsigned id, int type, int64_t* value) {
  unsigned ctrl_class = V4L2_CTRL_ID2CLASS(id);
  if (type == V4L2_CTRL_TYPE_INTEGER64 || V4L2_CTRL_DRIVER_PRIV(id) ||
      (ctrl_class != V4L2_CTRL_CLASS_USER &&
       ctrl_class != V4L2_CID_PRIVATE_BASE)) {
    // Use extended control
    struct v4l2_ext_control ctrl;
    struct v4l2_ext_controls ctrls;
    std::memset(&ctrl, 0, sizeof(ctrl));
    std::memset(&ctrls, 0, sizeof(ctrls));
    ctrl.id = id;
    ctrls.ctrl_class = ctrl_class;
    ctrls.count = 1;
    ctrls.controls = &ctrl;
    int rc = DoIoctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
    if (rc < 0) return rc;
    *value = ctrl.value;
  } else {
    // Use normal control
    struct v4l2_control ctrl;
    std::memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = id;
    int rc = DoIoctl(fd, VIDIOC_G_CTRL, &ctrl);
    if (rc < 0) return rc;
    *value = ctrl.value;
  }
  return 0;
}

static int SetIntCtrlIoctl(int fd, unsigned id, int type, int64_t value) {
  unsigned ctrl_class = V4L2_CTRL_ID2CLASS(id);
  if (type == V4L2_CTRL_TYPE_INTEGER64 || V4L2_CTRL_DRIVER_PRIV(id) ||
      (ctrl_class != V4L2_CTRL_CLASS_USER &&
       ctrl_class != V4L2_CID_PRIVATE_BASE)) {
    // Use extended control
    struct v4l2_ext_control ctrl;
    struct v4l2_ext_controls ctrls;
    std::memset(&ctrl, 0, sizeof(ctrl));
    std::memset(&ctrls, 0, sizeof(ctrls));
    ctrl.id = id;
    if (type == V4L2_CTRL_TYPE_INTEGER64)
      ctrl.value64 = value;
    else
      ctrl.value = static_cast<__s32>(value);
    ctrls.ctrl_class = ctrl_class;
    ctrls.count = 1;
    ctrls.controls = &ctrl;
    return DoIoctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
  } else {
    // Use normal control
    struct v4l2_control ctrl;
    ctrl.id = id;
    ctrl.value = static_cast<__s32>(value);
    return DoIoctl(fd, VIDIOC_S_CTRL, &ctrl);
  }
}

static int GetStringCtrlIoctl(int fd, int id, int maximum, std::string* value) {
  struct v4l2_ext_control ctrl;
  struct v4l2_ext_controls ctrls;
  std::memset(&ctrl, 0, sizeof(ctrl));
  std::memset(&ctrls, 0, sizeof(ctrls));
  ctrl.id = id;
  ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(id);
  ctrls.count = 1;
  ctrls.controls = &ctrl;
  int rc = DoIoctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
  if (rc < 0) {
    value->clear();
    return rc;
  }
  value->assign(ctrl.string, std::strlen(ctrl.string));
  return 0;
}

static int SetStringCtrlIoctl(int fd, int id, int maximum,
                              const wpi::Twine& value) {
  wpi::SmallString<64> strBuf, strBuf2;
  wpi::StringRef str = value.toNullTerminatedStringRef(strBuf);
  if (str.size() > static_cast<size_t>(maximum)) {
    // don't know if strBuf was used, just recopy
    strBuf2 = str.take_front(maximum);
    str = strBuf2;
    strBuf2.push_back('\0');  // null terminate
  }

  struct v4l2_ext_control ctrl;
  struct v4l2_ext_controls ctrls;
  std::memset(&ctrl, 0, sizeof(ctrl));
  std::memset(&ctrls, 0, sizeof(ctrls));
  ctrl.id = id;
  ctrl.size = str.size();
  ctrl.string = const_cast<char*>(str.data());
  ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(id);
  ctrls.count = 1;
  ctrls.controls = &ctrl;
  return DoIoctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
}

// Removes non-alphanumeric characters and replaces spaces with underscores.
// e.g. "Zoom, Absolute" -> "zoom_absolute", "Pan (Absolute)" -> "pan_absolute"
static wpi::StringRef NormalizeName(wpi::StringRef name,
                                    wpi::SmallVectorImpl<char>& buf) {
  bool newWord = false;
  for (auto ch : name) {
    if (std::isalnum(ch)) {
      if (newWord) buf.push_back('_');
      newWord = false;
      buf.push_back(std::tolower(ch));
    } else if (!buf.empty()) {
      newWord = true;
    }
  }
  return wpi::StringRef(buf.data(), buf.size());
}

#ifdef VIDIOC_QUERY_EXT_CTRL
UsbCameraProperty::UsbCameraProperty(const struct v4l2_query_ext_ctrl& ctrl)
    : PropertyImpl(wpi::StringRef{}, CS_PROP_NONE, ctrl.step,
                   ctrl.default_value, 0),
      id(ctrl.id & V4L2_CTRL_ID_MASK),
      type(ctrl.type) {
  hasMinimum = true;
  minimum = ctrl.minimum;
  hasMaximum = true;
  maximum = ctrl.maximum;

  // propKind
  switch (ctrl.type) {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_INTEGER64:
      propKind = CS_PROP_INTEGER;
      break;
    case V4L2_CTRL_TYPE_BOOLEAN:
      propKind = CS_PROP_BOOLEAN;
      break;
    case V4L2_CTRL_TYPE_INTEGER_MENU:
    case V4L2_CTRL_TYPE_MENU:
      propKind = CS_PROP_ENUM;
      break;
    case V4L2_CTRL_TYPE_STRING:
      propKind = CS_PROP_STRING;
      break;
    default:
      return;  // others unsupported
  }

  // name
  size_t len = 0;
  while (len < sizeof(ctrl.name) && ctrl.name[len] != '\0') ++len;
  wpi::SmallString<64> name_buf;
  name = NormalizeName(wpi::StringRef(ctrl.name, len), name_buf);
}
#endif

UsbCameraProperty::UsbCameraProperty(const struct v4l2_queryctrl& ctrl)
    : PropertyImpl(wpi::StringRef{}, CS_PROP_NONE, ctrl.step,
                   ctrl.default_value, 0),
      id(ctrl.id & V4L2_CTRL_ID_MASK),
      type(ctrl.type) {
  hasMinimum = true;
  minimum = ctrl.minimum;
  hasMaximum = true;
  maximum = ctrl.maximum;

  // propKind
  switch (ctrl.type) {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_INTEGER64:
      propKind = CS_PROP_INTEGER;
      break;
    case V4L2_CTRL_TYPE_BOOLEAN:
      propKind = CS_PROP_BOOLEAN;
      break;
    case V4L2_CTRL_TYPE_INTEGER_MENU:
    case V4L2_CTRL_TYPE_MENU:
      propKind = CS_PROP_ENUM;
      break;
    case V4L2_CTRL_TYPE_STRING:
      propKind = CS_PROP_STRING;
      break;
    default:
      return;  // others unsupported
  }

  // name
  size_t len = 0;
  while (len < sizeof(ctrl.name) && ctrl.name[len] != '\0') ++len;
  wpi::SmallString<64> name_buf;
  name = NormalizeName(
      wpi::StringRef(reinterpret_cast<const char*>(ctrl.name), len), name_buf);
}

std::unique_ptr<UsbCameraProperty> UsbCameraProperty::DeviceQuery(int fd,
                                                                  __u32* id) {
  int rc;
  std::unique_ptr<UsbCameraProperty> prop;
#ifdef VIDIOC_QUERY_EXT_CTRL
  v4l2_query_ext_ctrl qc_ext;
  std::memset(&qc_ext, 0, sizeof(qc_ext));
  qc_ext.id = *id;
  rc = TryIoctl(fd, VIDIOC_QUERY_EXT_CTRL, &qc_ext);
  if (rc == 0) {
    *id = qc_ext.id;  // copy back
    // We don't support array types
    if (qc_ext.elems > 1 || qc_ext.nr_of_dims > 0) return nullptr;
    prop = wpi::make_unique<UsbCameraProperty>(qc_ext);
  }
#endif
  if (!prop) {
    // Fall back to normal QUERYCTRL
    struct v4l2_queryctrl qc;
    std::memset(&qc, 0, sizeof(qc));
    qc.id = *id;
    rc = TryIoctl(fd, VIDIOC_QUERYCTRL, &qc);
    *id = qc.id;  // copy back
    if (rc != 0) return nullptr;
    prop = wpi::make_unique<UsbCameraProperty>(qc);
  }

  // Cache enum property choices
  if (prop->propKind == CS_PROP_ENUM) {
    prop->enumChoices.resize(prop->maximum + 1);
    v4l2_querymenu qmenu;
    std::memset(&qmenu, 0, sizeof(qmenu));
    qmenu.id = *id;
    for (int i = prop->minimum; i <= prop->maximum; ++i) {
      qmenu.index = static_cast<__u32>(i);
      if (TryIoctl(fd, VIDIOC_QUERYMENU, &qmenu) != 0) continue;
      prop->enumChoices[i] = reinterpret_cast<const char*>(qmenu.name);
    }
  }

  return prop;
}

bool UsbCameraProperty::DeviceGet(std::unique_lock<wpi::mutex>& lock, int fd) {
  if (fd < 0) return true;
  unsigned idCopy = id;
  int rv = 0;

  switch (propKind) {
    case CS_PROP_BOOLEAN:
    case CS_PROP_INTEGER:
    case CS_PROP_ENUM: {
      int typeCopy = type;
      int64_t newValue = 0;
      lock.unlock();
      rv = GetIntCtrlIoctl(fd, idCopy, typeCopy, &newValue);
      lock.lock();
      if (rv >= 0) value = newValue;
      break;
    }
    case CS_PROP_STRING: {
      int maximumCopy = maximum;
      std::string newValueStr;
      lock.unlock();
      rv = GetStringCtrlIoctl(fd, idCopy, maximumCopy, &newValueStr);
      lock.lock();
      if (rv >= 0) valueStr = std::move(newValueStr);
      break;
    }
    default:
      break;
  }

  return rv >= 0;
}

bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock,
                                  int fd) const {
  // Make a copy of the string as we're about to release the lock
  wpi::SmallString<128> valueStrCopy{valueStr};
  return DeviceSet(lock, fd, value, valueStrCopy);
}

bool UsbCameraProperty::DeviceSet(std::unique_lock<wpi::mutex>& lock, int fd,
                                  int newValue,
                                  const wpi::Twine& newValueStr) const {
  if (!device || fd < 0) return true;
  unsigned idCopy = id;
  int rv = 0;

  switch (propKind) {
    case CS_PROP_BOOLEAN:
    case CS_PROP_INTEGER:
    case CS_PROP_ENUM: {
      int typeCopy = type;
      lock.unlock();
      rv = SetIntCtrlIoctl(fd, idCopy, typeCopy, newValue);
      lock.lock();
      break;
    }
    case CS_PROP_STRING: {
      int maximumCopy = maximum;
      lock.unlock();
      rv = SetStringCtrlIoctl(fd, idCopy, maximumCopy, newValueStr);
      lock.lock();
      break;
    }
    default:
      break;
  }

  return rv >= 0;
}
