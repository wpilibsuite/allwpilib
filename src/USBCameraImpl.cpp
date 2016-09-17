/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "USBCameraImpl.h"

#include <algorithm>

#ifdef __linux__
#include <dirent.h>
#include <fcntl.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#elif defined(_WIN32)

#endif

#include "llvm/raw_ostream.h"
#include "llvm/SmallString.h"

#include "cameraserver_cpp.h"
#include "c_util.h"
#include "Handle.h"
#include "Log.h"

using namespace cs;

#ifdef __linux__

static inline int ControlIdToProperty(__u32 control_id) {
  return (control_id & 0xffff) + 1;
}

#ifdef VIDIOC_QUERY_EXT_CTRL
USBCameraImpl::PropertyData::PropertyData(
    const struct v4l2_query_ext_ctrl& ctrl)
    : id(ctrl.id & V4L2_CTRL_ID_MASK),
      type(ctrl.type),
      minimum(ctrl.minimum),
      maximum(ctrl.maximum),
      step(ctrl.step),
      defaultValue(ctrl.default_value) {
  // propType
  switch (ctrl.type) {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_INTEGER64:
      propType = CS_PROP_DOUBLE;
      break;
    case V4L2_CTRL_TYPE_BOOLEAN:
      propType = CS_PROP_BOOLEAN;
      break;
    case V4L2_CTRL_TYPE_INTEGER_MENU:
    case V4L2_CTRL_TYPE_MENU:
      propType = CS_PROP_ENUM;
      break;
    case V4L2_CTRL_TYPE_STRING:
      propType = CS_PROP_STRING;
      break;
    default:
      return;  // others unsupported
  }

  // name
  std::size_t len = 0;
  while (len < sizeof(ctrl.name) && ctrl.name[len] != '\0') ++len;
  name.assign(ctrl.name, len);
}
#endif

USBCameraImpl::PropertyData::PropertyData(const struct v4l2_queryctrl& ctrl)
    : id(ctrl.id & V4L2_CTRL_ID_MASK),
      type(ctrl.type),
      minimum(ctrl.minimum),
      maximum(ctrl.maximum),
      step(ctrl.step),
      defaultValue(ctrl.default_value) {
  // propType
  switch (ctrl.type) {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_INTEGER64:
      propType = CS_PROP_DOUBLE;
      break;
    case V4L2_CTRL_TYPE_BOOLEAN:
      propType = CS_PROP_BOOLEAN;
      break;
    case V4L2_CTRL_TYPE_INTEGER_MENU:
    case V4L2_CTRL_TYPE_MENU:
      propType = CS_PROP_ENUM;
      break;
    case V4L2_CTRL_TYPE_STRING:
      propType = CS_PROP_STRING;
      break;
    default:
      return;  // others unsupported
  }

  // name
  std::size_t len = 0;
  while (len < sizeof(ctrl.name) && ctrl.name[len] != '\0') ++len;
  name.assign(reinterpret_cast<const char*>(ctrl.name), len);
}

static inline int CheckedIoctl(int fd, unsigned long req, void* data,
                               const char* name, bool quiet) {
  int retval = ioctl(fd, req, data);
  if (!quiet && retval < 0)
    ERROR("ioctl " << name << "failed: " << std::strerror(errno));
  return retval;
}

#define DoIoctl(fd, req, data) CheckedIoctl(fd, req, data, #req, false)
#define TryIoctl(fd, req, data) CheckedIoctl(fd, req, data, #req, true)

static int ExtCtrlIoctl(int fd, __u32* id, USBCameraImpl::PropertyData* prop) {
  int rc;
#ifdef VIDIOC_QUERY_EXT_CTRL
  v4l2_query_ext_ctrl qc_ext;
  std::memset(&qc_ext, 0, sizeof(qc_ext));
  qc_ext.id = *id;
  rc = TryIoctl(fd, VIDIOC_QUERY_EXT_CTRL, &qc_ext);
  if (rc == 0) {
    *id = qc_ext.id;  // copy back
    // We don't support array types
    if (qc_ext.elems > 1 || qc_ext.nr_of_dims > 0) return 0;
    *prop = qc_ext;
    return 0;
  }
#endif
  // Fall back to normal QUERYCTRL
  struct v4l2_queryctrl qc;
  std::memset(&qc, 0, sizeof(qc));
  qc.id = *id;
  rc = TryIoctl(fd, VIDIOC_QUERYCTRL, &qc);
  *id = qc.id;  // copy back
  if (rc != 0) return rc;
  *prop = qc;
  return 0;
}

