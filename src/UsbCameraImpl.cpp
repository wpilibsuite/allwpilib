/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "UsbCameraImpl.h"

#include <algorithm>

#ifdef __linux__
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#elif defined(_WIN32)

#endif

#include "llvm/raw_ostream.h"
#include "llvm/SmallString.h"
#include "support/timestamp.h"

#include "cscore_cpp.h"
#include "c_util.h"
#include "Handle.h"
#include "Log.h"
#include "Notifier.h"
#include "UsbUtil.h"

using namespace cs;

#ifdef __linux__

// Conversions v4l2_fract time per frame from/to frames per second (fps)
static inline int FractToFPS(const struct v4l2_fract& timeperframe) {
  return (1.0 * timeperframe.denominator) / timeperframe.numerator;
}

static inline struct v4l2_fract FPSToFract(int fps) {
  struct v4l2_fract timeperframe;
  timeperframe.numerator = 1;
  timeperframe.denominator = fps;
  return timeperframe;
}

// Conversion from v4l2_format pixelformat to VideoMode::PixelFormat
static VideoMode::PixelFormat ToPixelFormat(__u32 pixelFormat) {
  switch (pixelFormat) {
    case V4L2_PIX_FMT_MJPEG:
      return VideoMode::kMJPEG;
    case V4L2_PIX_FMT_YUYV:
      return VideoMode::kYUYV;
    case V4L2_PIX_FMT_RGB565:
      return VideoMode::kRGB565;
    case V4L2_PIX_FMT_BGR24:
      return VideoMode::kBGR;
    default:
      return VideoMode::kUnknown;
  }
}

// Conversion from VideoMode::PixelFormat to v4l2_format pixelformat
static __u32 FromPixelFormat(VideoMode::PixelFormat pixelFormat) {
  switch (pixelFormat) {
    case VideoMode::kMJPEG:
      return V4L2_PIX_FMT_MJPEG;
    case VideoMode::kYUYV:
      return V4L2_PIX_FMT_YUYV;
    case VideoMode::kRGB565:
      return V4L2_PIX_FMT_RGB565;
    case VideoMode::kBGR:
      return V4L2_PIX_FMT_BGR24;
    default:
      return 0;
  }
}

// Removes non-alphanumeric characters and replaces spaces with underscores.
// e.g. "Zoom, Absolute" -> "zoom_absolute", "Pan (Absolute)" -> "pan_absolute"
static llvm::StringRef NormalizeName(llvm::StringRef name,
                                     llvm::SmallVectorImpl<char>& buf) {
  bool newWord = false;
  for (auto ch : name) {
    if (std::isalnum(ch)) {
      if (newWord) buf.push_back('_');
      newWord = false;
      buf.push_back(std::tolower(ch));
    } else if (!buf.empty())
      newWord = true;
  }
  return llvm::StringRef(buf.data(), buf.size());
}

static bool IsPercentageProperty(llvm::StringRef name) {
  if (name.startswith("raw_")) name = name.substr(4);
  return name == "brightness" || name == "contrast" || name == "saturation" ||
         name == "hue" || name == "sharpness" || name == "gain" ||
         name == "exposure_absolute";
}

int UsbCameraImpl::RawToPercentage(const PropertyData& rawProp, int rawValue) {
  return 100.0 * (rawValue - rawProp.minimum) /
         (rawProp.maximum - rawProp.minimum);
}

int UsbCameraImpl::PercentageToRaw(const PropertyData& rawProp,
                                   int percentValue) {
  return rawProp.minimum +
         (rawProp.maximum - rawProp.minimum) * (percentValue / 100.0);
}

void UsbCameraImpl::UpdatePropertyValue(int property, bool setString, int value,
                                        llvm::StringRef valueStr) {
  auto prop = static_cast<PropertyData*>(GetProperty(property));
  if (!prop) return;

  if (setString)
    prop->SetValue(valueStr);
  else
    prop->SetValue(value);

  // Only notify updates after we've notified created
  if (m_properties_cached)
    Notifier::GetInstance().NotifySourceProperty(
        *this, CS_SOURCE_PROPERTY_VALUE_UPDATED, property, prop->propKind,
        prop->value, prop->valueStr);
}

#ifdef VIDIOC_QUERY_EXT_CTRL
UsbCameraImpl::PropertyData::PropertyData(
    const struct v4l2_query_ext_ctrl& ctrl)
    : PropertyBase(llvm::StringRef{}, CS_PROP_NONE, ctrl.step,
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
  std::size_t len = 0;
  while (len < sizeof(ctrl.name) && ctrl.name[len] != '\0') ++len;
  llvm::SmallString<64> name_buf;
  name = NormalizeName(llvm::StringRef(ctrl.name, len), name_buf);
}
#endif

UsbCameraImpl::PropertyData::PropertyData(const struct v4l2_queryctrl& ctrl)
    : PropertyBase(llvm::StringRef{}, CS_PROP_NONE, ctrl.step,
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
  std::size_t len = 0;
  while (len < sizeof(ctrl.name) && ctrl.name[len] != '\0') ++len;
  llvm::SmallString<64> name_buf;
  name = NormalizeName(
      llvm::StringRef(reinterpret_cast<const char*>(ctrl.name), len), name_buf);
}

static inline int CheckedIoctl(int fd, unsigned long req, void* data,
                               const char* name, const char* file, int line,
                               bool quiet) {
  int retval = ioctl(fd, req, data);
  if (!quiet && retval < 0) {
    llvm::SmallString<64> localfile{file};
    localfile.push_back('\0');
    ERROR("ioctl " << name << " failed at " << basename(localfile.data()) << ":"
                   << line << ": " << std::strerror(errno));
  }
  return retval;
}