static int GetIntCtrlIoctl(int fd, unsigned id, int type, int64_t* value) {
  unsigned ctrl_class = V4L2_CTRL_ID2CLASS(id);
  if (type == V4L2_CTRL_TYPE_INTEGER64 || V4L2_CTRL_DRIVER_PRIV(id) ||
      (ctrl_class != V4L2_CTRL_CLASS_USER &&
       ctrl_class != V4L2_CID_PRIVATE_BASE)) {
    // Use extended control
    struct v4l2_ext_control ctrl;
    struct v4l2_ext_controls ctrls;
    std::memset(&ctrl, 0, sizeof(ctrl));
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

static std::string GetDescriptionImpl(const char* cpath) {
  llvm::StringRef path{cpath};
  int fd;
  char pathBuf[128];

  // If trying to get by id or path, follow symlink
  if (path.startswith("/dev/v4l/by-id/")) {
    ssize_t n = readlink(cpath, pathBuf, sizeof(pathBuf));
    if (n > 0) path = llvm::StringRef(pathBuf, n);
  } else if (path.startswith("/dev/v4l/by-path/")) {
    ssize_t n = readlink(cpath, pathBuf, sizeof(pathBuf));
    if (n > 0) path = llvm::StringRef(pathBuf, n);
  }

  if (path.startswith("/dev/video")) {
    // Sometimes the /sys tree gives a better name.
    llvm::SmallString<64> ifpath{"/sys/class/video4linux/"};
    ifpath += path.substr(5);
    ifpath += "/device/interface";
    fd = open(ifpath.c_str(), O_RDONLY);
    if (fd >= 0) {
      char readBuf[128];
      ssize_t n = read(fd, readBuf, sizeof(readBuf));
      if (n > 0) {
        close(fd);
        return llvm::StringRef(readBuf, n).rtrim();
      }
      close(fd);
    }
  }

  // Otherwise use an ioctl to query the caps and get the card name
  fd = open(cpath, O_RDWR);
  if (fd >= 0) {
    struct v4l2_capability vcap;
    if (DoIoctl(fd, VIDIOC_QUERYCAP, &vcap) >= 0) {
      close(fd);
      llvm::StringRef card{reinterpret_cast<const char*>(vcap.card)};
      // try to convert "UVC Camera (0000:0000)" into a better name
      int vendor = 0;
      int product = 0;
      if (card.startswith("UVC Camera (") &&
        !card.substr(12, 4).getAsInteger(16, vendor) &&
        !card.substr(17, 4).getAsInteger(16, product)) {
        switch (vendor) {
          case 0x046d:
            switch (product) {
              case 0x081b: return "Logitech, Inc. Webcam C310";
              case 0x0825: return "Logitech, Inc. Webcam C270";
            }
            break;
        }
      }
      return card;
    }
    close(fd);
  }

  return std::string{};
}

USBCameraImpl::USBCameraImpl(llvm::StringRef name, llvm::StringRef path)
    : SourceImpl{name},
      m_path{path},
      m_description{GetDescriptionImpl(m_path.c_str())},
      m_fd{open(m_path.c_str(), O_RDWR)},
      m_active{false} {
  if (m_fd >= 0) m_connected = true;
}

USBCameraImpl::~USBCameraImpl() { Stop(); }

llvm::StringRef USBCameraImpl::GetDescription(
    llvm::SmallVectorImpl<char>& buf) const {
  return m_description;
}

void USBCameraImpl::CacheProperty(PropertyData&& prop) const {
  // NOTE: must be called with m_mutex held!
  int propIndex = ControlIdToProperty(prop.id);
  m_properties[prop.name] = propIndex;
  m_property_data[propIndex] = std::move(prop);
}

void USBCameraImpl::CacheProperties() const {
  int fd = m_fd.load();
  if (fd < 0) return;

  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_properties_cached) return;  // double-checked locking

  constexpr __u32 nextFlags = V4L2_CTRL_FLAG_NEXT_CTRL
#ifdef V4L2_CTRL_FLAG_NEXT_COMPOUND
                          | V4L2_CTRL_FLAG_NEXT_COMPOUND
#endif
      ;
  __u32 id = nextFlags;
  PropertyData prop;

  while (ExtCtrlIoctl(fd, &id, &prop) == 0) {
    CacheProperty(std::move(prop));
    id |= nextFlags;
  }

  if (id == nextFlags) {
    // try just enumerating standard...
    for (id = V4L2_CID_BASE; id < V4L2_CID_LASTP1; ++id) {
      if (ExtCtrlIoctl(fd, &id, &prop) == 0) CacheProperty(std::move(prop));
    }
    // ... and custom controls
    for (id = V4L2_CID_PRIVATE_BASE; ExtCtrlIoctl(fd, &id, &prop) == 0; ++id) {
      CacheProperty(std::move(prop));
    }
  }

  m_properties_cached = true;
}

int USBCameraImpl::GetProperty(llvm::StringRef name) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_properties.lookup(name);
}