#define DoIoctl(fd, req, data) \
  CheckedIoctl(fd, req, data, #req, __FILE__, __LINE__, false)
#define TryIoctl(fd, req, data) \
  CheckedIoctl(fd, req, data, #req, __FILE__, __LINE__, true)

static std::unique_ptr<UsbCameraImpl::PropertyData> ExtCtrlIoctl(int fd,
                                                                 __u32* id) {
  int rc;
  std::unique_ptr<UsbCameraImpl::PropertyData> prop;
#ifdef VIDIOC_QUERY_EXT_CTRL
  v4l2_query_ext_ctrl qc_ext;
  std::memset(&qc_ext, 0, sizeof(qc_ext));
  qc_ext.id = *id;
  rc = TryIoctl(fd, VIDIOC_QUERY_EXT_CTRL, &qc_ext);
  if (rc == 0) {
    *id = qc_ext.id;  // copy back
    // We don't support array types
    if (qc_ext.elems > 1 || qc_ext.nr_of_dims > 0) return nullptr;
    prop = llvm::make_unique<UsbCameraImpl::PropertyData>(qc_ext);
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
    prop = llvm::make_unique<UsbCameraImpl::PropertyData>(qc);
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

static int GetStringCtrlIoctl(int fd, int id, int maximum,
                              std::string* value) {
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
                              llvm::StringRef value) {
  llvm::SmallString<64> str{value.substr(
      0, std::min(value.size(), static_cast<std::size_t>(maximum)))};

  struct v4l2_ext_control ctrl;
  struct v4l2_ext_controls ctrls;
  std::memset(&ctrl, 0, sizeof(ctrl));
  std::memset(&ctrls, 0, sizeof(ctrls));
  ctrl.id = id;
  ctrl.size = str.size();
  ctrl.string = const_cast<char*>(str.c_str());
  ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(id);
  ctrls.count = 1;
  ctrls.controls = &ctrl;
  return DoIoctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
}

static bool GetDescriptionSysV4L(llvm::StringRef path, std::string* desc) {
  llvm::SmallString<64> ifpath{"/sys/class/video4linux/"};
  ifpath += path.substr(5);
  ifpath += "/device/interface";

  int fd = open(ifpath.c_str(), O_RDONLY);
  if (fd < 0) return false;

  char readBuf[128];
  ssize_t n = read(fd, readBuf, sizeof(readBuf));
  close(fd);

  if (n <= 0) return false;

  *desc = llvm::StringRef(readBuf, n).rtrim();
  return true;
}

static bool GetDescriptionIoctl(const char* cpath, std::string* desc) {
  int fd = open(cpath, O_RDWR);
  if (fd < 0) return false;

  struct v4l2_capability vcap;
  std::memset(&vcap, 0, sizeof(vcap));
  if (DoIoctl(fd, VIDIOC_QUERYCAP, &vcap) < 0) {
    close(fd);
    return false;
  }
  close(fd);

  llvm::StringRef card{reinterpret_cast<const char*>(vcap.card)};
  // try to convert "UVC Camera (0000:0000)" into a better name
  int vendor = 0;
  int product = 0;
  if (card.startswith("UVC Camera (") &&
      !card.substr(12, 4).getAsInteger(16, vendor) &&
      !card.substr(17, 4).getAsInteger(16, product)) {
    llvm::SmallString<64> card2Buf;
    llvm::StringRef card2 = GetUsbNameFromId(vendor, product, card2Buf);
    if (!card2.empty()) {
      *desc = card2;
      return true;
    }
  }

  *desc = card;
  return true;
}

static std::string GetDescriptionImpl(const char* cpath) {
  llvm::StringRef path{cpath};
  char pathBuf[128];
  std::string rv;

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
    if (GetDescriptionSysV4L(path, &rv)) return rv;
  }

  // Otherwise use an ioctl to query the caps and get the card name
  if (GetDescriptionIoctl(cpath, &rv)) return rv;

  return std::string{};
}

UsbCameraImpl::UsbCameraImpl(llvm::StringRef name, llvm::StringRef path)
    : SourceImpl{name},
      m_path{path},
      m_fd{-1},
      m_command_fd{eventfd(0, 0)},
      m_active{true} {
  SetDescription(GetDescriptionImpl(m_path.c_str()));
}

UsbCameraImpl::~UsbCameraImpl() {
  m_active = false;

  // Just in case anyone is waiting...
  m_responseCv.notify_all();

  // Send message to wake up thread; select timeout will wake us up anyway,
  // but this speeds shutdown.
  Send(CreateMessage(Message::kNone));

  // join camera thread
  if (m_cameraThread.joinable()) m_cameraThread.join();

  // close command fd
  int fd = m_command_fd.exchange(-1);
  if (fd >= 0) close(fd);
}

static inline void DoFdSet(int fd, fd_set* set, int* nfds) {
  if (fd >= 0) {
    FD_SET(fd, set);
    if ((fd + 1) > *nfds) *nfds = fd + 1;
  }
}

void UsbCameraImpl::Start() {
  // Kick off the camera thread
  m_cameraThread = std::thread(&UsbCameraImpl::CameraThreadMain, this);
}

void UsbCameraImpl::CameraThreadMain() {
  // We want to be notified on file creation and deletion events in the device
  // path.  This is used to detect disconnects and reconnects.
  std::unique_ptr<wpi::raw_fd_istream> notify_is;
  int notify_fd = inotify_init();
  if (notify_fd >= 0) {
    // need to make a copy as dirname can modify it
    llvm::SmallString<64> pathCopy{m_path};
    pathCopy.push_back('\0');
    if (inotify_add_watch(notify_fd, dirname(pathCopy.data()),
                          IN_CREATE | IN_DELETE) < 0) {
      close(notify_fd);
      notify_fd = -1;
    } else {
      notify_is.reset(new wpi::raw_fd_istream{
          notify_fd, true, sizeof(struct inotify_event) + NAME_MAX + 1});
    }
  }
  bool notified = (notify_fd < 0);  // treat as always notified if cannot notify

  // Get the basename for later notify use
  llvm::SmallString<64> pathCopy{m_path};
  pathCopy.push_back('\0');
  llvm::SmallString<64> base{basename(pathCopy.data())};

  // Used to restart streaming on reconnect
  bool wasStreaming = false;

  // Default to not streaming
  m_streaming = false;

  while (m_active) {
    // If not connected, try to reconnect
    if (m_fd < 0) DeviceConnect();

    // Make copies of fd's in case they go away
    int command_fd = m_command_fd.load();
    int fd = m_fd.load();
    if (!m_active) break;

    // Reset notified flag and restart streaming if necessary
    if (fd >= 0) {
      notified = (notify_fd < 0);
      if (wasStreaming && !m_streaming) {
        DeviceStreamOn();
        wasStreaming = false;
      }
    }

    // Turn off streaming if no one is listening, and turn it on if there is.
    if (m_streaming && m_numSinksEnabled == 0) {
      DeviceStreamOff();
    } else if (!m_streaming && m_numSinksEnabled > 0) {
      DeviceStreamOn();
    }

    // The select timeout can be long unless we're trying to reconnect
    struct timeval tv;
    if (fd < 0 && notified) {
      tv.tv_sec = 0;
      tv.tv_usec = 300000;
    } else {
      tv.tv_sec = 2;
      tv.tv_usec = 0;
    }

    // select on applicable read descriptors
    int nfds = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    DoFdSet(command_fd, &readfds, &nfds);
    if (m_streaming) DoFdSet(fd, &readfds, &nfds);
    DoFdSet(notify_fd, &readfds, &nfds);

    if (select(nfds, &readfds, nullptr, nullptr, &tv) < 0) {
      SERROR("select(): " << strerror(errno));
      break;  // XXX: is this the right thing to do here?
    }

    // Double-check to see if we're shutting down
    if (!m_active) break;

    // Handle notify events
    if (notify_fd >= 0 && FD_ISSET(notify_fd, &readfds)) {
      SDEBUG4("notify event");
      struct inotify_event event;
      do {
        // Read the event structure
        notify_is->read(&event, sizeof(event));
        // Read the event name
        llvm::SmallString<64> raw_name;
        raw_name.resize(event.len);
        notify_is->read(raw_name.data(), event.len);
        // If the name is what we expect...
        llvm::StringRef name{raw_name.c_str()};
        SDEBUG4("got event on '" << name << "' (" << name.size()
                                 << ") compare to '" << base << "' ("
                                 << base.size() << ") mask " << event.mask);
        if (name == base) {
          if ((event.mask & IN_DELETE) != 0) {
            wasStreaming = m_streaming;
            DeviceStreamOff();
            DeviceDisconnect();
          } else if ((event.mask & IN_CREATE) != 0) {
            notified = true;
          }
        }
      } while (!notify_is->has_error() &&
               notify_is->in_avail() >= sizeof(event));
      continue;
    }

    // Handle commands
    if (command_fd >= 0 && FD_ISSET(command_fd, &readfds)) {
      SDEBUG4("got command");
      // Read it to clear
      eventfd_t val;
      eventfd_read(command_fd, &val);
      DeviceProcessCommands();
      continue;
    }

    // Handle frames
    if (m_streaming && fd >= 0 && FD_ISSET(fd, &readfds)) {
      SDEBUG4("grabbing image");

      // Dequeue buffer
      struct v4l2_buffer buf;
      std::memset(&buf, 0, sizeof(buf));
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      if (DoIoctl(fd, VIDIOC_DQBUF, &buf) != 0) {
        SWARNING("could not dequeue buffer");
        wasStreaming = m_streaming;
        DeviceStreamOff();
        DeviceDisconnect();
        notified = true;  // device wasn't deleted, just error'ed
        continue;  // will reconnect
      }

      if ((buf.flags & V4L2_BUF_FLAG_ERROR) == 0) {
        SDEBUG4("got image size=" << buf.bytesused << " index=" << buf.index);

        if (buf.index >= kNumBuffers || !m_buffers[buf.index].m_data) {
          SWARNING("invalid buffer" << buf.index);
          continue;
        }

        PutFrame(static_cast<VideoMode::PixelFormat>(m_mode.pixelFormat),
                 m_mode.width, m_mode.height,
                 llvm::StringRef(
                     static_cast<const char*>(m_buffers[buf.index].m_data),
                     static_cast<std::size_t>(buf.bytesused)),
                 wpi::Now());  // TODO: time
      }

      // Requeue buffer
      if (DoIoctl(fd, VIDIOC_QBUF, &buf) != 0) {
        SWARNING("could not requeue buffer");
        wasStreaming = m_streaming;
        DeviceStreamOff();
        DeviceDisconnect();
        notified = true;  // device wasn't deleted, just error'ed
        continue;  // will reconnect
      }
    }
  }

  // close camera connection
  DeviceStreamOff();
  DeviceDisconnect();
}

void UsbCameraImpl::DeviceDisconnect() {
  int fd = m_fd.exchange(-1);
  if (fd < 0) return;  // already disconnected

  // Unmap buffers
  for (int i = 0; i < kNumBuffers; ++i)
    m_buffers[i] = std::move(UsbCameraBuffer{});

  // Close device
  close(fd);

  // Notify
  SetConnected(false);
}

void UsbCameraImpl::DeviceConnect() {
  if (m_fd >= 0) return;

  SINFO("Connecting to USB camera on " << m_path);

  // Try to open the device
  SDEBUG3("opening device");
  int fd = open(m_path.c_str(), O_RDWR);
  if (fd < 0) return;
  m_fd = fd;

  // Get capabilities
  SDEBUG3("getting capabilities");
  struct v4l2_capability vcap;
  std::memset(&vcap, 0, sizeof(vcap));
  if (DoIoctl(fd, VIDIOC_QUERYCAP, &vcap) >= 0) {
    m_capabilities = vcap.capabilities;
    if (m_capabilities & V4L2_CAP_DEVICE_CAPS)
      m_capabilities = vcap.device_caps;
  }

  // Get or restore video mode
  if (!m_properties_cached) {
    SDEBUG3("caching properties");
    DeviceCacheProperties();
    DeviceCacheVideoModes();
    DeviceCacheMode();
    m_properties_cached = true;
  } else {
    SDEBUG3("restoring video mode");
    DeviceSetMode();
    DeviceSetFPS();

    // Restore settings
    SDEBUG3("restoring settings");
    std::unique_lock<std::mutex> lock2(m_mutex);
    for (std::size_t i = 0; i < m_propertyData.size(); ++i) {
      const auto prop =
          static_cast<const PropertyData*>(m_propertyData[i].get());
      if (!prop || !prop->valueSet || prop->percentage) continue;
      if (!DeviceSetProperty(lock2, *prop))
        SWARNING("failed to set property " << prop->name);
    }
  }

  // Request buffers
  SDEBUG3("allocating buffers");
  struct v4l2_requestbuffers rb;
  std::memset(&rb, 0, sizeof(rb));
  rb.count = kNumBuffers;
  rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  rb.memory = V4L2_MEMORY_MMAP;
  if (DoIoctl(fd, VIDIOC_REQBUFS, &rb) != 0) {
    SWARNING("could not allocate buffers");
    close(fd);
    m_fd = -1;
    return;
  }

  // Map buffers
  SDEBUG3("mapping buffers");
  for (int i = 0; i < kNumBuffers; ++i) {
    struct v4l2_buffer buf;
    std::memset(&buf, 0, sizeof(buf));
    buf.index = i;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (DoIoctl(fd, VIDIOC_QUERYBUF, &buf) != 0) {
      SWARNING("could not query buffer " << i);
      close(fd);
      m_fd = -1;
      return;
    }
    SDEBUG4("buf " << i << " length=" << buf.length
                   << " offset=" << buf.m.offset);

    m_buffers[i] = std::move(UsbCameraBuffer(fd, buf.length, buf.m.offset));
    if (!m_buffers[i].m_data) {
      SWARNING("could not map buffer " << i);
      // release other buffers
      for (int j = 0; j < i; ++j) m_buffers[j] = std::move(UsbCameraBuffer{});
      close(fd);
      m_fd = -1;
      return;
    }

    SDEBUG4("buf " << i << " address=" << m_buffers[i].m_data);
  }

  // Update description (as it may have changed)
  SetDescription(GetDescriptionImpl(m_path.c_str()));

  // Notify
  SetConnected(true);
}

bool UsbCameraImpl::DeviceStreamOn() {
  if (m_streaming) return false;  // ignore if already enabled
  int fd = m_fd.load();
  if (fd < 0) return false;

  // Queue buffers
  SDEBUG3("queuing buffers");
  for (int i = 0; i < kNumBuffers; ++i) {
    struct v4l2_buffer buf;
    std::memset(&buf, 0, sizeof(buf));
    buf.index = i;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (DoIoctl(fd, VIDIOC_QBUF, &buf) != 0) {
      SWARNING("could not queue buffer " << i);
      return false;
    }
  }

  // Turn stream on
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (TryIoctl(fd, VIDIOC_STREAMON, &type) < 0) {
    if (errno == ENOSPC) {
      // indicates too much USB bandwidth requested
      SERROR(
          "could not start streaming due to USB bandwidth limitations; try a "
          "lower resolution or a different pixel format (VIDIOC_STREAMON: "
          "No space left on device)");
    } else {
      // some other error
      SERROR("ioctl VIDIOC_STREAMON failed: " << std::strerror(errno));
    }
    return false;
  }
  SDEBUG4("enabled streaming");
  m_streaming = true;
  return true;
}

bool UsbCameraImpl::DeviceStreamOff() {
  if (!m_streaming) return false;  // ignore if already disabled
  int fd = m_fd.load();
  if (fd < 0) return false;
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd, VIDIOC_STREAMOFF, &type) != 0) return false;
  SDEBUG4("disabled streaming");
  m_streaming = false;
  return true;
}

void UsbCameraImpl::DeviceProcessCommands() {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_commands.empty()) return;
  while (!m_commands.empty()) {
    auto msg = std::move(m_commands.back());
    m_commands.pop_back();

    if (msg->kind == Message::kCmdSetMode ||
        msg->kind == Message::kCmdSetPixelFormat ||
        msg->kind == Message::kCmdSetResolution ||
        msg->kind == Message::kCmdSetFPS) {
      VideoMode newMode;
      if (msg->kind == Message::kCmdSetMode) {
        newMode.pixelFormat = msg->data[0];
        newMode.width = msg->data[1];
        newMode.height = msg->data[2];
        newMode.fps = msg->data[3];
        m_modeSetPixelFormat = true;
        m_modeSetResolution = true;
        m_modeSetFPS = true;
      } else if (msg->kind == Message::kCmdSetPixelFormat) {
        newMode = m_mode;
        newMode.pixelFormat = msg->data[0];
        m_modeSetPixelFormat = true;
      } else if (msg->kind == Message::kCmdSetResolution) {
        newMode = m_mode;
        newMode.width = msg->data[0];
        newMode.height = msg->data[1];
        m_modeSetResolution = true;
      } else if (msg->kind == Message::kCmdSetFPS) {
        newMode = m_mode;
        newMode.fps = msg->data[0];
        m_modeSetFPS = true;
      }

      // If the pixel format or resolution changed, we need to disconnect and
      // reconnect
      if (newMode.pixelFormat != m_mode.pixelFormat ||
          newMode.width != m_mode.width || newMode.height != m_mode.height) {
        m_mode = newMode;
        lock.unlock();
        bool wasStreaming = m_streaming;
        if (wasStreaming) DeviceStreamOff();
        if (m_fd >= 0) {
          DeviceDisconnect();
          DeviceConnect();
        }
        if (wasStreaming) DeviceStreamOn();
        Notifier::GetInstance().NotifySource(*this,
                                             CS_SOURCE_VIDEOMODE_CHANGED);
        lock.lock();
      } else if (newMode.fps != m_mode.fps) {
        m_mode = newMode;
        lock.unlock();
        // Need to stop streaming to set FPS
        bool wasStreaming = m_streaming;
        if (wasStreaming) DeviceStreamOff();
        DeviceSetFPS();
        if (wasStreaming) DeviceStreamOn();
        Notifier::GetInstance().NotifySource(*this,
                                             CS_SOURCE_VIDEOMODE_CHANGED);
        lock.lock();
      }
    } else if (msg->kind == Message::kCmdSetProperty ||
               msg->kind == Message::kCmdSetPropertyStr) {
      bool setString = (msg->kind == Message::kCmdSetPropertyStr);
      int property = msg->data[0];
      int value = msg->data[1];
      CS_StatusValue status =
          DeviceCmdSetProperty(lock, property, setString, value, msg->dataStr);
      if (status == CS_OK) {
        msg->kind = Message::kOk;
      } else {
        msg->kind = Message::kError;
        msg->data[0] = status;
      }
    } else if (msg->kind == Message::kNumSinksChanged ||
               msg->kind == Message::kNumSinksEnabledChanged) {
      // These are send-only messages, so recycle here.  DestroyMessage needs
      // the mutex, so unlock it.  We don't actually do anything directly
      // based on these messages (instead we check in the main loop), but
      // they do wake up the thread.
      lock.unlock();
      DestroyMessage(std::move(msg));
      lock.lock();
    } else {
      msg->kind = Message::kNone;
    }

    if (msg) m_responses.emplace_back(std::move(msg));
  }
  lock.unlock();
  m_responseCv.notify_all();
}