llvm::ArrayRef<int> USBCameraImpl::EnumerateProperties(
    llvm::SmallVectorImpl<int>& vec) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  for (const auto& data : m_property_data) vec.push_back(data.getFirst());
  return vec;
}

bool USBCameraImpl::GetPropertyTypeValueFd(int property,
                                           CS_PropertyType propType,
                                           unsigned* id, int* type,
                                           int* fd) const {
  // Get id and type from cached properties
  {
    if (!m_properties_cached) CacheProperties();
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_property_data.find(property);
    if (it == m_property_data.end()) {
      //*status = ;
      return false;
    }
    if (it->getSecond().propType != propType) {
      //*status = ;
      return false;
    }
    *id = it->getSecond().id;
    *type = it->getSecond().type;
  }

  *fd = m_fd.load();
  if (*fd < 0) {
    //*status = ;
    return false;
  }
  return true;
}

CS_PropertyType USBCameraImpl::GetPropertyType(int property) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_property_data.find(property);
  if (it == m_property_data.end()) {
    //*status = ;
    return CS_PROP_NONE;
  }
  return it->getSecond().propType;
}

llvm::StringRef USBCameraImpl::GetPropertyName(
    int property, llvm::SmallVectorImpl<char>& buf) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_property_data.find(property);
  if (it == m_property_data.end()) {
    //*status = ;
    return llvm::StringRef{};
  }
  return it->getSecond().name;  // safe because we never modify it after caching
}

bool USBCameraImpl::GetBooleanProperty(int property) const {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_BOOLEAN, &id, &type, &fd))
    return false;

  int64_t value = 0;
  if (GetIntCtrlIoctl(fd, id, type, &value) < 0) {
    //*status = ;
    return false;
  }
  return value != 0;
}

void USBCameraImpl::SetBooleanProperty(int property, bool value) {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_BOOLEAN, &id, &type, &fd))
    return;

  if (SetIntCtrlIoctl(fd, id, type, value ? 1 : 0) < 0) {
    //*status = ;
  }
}

double USBCameraImpl::GetDoubleProperty(int property) const {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_DOUBLE, &id, &type, &fd))
    return false;

  int64_t value = 0;
  if (GetIntCtrlIoctl(fd, id, type, &value) < 0) {
    //*status = ;
    return false;
  }
  return value;
}

void USBCameraImpl::SetDoubleProperty(int property, double value) {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_DOUBLE, &id, &type, &fd))
    return;

  if (SetIntCtrlIoctl(fd, id, type, static_cast<int64_t>(value)) < 0) {
    //*status = ;
  }
}