void UsbCameraImpl::DeviceSetMode() {
  int fd = m_fd.load();
  if (fd < 0) return;

  struct v4l2_format vfmt;
  std::memset(&vfmt, 0, sizeof(vfmt));
#ifdef V4L2_CAP_EXT_PIX_FORMAT
  vfmt.fmt.pix.priv = (m_capabilities & V4L2_CAP_EXT_PIX_FORMAT) != 0
                          ? V4L2_PIX_FMT_PRIV_MAGIC
                          : 0;
#endif
  vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  vfmt.fmt.pix.pixelformat =
      FromPixelFormat(static_cast<VideoMode::PixelFormat>(m_mode.pixelFormat));
  if (vfmt.fmt.pix.pixelformat == 0) {
    SWARNING("could not set format " << m_mode.pixelFormat
                                     << ", defaulting to MJPEG");
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  }
  vfmt.fmt.pix.width = m_mode.width;
  vfmt.fmt.pix.height = m_mode.height;
  vfmt.fmt.pix.field = V4L2_FIELD_ANY;
  if (DoIoctl(fd, VIDIOC_S_FMT, &vfmt) != 0) {
    SWARNING("could not set format " << m_mode.pixelFormat << " res "
                                     << m_mode.width << "x" << m_mode.height);
  } else {
    SINFO("set format " << m_mode.pixelFormat << " res " << m_mode.width << "x"
                        << m_mode.height);
  }
}

void UsbCameraImpl::DeviceSetFPS() {
  int fd = m_fd.load();
  if (fd < 0) return;

  struct v4l2_streamparm parm;
  std::memset(&parm, 0, sizeof(parm));
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd, VIDIOC_G_PARM, &parm) != 0) return;
  if ((parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) == 0) return;
  std::memset(&parm, 0, sizeof(parm));
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  parm.parm.capture.timeperframe = FPSToFract(m_mode.fps);
  if (DoIoctl(fd, VIDIOC_S_PARM, &parm) != 0)
    SWARNING("could not set FPS to " << m_mode.fps);
  else
    SINFO("set FPS to " << m_mode.fps);
}

void UsbCameraImpl::DeviceCacheMode() {
  int fd = m_fd.load();
  if (fd < 0) return;

  // Get format
  struct v4l2_format vfmt;
  std::memset(&vfmt, 0, sizeof(vfmt));
#ifdef V4L2_CAP_EXT_PIX_FORMAT
  vfmt.fmt.pix.priv = (m_capabilities & V4L2_CAP_EXT_PIX_FORMAT) != 0
                          ? V4L2_PIX_FMT_PRIV_MAGIC
                          : 0;
#endif
  vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd, VIDIOC_G_FMT, &vfmt) != 0) {
    SERROR("could not read current video mode");
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mode = VideoMode{VideoMode::kMJPEG, 320, 240, 30};
    return;
  }
  VideoMode::PixelFormat pixelFormat = ToPixelFormat(vfmt.fmt.pix.pixelformat);
  int width = vfmt.fmt.pix.width;
  int height = vfmt.fmt.pix.height;

  // Get FPS
  int fps = 0;
  struct v4l2_streamparm parm;
  std::memset(&parm, 0, sizeof(parm));
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (TryIoctl(fd, VIDIOC_G_PARM, &parm) == 0) {
    if (parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
      fps = FractToFPS(parm.parm.capture.timeperframe);
  }

  // Update format with user changes.
  bool formatChanged = false;

  if (m_modeSetPixelFormat) {
    // User set pixel format
    if (pixelFormat != m_mode.pixelFormat) {
      formatChanged = true;
      pixelFormat = static_cast<VideoMode::PixelFormat>(m_mode.pixelFormat);
    }
  } else {
    // Default to MJPEG
    if (pixelFormat != VideoMode::kMJPEG) {
      formatChanged = true;
      pixelFormat = VideoMode::kMJPEG;
    }
  }

  if (m_modeSetResolution) {
    // User set resolution
    if (width != m_mode.width || height != m_mode.height) {
      formatChanged = true;
      width = m_mode.width;
      height = m_mode.height;
    }
  } else {
    // Default to lowest known resolution (based on number of total pixels)
    int numPixels = width * height;
    for (const auto& mode : m_videoModes) {
      if (mode.pixelFormat != pixelFormat) continue;
      int numPixelsHere = mode.width * mode.height;
      if (numPixelsHere < numPixels) {
        formatChanged = true;
        numPixels = numPixelsHere;
        width = mode.width;
        height = mode.height;
      }
    }
  }

  // Update FPS with user changes
  bool fpsChanged = false;
  if (m_modeSetFPS && fps != m_mode.fps) {
    fpsChanged = true;
    fps = m_mode.fps;
  }

  // Save to global mode
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mode.pixelFormat = pixelFormat;
    m_mode.width = width;
    m_mode.height = height;
    m_mode.fps = fps;
  }

  if (formatChanged) DeviceSetMode();
  if (fpsChanged) DeviceSetFPS();

  Notifier::GetInstance().NotifySource(*this, CS_SOURCE_VIDEOMODE_CHANGED);
}