double USBCameraImpl::GetPropertyMin(int property) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_property_data.find(property);
  if (it == m_property_data.end()) {
    //*status = ;
    return 0;
  }
  return it->getSecond().minimum;
}

double USBCameraImpl::GetPropertyMax(int property) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_property_data.find(property);
  if (it == m_property_data.end()) {
    //*status = ;
    return 0;
  }
  return it->getSecond().maximum;
}

double USBCameraImpl::GetPropertyStep(int property) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_property_data.find(property);
  if (it == m_property_data.end()) {
    //*status = ;
    return 0;
  }
  return it->getSecond().step;
}

double USBCameraImpl::GetPropertyDefault(int property) const {
  if (!m_properties_cached) CacheProperties();
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_property_data.find(property);
  if (it == m_property_data.end()) {
    //*status = ;
    return 0;
  }
  return it->getSecond().defaultValue;
}

llvm::StringRef USBCameraImpl::GetStringProperty(
    int property, llvm::SmallVectorImpl<char>& buf) const {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_STRING, &id, &type, &fd))
    return llvm::StringRef{};

  struct v4l2_ext_control ctrl;
  struct v4l2_ext_controls ctrls;
  std::memset(&ctrl, 0, sizeof(ctrl));
  ctrl.id = id;
  ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(id);
  ctrls.count = 1;
  ctrls.controls = &ctrl;
  int rc = DoIoctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
  if (rc < 0) {
    //*status = ;
    return llvm::StringRef{};
  }
  buf.append(ctrl.string, ctrl.string + std::strlen(ctrl.string));
  return llvm::StringRef(buf.data(), buf.size());
}

void USBCameraImpl::SetStringProperty(int property, llvm::StringRef value) {
  unsigned id;
  std::size_t maximum;
  // Get id and maximum from cached properties
  {
    if (!m_properties_cached) CacheProperties();
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_property_data.find(property);
    if (it == m_property_data.end()) {
      //*status = ;
      return;
    }
    if (it->getSecond().propType != CS_PROP_STRING) {
      //*status = ;
      return;
    }
    id = it->getSecond().id;
    maximum = static_cast<std::size_t>(it->getSecond().maximum);
  }

  int fd = m_fd.load();
  if (fd < 0) {
    //*status = ;
    return;
  }

  llvm::SmallString<64> str{value.substr(0, std::min(value.size(), maximum))};

  struct v4l2_ext_control ctrl;
  struct v4l2_ext_controls ctrls;
  std::memset(&ctrl, 0, sizeof(ctrl));
  ctrl.id = id;
  ctrl.size = str.size();
  ctrl.string = const_cast<char*>(str.c_str());
  ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(id);
  ctrls.count = 1;
  ctrls.controls = &ctrl;
  int rc = DoIoctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
  if (rc < 0) {
    //*status = ;
    return;
  }
}

int USBCameraImpl::GetEnumProperty(int property) const {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_ENUM, &id, &type, &fd))
    return false;

  int64_t value = 0;
  if (GetIntCtrlIoctl(fd, id, type, &value) < 0) {
    //*status = ;
    return false;
  }
  return static_cast<int>(value);
}

void USBCameraImpl::SetEnumProperty(int property, int value) {
  unsigned id;
  int type;
  int fd;
  if (!GetPropertyTypeValueFd(property, CS_PROP_ENUM, &id, &type, &fd)) return;

  if (SetIntCtrlIoctl(fd, id, type, value) < 0) {
    //*status = ;
  }
}