void UsbCameraImpl::NotifyPropertyCreated(int propIndex, PropertyData& prop) {
  auto& notifier = Notifier::GetInstance();
  notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, propIndex,
                                prop.propKind, prop.value, prop.valueStr);
  // also notify choices updated event for enum types
  if (prop.propKind == CS_PROP_ENUM)
    notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                  propIndex, prop.propKind, prop.value,
                                  llvm::StringRef{});
}

void UsbCameraImpl::DeviceCacheProperty(std::unique_ptr<PropertyData> rawProp) {
  // For percentage properties, we want to cache both the raw and the
  // percentage versions.  This function is always called with prop being
  // the raw property (as it's coming from the camera) so if required, we need
  // to rename this one as well as create/cache the percentage version.
  //
  // This is complicated by the fact that either the percentage version or the
  // the raw version may have been set previously.  If both were previously set,
  // the raw version wins.
  std::unique_ptr<PropertyData> perProp;
  if (IsPercentageProperty(rawProp->name)) {
    perProp = llvm::make_unique<PropertyData>(rawProp->name, 0, *rawProp, 0, 0);
    rawProp->name = "raw_" + perProp->name;
  }

  std::unique_lock<std::mutex> lock(m_mutex);
  int* rawIndex = &m_properties[rawProp->name];
  bool newRaw = *rawIndex == 0;
  PropertyData* oldRawProp =
      newRaw ? nullptr : static_cast<PropertyData*>(GetProperty(*rawIndex));

  int* perIndex = perProp ? &m_properties[perProp->name] : nullptr;
  bool newPer = !perIndex || *perIndex == 0;
  PropertyData* oldPerProp =
      newPer ? nullptr : static_cast<PropertyData*>(GetProperty(*perIndex));

  if (oldRawProp && oldRawProp->valueSet) {
    // Merge existing raw setting and set percentage from it
    rawProp->SetValue(oldRawProp->value);
    rawProp->valueStr = std::move(oldRawProp->valueStr);

    if (perProp) {
      perProp->SetValue(RawToPercentage(*rawProp, rawProp->value));
      perProp->valueStr = rawProp->valueStr;  // copy
    }
  } else if (oldPerProp && oldPerProp->valueSet) {
    // Merge existing percentage setting and set raw from it
    perProp->SetValue(oldPerProp->value);
    perProp->valueStr = std::move(oldPerProp->valueStr);

    rawProp->SetValue(PercentageToRaw(*rawProp, perProp->value));
    rawProp->valueStr = perProp->valueStr;  // copy
  } else {
    // Read current raw value and set percentage from it
    lock.unlock();
    if (!DeviceGetProperty(rawProp.get()))
      SWARNING("failed to get property " << rawProp->name);
    lock.lock();

    if (perProp) {
      perProp->SetValue(RawToPercentage(*rawProp, rawProp->value));
      perProp->valueStr = rawProp->valueStr;  // copy
    }
  }

  // Set value on device if user-configured
  if (rawProp->valueSet) {
    if (!DeviceSetProperty(lock, *rawProp))
      SWARNING("failed to set property " << rawProp->name);
  }

  // Update pointers since we released the lock
  rawIndex = &m_properties[rawProp->name];
  perIndex = perProp ? &m_properties[perProp->name] : nullptr;

  // Get pointers before we move the std::unique_ptr values
  auto rawPropPtr = rawProp.get();
  auto perPropPtr = perProp.get();

  if (newRaw) {
    // create a new index
    *rawIndex = m_propertyData.size() + 1;
    m_propertyData.emplace_back(std::move(rawProp));
  } else {
    // update
    m_propertyData[*rawIndex - 1] = std::move(rawProp);
  }

  // Finish setting up percentage property
  if (perProp) {
    perProp->propPair = *rawIndex;
    perProp->defaultValue =
        RawToPercentage(*rawPropPtr, rawPropPtr->defaultValue);

    if (newPer) {
      // create a new index
      *perIndex = m_propertyData.size() + 1;
      m_propertyData.emplace_back(std::move(perProp));
    } else if (perIndex) {
      // update
      m_propertyData[*perIndex - 1] = std::move(perProp);
    }

    // Tell raw property where to find percentage property
    rawPropPtr->propPair = *perIndex;
  }

  NotifyPropertyCreated(*rawIndex, *rawPropPtr);
  if (perPropPtr) NotifyPropertyCreated(*perIndex, *perPropPtr);
}

void UsbCameraImpl::DeviceCacheProperties() {
  int fd = m_fd.load();
  if (fd < 0) return;

  constexpr __u32 nextFlags = V4L2_CTRL_FLAG_NEXT_CTRL
#ifdef V4L2_CTRL_FLAG_NEXT_COMPOUND
                          | V4L2_CTRL_FLAG_NEXT_COMPOUND
#endif
      ;
  __u32 id = nextFlags;

  while (auto prop = ExtCtrlIoctl(fd, &id)) {
    DeviceCacheProperty(std::move(prop));
    id |= nextFlags;
  }

  if (id == nextFlags) {
    // try just enumerating standard...
    for (id = V4L2_CID_BASE; id < V4L2_CID_LASTP1; ++id) {
      if (auto prop = ExtCtrlIoctl(fd, &id))
        DeviceCacheProperty(std::move(prop));
    }
    // ... and custom controls
    std::unique_ptr<PropertyData> prop;
    for (id = V4L2_CID_PRIVATE_BASE; (prop = ExtCtrlIoctl(fd, &id)); ++id)
      DeviceCacheProperty(std::move(prop));
  }
}

void UsbCameraImpl::DeviceCacheVideoModes() {
  int fd = m_fd.load();
  if (fd < 0) return;

  std::vector<VideoMode> modes;

  // Pixel formats
  struct v4l2_fmtdesc fmt;
  std::memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  for (fmt.index = 0; TryIoctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0; ++fmt.index) {
    VideoMode::PixelFormat pixelFormat = ToPixelFormat(fmt.pixelformat);
    if (pixelFormat == VideoMode::kUnknown) continue;

    // Frame sizes
    struct v4l2_frmsizeenum frmsize;
    std::memset(&frmsize, 0, sizeof(frmsize));
    frmsize.pixel_format = fmt.pixelformat;
    for (frmsize.index = 0; TryIoctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0;
         ++frmsize.index) {
      if (frmsize.type != V4L2_FRMSIZE_TYPE_DISCRETE) continue;

      // Frame intervals
      struct v4l2_frmivalenum frmival;
      std::memset(&frmival, 0, sizeof(frmival));
      frmival.pixel_format = fmt.pixelformat;
      frmival.width = frmsize.discrete.width;
      frmival.height = frmsize.discrete.height;
      for (frmival.index = 0;
           TryIoctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) >= 0;
           ++frmival.index) {
        if (frmival.type != V4L2_FRMIVAL_TYPE_DISCRETE) continue;

        modes.emplace_back(pixelFormat,
                           static_cast<int>(frmsize.discrete.width),
                           static_cast<int>(frmsize.discrete.height),
                           FractToFPS(frmival.discrete));
      }
    }
  }

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_videoModes.swap(modes);
  }
  Notifier::GetInstance().NotifySource(*this, CS_SOURCE_VIDEOMODES_UPDATED);
}