std::vector<std::string> USBCameraImpl::GetEnumPropertyChoices(
    int property) const {
  unsigned id;
  unsigned minimum;
  unsigned maximum;
  // Get id, minimum, and maximum from cached properties
  {
    if (!m_properties_cached) CacheProperties();
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_property_data.find(property);
    if (it == m_property_data.end()) {
      //*status = ;
      return std::vector<std::string>{};
    }
    if (it->getSecond().propType != CS_PROP_ENUM) {
      //*status = ;
      return std::vector<std::string>{};
    }
    id = it->getSecond().id;
    minimum = static_cast<unsigned>(it->getSecond().minimum);
    maximum = static_cast<unsigned>(it->getSecond().maximum);
  }

  int fd = m_fd.load();
  if (fd < 0) {
    //*status = ;
    return std::vector<std::string>{};
  }

  std::vector<std::string> vec(maximum + 1);
  v4l2_querymenu qmenu;
  std::memset(&qmenu, 0, sizeof(qmenu));
  qmenu.id = id;
  for (unsigned i = minimum; i <= maximum; ++i) {
    qmenu.index = static_cast<__u32>(i);
    if (TryIoctl(fd, VIDIOC_QUERYMENU, &qmenu) != 0) continue;
    vec[i] = reinterpret_cast<const char*>(qmenu.name);
  }

  return vec;
}

void USBCameraImpl::Stop() {
  m_active = false;

  // close camera connection
  int fd = m_fd.exchange(-1);
  if (fd >= 0) close(fd);

  // join camera thread
}

namespace cs {

CS_Source CreateUSBSourceDev(llvm::StringRef name, int dev, CS_Status* status) {
  llvm::SmallString<32> path;
  llvm::raw_svector_ostream oss{path};
  oss << "/dev/video" << dev;
  return CreateUSBSourcePath(name, oss.str(), status);
}

CS_Source CreateUSBSourcePath(llvm::StringRef name, llvm::StringRef path,
                              CS_Status* status) {
  auto source = std::make_shared<USBCameraImpl>(name, path);
  return Sources::GetInstance().Allocate(SourceData::kUSB, source);
}

std::vector<USBCameraInfo> EnumerateUSBCameras(CS_Status* status) {
  std::vector<USBCameraInfo> retval;

  if (DIR* dp = opendir("/dev")) {
    while (struct dirent* ep = readdir(dp)) {
      llvm::StringRef fname{ep->d_name};
      if (!fname.startswith("video")) continue;

      USBCameraInfo info;
      info.dev = -1;
      fname.substr(5).getAsInteger(10, info.dev);
      llvm::SmallString<32> path{"/dev/"};
      path += fname;
      info.path = path.str();

      info.name = GetDescriptionImpl(path.c_str());
      if (info.name.empty()) continue;

      retval.emplace_back(std::move(info));
    }
    closedir(dp);
  } else {
    //*status = ;
    ERROR("Could not open /dev");
    return retval;
  }

  // sort by device number
  std::sort(retval.begin(), retval.end(),
            [](const USBCameraInfo& a, const USBCameraInfo& b) {
              return a.dev < b.dev;
            });

  return retval;
}

}  // namespace cs

extern "C" {

CS_Source CS_CreateUSBSourceDev(const char* name, int dev, CS_Status* status) {
  return cs::CreateUSBSourceDev(name, dev, status);
}

CS_Source CS_CreateUSBSourcePath(const char* name, const char* path,
                                 CS_Status* status) {
  return cs::CreateUSBSourcePath(name, path, status);
}

CS_USBCameraInfo* CS_EnumerateUSBCameras(int* count, CS_Status* status) {
  auto cameras = cs::EnumerateUSBCameras(status);
  CS_USBCameraInfo* out = static_cast<CS_USBCameraInfo*>(
      std::malloc(cameras.size() * sizeof(CS_USBCameraInfo)));
  *count = cameras.size();
  for (std::size_t i = 0; i < cameras.size(); ++i) {
    out[i].dev = cameras[i].dev;
    out[i].path = ConvertToC(cameras[i].path);
    out[i].name = ConvertToC(cameras[i].name);
  }
  return out;
}

void CS_FreeEnumeratedUSBCameras(CS_USBCameraInfo* cameras, int count) {
  if (!cameras) return;
  for (int i = 0; i < count; ++i) {
    std::free(cameras[i].path);
    std::free(cameras[i].name);
  }
  std::free(cameras);
}

}  // extern "C"

#endif  // __linux__