bool UsbCameraImpl::DeviceGetProperty(PropertyData* prop) {
  int fd = m_fd.load();
  if (fd < 0) return true;
  int rv = 0;

  switch (prop->propKind) {
    case CS_PROP_BOOLEAN:
    case CS_PROP_INTEGER:
    case CS_PROP_ENUM:
      {
        int64_t value = 0;
        rv = GetIntCtrlIoctl(fd, prop->id, prop->type, &value);
        if (rv >= 0) prop->value = value;
      }
      break;
    case CS_PROP_STRING:
      rv = GetStringCtrlIoctl(fd, prop->id, prop->maximum, &prop->valueStr);
      break;
    default:
      break;
  }

  return rv >= 0;
}

bool UsbCameraImpl::DeviceSetProperty(std::unique_lock<std::mutex>& lock,
                                      const PropertyData& prop) {
  // Make a copy of the string as we're about to release the lock
  llvm::SmallString<128> valueStr{prop.valueStr};
  return DeviceSetProperty(lock, prop, prop.value, valueStr);
}

bool UsbCameraImpl::DeviceSetProperty(std::unique_lock<std::mutex>& lock,
                                      const PropertyData& prop, int value,
                                      llvm::StringRef valueStr) {
  int fd = m_fd.load();
  if (fd < 0) return true;
  unsigned id = prop.id;
  int rv = 0;

  switch (prop.propKind) {
    case CS_PROP_BOOLEAN:
    case CS_PROP_INTEGER:
    case CS_PROP_ENUM:
      {
        int type = prop.type;
        lock.unlock();
        rv = SetIntCtrlIoctl(fd, id, type, value);
        lock.lock();
      }
      break;
    case CS_PROP_STRING:
      {
        int maximum = prop.maximum;
        lock.unlock();
        rv = SetStringCtrlIoctl(fd, id, maximum, valueStr);
        lock.lock();
      }
      break;
    default:
      break;
  }

  return rv >= 0;
}

CS_StatusValue UsbCameraImpl::DeviceCmdSetProperty(
    std::unique_lock<std::mutex>& lock, int property, bool setString, int value,
    llvm::StringRef valueStr) {
  // Look up
  auto prop = static_cast<PropertyData*>(GetProperty(property));
  if (!prop) return CS_INVALID_PROPERTY;

  // If setting before we get, guess initial type based on set
  if (prop->propKind == CS_PROP_NONE) {
    if (setString)
      prop->propKind = CS_PROP_STRING;
    else
      prop->propKind = CS_PROP_INTEGER;
  }

  // Check kind match
  if ((setString && prop->propKind != CS_PROP_STRING) ||
      (!setString &&
       (prop->propKind & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
           0))
    return CS_WRONG_PROPERTY_TYPE;

  // Handle percentage property
  int percentageProperty = prop->propPair;
  int percentageValue = value;
  if (percentageProperty != 0) {
    if (prop->percentage) {
      std::swap(percentageProperty, property);
      prop = static_cast<PropertyData*>(GetProperty(property));
      value = PercentageToRaw(*prop, percentageValue);
    } else {
      percentageValue = RawToPercentage(*prop, value);
    }
  }

  // Actually set the new value on the device (if possible)
  if (!DeviceSetProperty(lock, *prop, value, valueStr))
    return CS_PROPERTY_WRITE_FAILED;

  // Cache the set values
  UpdatePropertyValue(property, setString, value, valueStr);
  if (percentageProperty != 0)
    UpdatePropertyValue(percentageProperty, setString, percentageValue,
                        valueStr);

  return CS_OK;
}

std::unique_ptr<UsbCameraImpl::Message> UsbCameraImpl::SendAndWait(
    std::unique_ptr<Message> msg) const {
  int fd = m_command_fd.load();
  if (fd < 0) {
    // not possible to signal, exit early
    DestroyMessage(std::move(msg));
    return nullptr;
  }

  // Use the message address as a unique identifier
  Message* ptr = msg.get();

  // Add the message to the command queue
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_commands.emplace_back(std::move(msg));
  }

  // Signal the camera thread
  if (eventfd_write(fd, 1) < 0) return nullptr;

  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    // Did we get a response to *our* request?
    auto it = std::find_if(
        m_responses.begin(), m_responses.end(),
        [=](const std::unique_ptr<Message>& m) { return m.get() == ptr; });
    if (it != m_responses.end()) {
      // Yes, remove it from the vector and we're done.
      auto rv = std::move(*it);
      m_responses.erase(it);
      return rv;
    }
    // No, keep waiting for a response
    m_responseCv.wait(lock);
  }
  return nullptr;
}

void UsbCameraImpl::Send(std::unique_ptr<Message> msg) const {
  int fd = m_command_fd.load();
  if (fd < 0) {
    // not possible to signal, exit early
    DestroyMessage(std::move(msg));
    return;
  }

  // Add the message to the command queue
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_commands.emplace_back(std::move(msg));
  }

  // Signal the camera thread
  eventfd_write(fd, 1);
}

std::unique_ptr<SourceImpl::PropertyBase> UsbCameraImpl::CreateEmptyProperty(
    llvm::StringRef name) const {
  return llvm::make_unique<PropertyData>(name);
}

bool UsbCameraImpl::CacheProperties(CS_Status* status) const {
  // Wake up camera thread; this will try to reconnect
  auto msg = CreateMessage(Message::kNone);
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) {
    *status = CS_SOURCE_IS_DISCONNECTED;
    return false;
  }
  DestroyMessage(std::move(msg));
  if (!m_properties_cached) {
    *status = CS_SOURCE_IS_DISCONNECTED;
    return false;
  }
  return true;
}

void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {
  auto msg = CreateMessage(Message::kCmdSetProperty);
  msg->data[0] = property;
  msg->data[1] = value;
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) return;
  if (msg->kind == Message::kError) *status = msg->data[0];
  DestroyMessage(std::move(msg));
}

void UsbCameraImpl::SetStringProperty(int property, llvm::StringRef value,
                                      CS_Status* status) {
  auto msg = CreateMessage(Message::kCmdSetPropertyStr);
  msg->data[0] = property;
  msg->dataStr = value;
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) return;
  if (msg->kind == Message::kError) *status = msg->data[0];
  DestroyMessage(std::move(msg));
}

bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  auto msg = CreateMessage(Message::kCmdSetMode);
  msg->data[0] = mode.pixelFormat;
  msg->data[1] = mode.width;
  msg->data[2] = mode.height;
  msg->data[3] = mode.fps;
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) return false;
  bool rv = true;
  if (msg->kind == Message::kError) {
    *status = msg->data[0];
    rv = false;
  }
  DestroyMessage(std::move(msg));
  return rv;
}

bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                   CS_Status* status) {
  auto msg = CreateMessage(Message::kCmdSetPixelFormat);
  msg->data[0] = pixelFormat;
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) return false;
  bool rv = true;
  if (msg->kind == Message::kError) {
    *status = msg->data[0];
    rv = false;
  }
  DestroyMessage(std::move(msg));
  return rv;
}

bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {
  auto msg = CreateMessage(Message::kCmdSetResolution);
  msg->data[0] = width;
  msg->data[1] = height;
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) return false;
  bool rv = true;
  if (msg->kind == Message::kError) {
    *status = msg->data[0];
    rv = false;
  }
  DestroyMessage(std::move(msg));
  return rv;
}

bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {
  auto msg = CreateMessage(Message::kCmdSetFPS);
  msg->data[0] = fps;
  msg = std::move(SendAndWait(std::move(msg)));
  if (!msg) return false;
  bool rv = true;
  if (msg->kind == Message::kError) {
    *status = msg->data[0];
    rv = false;
  }
  DestroyMessage(std::move(msg));
  return rv;
}

void UsbCameraImpl::NumSinksChanged() {
  Send(CreateMessage(Message::kNumSinksChanged));
}

void UsbCameraImpl::NumSinksEnabledChanged() {
  Send(CreateMessage(Message::kNumSinksEnabledChanged));
}

namespace cs {

CS_Source CreateUsbCameraDev(llvm::StringRef name, int dev, CS_Status* status) {
  llvm::SmallString<32> path;
  llvm::raw_svector_ostream oss{path};
  oss << "/dev/video" << dev;
  return CreateUsbCameraPath(name, oss.str(), status);
}

CS_Source CreateUsbCameraPath(llvm::StringRef name, llvm::StringRef path,
                              CS_Status* status) {
  auto source = std::make_shared<UsbCameraImpl>(name, path);
  auto handle = Sources::GetInstance().Allocate(CS_SOURCE_USB, source);
  Notifier::GetInstance().NotifySource(name, handle, CS_SOURCE_CREATED);
  // Start thread after the source created event to ensure other events
  // come after it.
  source->Start();
  return handle;
}

std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<UsbCameraImpl&>(*data->source).GetPath();
}

std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  std::vector<UsbCameraInfo> retval;

  if (DIR* dp = opendir("/dev")) {
    while (struct dirent* ep = readdir(dp)) {
      llvm::StringRef fname{ep->d_name};
      if (!fname.startswith("video")) continue;

      UsbCameraInfo info;
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
            [](const UsbCameraInfo& a, const UsbCameraInfo& b) {
              return a.dev < b.dev;
            });

  return retval;
}

}  // namespace cs

extern "C" {

CS_Source CS_CreateUsbCameraDev(const char* name, int dev, CS_Status* status) {
  return cs::CreateUsbCameraDev(name, dev, status);
}

CS_Source CS_CreateUsbCameraPath(const char* name, const char* path,
                                 CS_Status* status) {
  return cs::CreateUsbCameraPath(name, path, status);
}

char* CS_GetUsbCameraPath(CS_Source source, CS_Status* status) {
  return ConvertToC(cs::GetUsbCameraPath(source, status));
}

CS_UsbCameraInfo* CS_EnumerateUsbCameras(int* count, CS_Status* status) {
  auto cameras = cs::EnumerateUsbCameras(status);
  CS_UsbCameraInfo* out = static_cast<CS_UsbCameraInfo*>(
      std::malloc(cameras.size() * sizeof(CS_UsbCameraInfo)));
  *count = cameras.size();
  for (std::size_t i = 0; i < cameras.size(); ++i) {
    out[i].dev = cameras[i].dev;
    out[i].path = ConvertToC(cameras[i].path);
    out[i].name = ConvertToC(cameras[i].name);
  }
  return out;
}

void CS_FreeEnumeratedUsbCameras(CS_UsbCameraInfo* cameras, int count) {
  if (!cameras) return;
  for (int i = 0; i < count; ++i) {
    std::free(cameras[i].path);
    std::free(cameras[i].name);
  }
  std::free(cameras);
}

}  // extern "C"

#endif  // __linux__
