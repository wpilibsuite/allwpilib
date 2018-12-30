/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "UsbCameraImpl.h"

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

#include <algorithm>

#include <wpi/FileSystem.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/memory.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "JpegUtil.h"
#include "Log.h"
#include "Notifier.h"
#include "Telemetry.h"
#include "UsbUtil.h"
#include "cscore_cpp.h"

using namespace cs;

static constexpr char const* kPropWbAuto = "white_balance_temperature_auto";
static constexpr char const* kPropWbValue = "white_balance_temperature";
static constexpr char const* kPropExAuto = "exposure_auto";
static constexpr char const* kPropExValue = "exposure_absolute";
static constexpr char const* kPropBrValue = "brightness";
static constexpr char const* kPropConnectVerbose = "connect_verbose";
static constexpr unsigned kPropConnectVerboseId = 0;

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
    case V4L2_PIX_FMT_GREY:
      return VideoMode::kGray;
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
    case VideoMode::kGray:
      return V4L2_PIX_FMT_GREY;
    default:
      return 0;
  }
}

static bool IsPercentageProperty(wpi::StringRef name) {
  if (name.startswith("raw_")) name = name.substr(4);
  return name == "brightness" || name == "contrast" || name == "saturation" ||
         name == "hue" || name == "sharpness" || name == "gain" ||
         name == "exposure_absolute";
}

static constexpr const int quirkLifeCamHd3000[] = {
    5, 10, 20, 39, 78, 156, 312, 625, 1250, 2500, 5000, 10000, 20000};

int UsbCameraImpl::RawToPercentage(const UsbCameraProperty& rawProp,
                                   int rawValue) {
  // LifeCam exposure setting quirk
  if (m_lifecam_exposure && rawProp.name == "raw_exposure_absolute" &&
      rawProp.minimum == 5 && rawProp.maximum == 20000) {
    int nelems = wpi::array_lengthof(quirkLifeCamHd3000);
    for (int i = 0; i < nelems; ++i) {
      if (rawValue < quirkLifeCamHd3000[i]) return 100.0 * i / nelems;
    }
    return 100;
  }
  return 100.0 * (rawValue - rawProp.minimum) /
         (rawProp.maximum - rawProp.minimum);
}

int UsbCameraImpl::PercentageToRaw(const UsbCameraProperty& rawProp,
                                   int percentValue) {
  // LifeCam exposure setting quirk
  if (m_lifecam_exposure && rawProp.name == "raw_exposure_absolute" &&
      rawProp.minimum == 5 && rawProp.maximum == 20000) {
    int nelems = wpi::array_lengthof(quirkLifeCamHd3000);
    int ndx = nelems * percentValue / 100.0;
    if (ndx < 0) ndx = 0;
    if (ndx >= nelems) ndx = nelems - 1;
    return quirkLifeCamHd3000[ndx];
  }
  return rawProp.minimum +
         (rawProp.maximum - rawProp.minimum) * (percentValue / 100.0);
}

static bool GetDescriptionSysV4L(wpi::StringRef path, std::string* desc) {
  wpi::SmallString<64> ifpath{"/sys/class/video4linux/"};
  ifpath += path.substr(5);
  ifpath += "/device/interface";

  int fd = open(ifpath.c_str(), O_RDONLY);
  if (fd < 0) return false;

  char readBuf[128];
  ssize_t n = read(fd, readBuf, sizeof(readBuf));
  close(fd);

  if (n <= 0) return false;

  *desc = wpi::StringRef(readBuf, n).rtrim();
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

  wpi::StringRef card{reinterpret_cast<const char*>(vcap.card)};
  // try to convert "UVC Camera (0000:0000)" into a better name
  int vendor = 0;
  int product = 0;
  if (card.startswith("UVC Camera (") &&
      !card.substr(12, 4).getAsInteger(16, vendor) &&
      !card.substr(17, 4).getAsInteger(16, product)) {
    wpi::SmallString<64> card2Buf;
    wpi::StringRef card2 = GetUsbNameFromId(vendor, product, card2Buf);
    if (!card2.empty()) {
      *desc = card2;
      return true;
    }
  }

  *desc = card;
  return true;
}

static std::string GetDescriptionImpl(const char* cpath) {
  wpi::StringRef path{cpath};
  char pathBuf[128];
  std::string rv;

  // If trying to get by id or path, follow symlink
  if (path.startswith("/dev/v4l/by-id/")) {
    ssize_t n = readlink(cpath, pathBuf, sizeof(pathBuf));
    if (n > 0) path = wpi::StringRef(pathBuf, n);
  } else if (path.startswith("/dev/v4l/by-path/")) {
    ssize_t n = readlink(cpath, pathBuf, sizeof(pathBuf));
    if (n > 0) path = wpi::StringRef(pathBuf, n);
  }

  if (path.startswith("/dev/video")) {
    // Sometimes the /sys tree gives a better name.
    if (GetDescriptionSysV4L(path, &rv)) return rv;
  }

  // Otherwise use an ioctl to query the caps and get the card name
  if (GetDescriptionIoctl(cpath, &rv)) return rv;

  return std::string{};
}

UsbCameraImpl::UsbCameraImpl(const wpi::Twine& name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             const wpi::Twine& path)
    : SourceImpl{name, logger, notifier, telemetry},
      m_path{path.str()},
      m_fd{-1},
      m_command_fd{eventfd(0, 0)},
      m_active{true} {
  SetDescription(GetDescriptionImpl(m_path.c_str()));
  SetQuirks();

  CreateProperty(kPropConnectVerbose, [] {
    return std::make_unique<UsbCameraProperty>(kPropConnectVerbose,
                                               kPropConnectVerboseId,
                                               CS_PROP_INTEGER, 0, 1, 1, 1, 1);
  });
}

UsbCameraImpl::~UsbCameraImpl() {
  m_active = false;

  // Just in case anyone is waiting...
  m_responseCv.notify_all();

  // Send message to wake up thread; select timeout will wake us up anyway,
  // but this speeds shutdown.
  Send(Message{Message::kNone});

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
    wpi::SmallString<64> pathCopy{m_path};
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
  wpi::SmallString<64> pathCopy{m_path};
  pathCopy.push_back('\0');
  wpi::SmallString<64> base{basename(pathCopy.data())};

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

    // Turn off streaming if not enabled, and turn it on if enabled
    if (m_streaming && !IsEnabled()) {
      DeviceStreamOff();
    } else if (!m_streaming && IsEnabled()) {
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
      SERROR("select(): " << std::strerror(errno));
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
        wpi::SmallString<64> raw_name;
        raw_name.resize(event.len);
        notify_is->read(raw_name.data(), event.len);
        // If the name is what we expect...
        wpi::StringRef name{raw_name.c_str()};
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
        continue;         // will reconnect
      }

      if ((buf.flags & V4L2_BUF_FLAG_ERROR) == 0) {
        SDEBUG4("got image size=" << buf.bytesused << " index=" << buf.index);

        if (buf.index >= kNumBuffers || !m_buffers[buf.index].m_data) {
          SWARNING("invalid buffer" << buf.index);
          continue;
        }

        wpi::StringRef image{
            static_cast<const char*>(m_buffers[buf.index].m_data),
            static_cast<size_t>(buf.bytesused)};
        int width = m_mode.width;
        int height = m_mode.height;
        bool good = true;
        if (m_mode.pixelFormat == VideoMode::kMJPEG &&
            !GetJpegSize(image, &width, &height)) {
          SWARNING("invalid JPEG image received from camera");
          good = false;
        }
        if (good) {
          PutFrame(static_cast<VideoMode::PixelFormat>(m_mode.pixelFormat),
                   width, height, image, wpi::Now());  // TODO: time
        }
      }

      // Requeue buffer
      if (DoIoctl(fd, VIDIOC_QBUF, &buf) != 0) {
        SWARNING("could not requeue buffer");
        wasStreaming = m_streaming;
        DeviceStreamOff();
        DeviceDisconnect();
        notified = true;  // device wasn't deleted, just error'ed
        continue;         // will reconnect
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

  if (m_connectVerbose) SINFO("Connecting to USB camera on " << m_path);

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
    std::unique_lock<wpi::mutex> lock2(m_mutex);
    for (size_t i = 0; i < m_propertyData.size(); ++i) {
      const auto prop =
          static_cast<const UsbCameraProperty*>(m_propertyData[i].get());
      if (!prop || !prop->valueSet || !prop->device || prop->percentage)
        continue;
      if (!prop->DeviceSet(lock2, m_fd))
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

  // Update quirks settings
  SetQuirks();

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

CS_StatusValue UsbCameraImpl::DeviceCmdSetMode(
    std::unique_lock<wpi::mutex>& lock, const Message& msg) {
  VideoMode newMode;
  if (msg.kind == Message::kCmdSetMode) {
    newMode.pixelFormat = msg.data[0];
    newMode.width = msg.data[1];
    newMode.height = msg.data[2];
    newMode.fps = msg.data[3];
    m_modeSetPixelFormat = true;
    m_modeSetResolution = true;
    m_modeSetFPS = true;
  } else if (msg.kind == Message::kCmdSetPixelFormat) {
    newMode = m_mode;
    newMode.pixelFormat = msg.data[0];
    m_modeSetPixelFormat = true;
  } else if (msg.kind == Message::kCmdSetResolution) {
    newMode = m_mode;
    newMode.width = msg.data[0];
    newMode.height = msg.data[1];
    m_modeSetResolution = true;
  } else if (msg.kind == Message::kCmdSetFPS) {
    newMode = m_mode;
    newMode.fps = msg.data[0];
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
    m_notifier.NotifySourceVideoMode(*this, newMode);
    lock.lock();
  } else if (newMode.fps != m_mode.fps) {
    m_mode = newMode;
    lock.unlock();
    // Need to stop streaming to set FPS
    bool wasStreaming = m_streaming;
    if (wasStreaming) DeviceStreamOff();
    DeviceSetFPS();
    if (wasStreaming) DeviceStreamOn();
    m_notifier.NotifySourceVideoMode(*this, newMode);
    lock.lock();
  }

  return CS_OK;
}

CS_StatusValue UsbCameraImpl::DeviceCmdSetProperty(
    std::unique_lock<wpi::mutex>& lock, const Message& msg) {
  bool setString = (msg.kind == Message::kCmdSetPropertyStr);
  int property = msg.data[0];
  int value = msg.data[1];
  wpi::StringRef valueStr = msg.dataStr;

  // Look up
  auto prop = static_cast<UsbCameraProperty*>(GetProperty(property));
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
      (!setString && (prop->propKind &
                      (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) == 0))
    return CS_WRONG_PROPERTY_TYPE;

  // Handle percentage property
  int percentageProperty = prop->propPair;
  int percentageValue = value;
  if (percentageProperty != 0) {
    if (prop->percentage) {
      std::swap(percentageProperty, property);
      prop = static_cast<UsbCameraProperty*>(GetProperty(property));
      value = PercentageToRaw(*prop, percentageValue);
    } else {
      percentageValue = RawToPercentage(*prop, value);
    }
  }

  // Actually set the new value on the device (if possible)
  if (!prop->device) {
    if (prop->id == kPropConnectVerboseId) m_connectVerbose = value;
  } else {
    if (!prop->DeviceSet(lock, m_fd, value, valueStr))
      return CS_PROPERTY_WRITE_FAILED;
  }

  // Cache the set values
  UpdatePropertyValue(property, setString, value, valueStr);
  if (percentageProperty != 0)
    UpdatePropertyValue(percentageProperty, setString, percentageValue,
                        valueStr);

  return CS_OK;
}

CS_StatusValue UsbCameraImpl::DeviceProcessCommand(
    std::unique_lock<wpi::mutex>& lock, const Message& msg) {
  if (msg.kind == Message::kCmdSetMode ||
      msg.kind == Message::kCmdSetPixelFormat ||
      msg.kind == Message::kCmdSetResolution ||
      msg.kind == Message::kCmdSetFPS) {
    return DeviceCmdSetMode(lock, msg);
  } else if (msg.kind == Message::kCmdSetProperty ||
             msg.kind == Message::kCmdSetPropertyStr) {
    return DeviceCmdSetProperty(lock, msg);
  } else if (msg.kind == Message::kNumSinksChanged ||
             msg.kind == Message::kNumSinksEnabledChanged) {
    return CS_OK;
  } else {
    return CS_OK;
  }
}

void UsbCameraImpl::DeviceProcessCommands() {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (m_commands.empty()) return;
  while (!m_commands.empty()) {
    auto msg = std::move(m_commands.back());
    m_commands.pop_back();

    CS_StatusValue status = DeviceProcessCommand(lock, msg);
    if (msg.kind != Message::kNumSinksChanged &&
        msg.kind != Message::kNumSinksEnabledChanged)
      m_responses.emplace_back(msg.from, status);
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
    std::lock_guard<wpi::mutex> lock(m_mutex);
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
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_mode.pixelFormat = pixelFormat;
    m_mode.width = width;
    m_mode.height = height;
    m_mode.fps = fps;
  }

  if (formatChanged) DeviceSetMode();
  if (fpsChanged) DeviceSetFPS();

  m_notifier.NotifySourceVideoMode(*this, m_mode);
}

void UsbCameraImpl::DeviceCacheProperty(
    std::unique_ptr<UsbCameraProperty> rawProp) {
  // For percentage properties, we want to cache both the raw and the
  // percentage versions.  This function is always called with prop being
  // the raw property (as it's coming from the camera) so if required, we need
  // to rename this one as well as create/cache the percentage version.
  //
  // This is complicated by the fact that either the percentage version or the
  // the raw version may have been set previously.  If both were previously set,
  // the raw version wins.
  std::unique_ptr<UsbCameraProperty> perProp;
  if (IsPercentageProperty(rawProp->name)) {
    perProp =
        wpi::make_unique<UsbCameraProperty>(rawProp->name, 0, *rawProp, 0, 0);
    rawProp->name = "raw_" + perProp->name;
  }

  std::unique_lock<wpi::mutex> lock(m_mutex);
  int* rawIndex = &m_properties[rawProp->name];
  bool newRaw = *rawIndex == 0;
  UsbCameraProperty* oldRawProp =
      newRaw ? nullptr
             : static_cast<UsbCameraProperty*>(GetProperty(*rawIndex));

  int* perIndex = perProp ? &m_properties[perProp->name] : nullptr;
  bool newPer = !perIndex || *perIndex == 0;
  UsbCameraProperty* oldPerProp =
      newPer ? nullptr
             : static_cast<UsbCameraProperty*>(GetProperty(*perIndex));

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
    if (!rawProp->DeviceGet(lock, m_fd))
      SWARNING("failed to get property " << rawProp->name);

    if (perProp) {
      perProp->SetValue(RawToPercentage(*rawProp, rawProp->value));
      perProp->valueStr = rawProp->valueStr;  // copy
    }
  }

  // Set value on device if user-configured
  if (rawProp->valueSet) {
    if (!rawProp->DeviceSet(lock, m_fd))
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

#ifdef V4L2_CTRL_FLAG_NEXT_COMPOUND
  constexpr __u32 nextFlags =
      V4L2_CTRL_FLAG_NEXT_CTRL | V4L2_CTRL_FLAG_NEXT_COMPOUND;
#else
  constexpr __u32 nextFlags = V4L2_CTRL_FLAG_NEXT_CTRL;
#endif
  __u32 id = nextFlags;

  while (auto prop = UsbCameraProperty::DeviceQuery(fd, &id)) {
    DeviceCacheProperty(std::move(prop));
    id |= nextFlags;
  }

  if (id == nextFlags) {
    // try just enumerating standard...
    for (id = V4L2_CID_BASE; id < V4L2_CID_LASTP1; ++id) {
      if (auto prop = UsbCameraProperty::DeviceQuery(fd, &id))
        DeviceCacheProperty(std::move(prop));
    }
    // ... and custom controls
    std::unique_ptr<UsbCameraProperty> prop;
    for (id = V4L2_CID_PRIVATE_BASE;
         (prop = UsbCameraProperty::DeviceQuery(fd, &id)); ++id)
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
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_videoModes.swap(modes);
  }
  m_notifier.NotifySource(*this, CS_SOURCE_VIDEOMODES_UPDATED);
}

CS_StatusValue UsbCameraImpl::SendAndWait(Message&& msg) const {
  int fd = m_command_fd.load();
  // exit early if not possible to signal
  if (fd < 0) return CS_SOURCE_IS_DISCONNECTED;

  auto from = msg.from;

  // Add the message to the command queue
  {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_commands.emplace_back(std::move(msg));
  }

  // Signal the camera thread
  if (eventfd_write(fd, 1) < 0) return CS_SOURCE_IS_DISCONNECTED;

  std::unique_lock<wpi::mutex> lock(m_mutex);
  while (m_active) {
    // Did we get a response to *our* request?
    auto it =
        std::find_if(m_responses.begin(), m_responses.end(),
                     [=](const std::pair<std::thread::id, CS_StatusValue>& r) {
                       return r.first == from;
                     });
    if (it != m_responses.end()) {
      // Yes, remove it from the vector and we're done.
      auto rv = it->second;
      m_responses.erase(it);
      return rv;
    }
    // No, keep waiting for a response
    m_responseCv.wait(lock);
  }

  return CS_SOURCE_IS_DISCONNECTED;
}

void UsbCameraImpl::Send(Message&& msg) const {
  int fd = m_command_fd.load();
  // exit early if not possible to signal
  if (fd < 0) return;

  // Add the message to the command queue
  {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    m_commands.emplace_back(std::move(msg));
  }

  // Signal the camera thread
  eventfd_write(fd, 1);
}

std::unique_ptr<PropertyImpl> UsbCameraImpl::CreateEmptyProperty(
    const wpi::Twine& name) const {
  return wpi::make_unique<UsbCameraProperty>(name);
}

bool UsbCameraImpl::CacheProperties(CS_Status* status) const {
  // Wake up camera thread; this will try to reconnect
  *status = SendAndWait(Message{Message::kNone});
  if (*status != CS_OK) return false;
  if (!m_properties_cached) {
    *status = CS_SOURCE_IS_DISCONNECTED;
    return false;
  }
  return true;
}

void UsbCameraImpl::SetQuirks() {
  wpi::SmallString<128> descbuf;
  wpi::StringRef desc = GetDescription(descbuf);
  m_lifecam_exposure =
      desc.endswith("LifeCam HD-3000") || desc.endswith("LifeCam Cinema (TM)");
}

void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {
  Message msg{Message::kCmdSetProperty};
  msg.data[0] = property;
  msg.data[1] = value;
  *status = SendAndWait(std::move(msg));
}

void UsbCameraImpl::SetStringProperty(int property, const wpi::Twine& value,
                                      CS_Status* status) {
  Message msg{Message::kCmdSetPropertyStr};
  msg.data[0] = property;
  msg.dataStr = value.str();
  *status = SendAndWait(std::move(msg));
}

void UsbCameraImpl::SetBrightness(int brightness, CS_Status* status) {
  if (brightness > 100) {
    brightness = 100;
  } else if (brightness < 0) {
    brightness = 0;
  }
  SetProperty(GetPropertyIndex(kPropBrValue), brightness, status);
}

int UsbCameraImpl::GetBrightness(CS_Status* status) const {
  return GetProperty(GetPropertyIndex(kPropBrValue), status);
}

void UsbCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropWbAuto), 1, status);  // auto
}

void UsbCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropWbAuto), 0, status);  // manual
}

void UsbCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropWbAuto), 0, status);  // manual
  SetProperty(GetPropertyIndex(kPropWbValue), value, status);
}

void UsbCameraImpl::SetExposureAuto(CS_Status* status) {
  // auto; this is an enum value
  SetProperty(GetPropertyIndex(kPropExAuto), 3, status);
}

void UsbCameraImpl::SetExposureHoldCurrent(CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropExAuto), 1, status);  // manual
}

void UsbCameraImpl::SetExposureManual(int value, CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropExAuto), 1, status);  // manual
  if (value > 100) {
    value = 100;
  } else if (value < 0) {
    value = 0;
  }
  SetProperty(GetPropertyIndex(kPropExValue), value, status);
}

bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  Message msg{Message::kCmdSetMode};
  msg.data[0] = mode.pixelFormat;
  msg.data[1] = mode.width;
  msg.data[2] = mode.height;
  msg.data[3] = mode.fps;
  *status = SendAndWait(std::move(msg));
  return *status == CS_OK;
}

bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                   CS_Status* status) {
  Message msg{Message::kCmdSetPixelFormat};
  msg.data[0] = pixelFormat;
  *status = SendAndWait(std::move(msg));
  return *status == CS_OK;
}

bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {
  Message msg{Message::kCmdSetResolution};
  msg.data[0] = width;
  msg.data[1] = height;
  *status = SendAndWait(std::move(msg));
  return *status == CS_OK;
}

bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {
  Message msg{Message::kCmdSetFPS};
  msg.data[0] = fps;
  *status = SendAndWait(std::move(msg));
  return *status == CS_OK;
}

void UsbCameraImpl::NumSinksChanged() {
  Send(Message{Message::kNumSinksChanged});
}

void UsbCameraImpl::NumSinksEnabledChanged() {
  Send(Message{Message::kNumSinksEnabledChanged});
}

namespace cs {

CS_Source CreateUsbCameraDev(const wpi::Twine& name, int dev,
                             CS_Status* status) {
  wpi::SmallString<32> path;
  wpi::raw_svector_ostream oss{path};
  oss << "/dev/video" << dev;
  return CreateUsbCameraPath(name, oss.str(), status);
}

CS_Source CreateUsbCameraPath(const wpi::Twine& name, const wpi::Twine& path,
                              CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_USB, std::make_shared<UsbCameraImpl>(
                                              name, inst.logger, inst.notifier,
                                              inst.telemetry, path));
}

std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<UsbCameraImpl&>(*data->source).GetPath();
}

static const char* symlinkDirs[] = {"/dev/v4l/by-id", "/dev/v4l/by-path"};

UsbCameraInfo GetUsbCameraInfo(CS_Source source, CS_Status* status) {
  UsbCameraInfo info;
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return info;
  }
  std::string keypath = static_cast<UsbCameraImpl&>(*data->source).GetPath();
  info.path = keypath;

  // it might be a symlink; if so, find the symlink target (e.g. /dev/videoN),
  // add that to the list and make it the keypath
  if (wpi::sys::fs::is_symlink_file(keypath)) {
    char* target = ::realpath(keypath.c_str(), nullptr);
    if (target) {
      keypath.assign(target);
      info.otherPaths.emplace_back(keypath);
      std::free(target);
    }
  }

  // device number
  wpi::StringRef fname = wpi::sys::path::filename(keypath);
  if (fname.startswith("video")) fname.substr(5).getAsInteger(10, info.dev);

  // description
  info.name = GetDescriptionImpl(keypath.c_str());

  // look through /dev/v4l/by-id and /dev/v4l/by-path for symlinks to the
  // keypath
  wpi::SmallString<128> path;
  for (auto symlinkDir : symlinkDirs) {
    if (DIR* dp = ::opendir(symlinkDir)) {
      while (struct dirent* ep = ::readdir(dp)) {
        if (ep->d_type == DT_LNK) {
          path = symlinkDir;
          path += '/';
          path += ep->d_name;
          char* target = ::realpath(path.c_str(), nullptr);
          if (target) {
            if (keypath == target) info.otherPaths.emplace_back(path.str());
            std::free(target);
          }
        }
      }
      ::closedir(dp);
    }
  }

  // eliminate any duplicates
  std::sort(info.otherPaths.begin(), info.otherPaths.end());
  info.otherPaths.erase(
      std::unique(info.otherPaths.begin(), info.otherPaths.end()),
      info.otherPaths.end());
  return info;
}

std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  std::vector<UsbCameraInfo> retval;

  if (DIR* dp = ::opendir("/dev")) {
    while (struct dirent* ep = ::readdir(dp)) {
      wpi::StringRef fname{ep->d_name};
      if (!fname.startswith("video")) continue;

      unsigned int dev = 0;
      if (fname.substr(5).getAsInteger(10, dev)) continue;

      UsbCameraInfo info;
      info.dev = dev;

      wpi::SmallString<32> path{"/dev/"};
      path += fname;
      info.path = path.str();

      info.name = GetDescriptionImpl(path.c_str());
      if (info.name.empty()) continue;

      if (dev >= retval.size()) retval.resize(info.dev + 1);
      retval[info.dev] = std::move(info);
    }
    ::closedir(dp);
  } else {
    // *status = ;
    WPI_ERROR(Instance::GetInstance().logger, "Could not open /dev");
    return retval;
  }

  // look through /dev/v4l/by-id and /dev/v4l/by-path for symlinks to
  // /dev/videoN
  wpi::SmallString<128> path;
  for (auto symlinkDir : symlinkDirs) {
    if (DIR* dp = ::opendir(symlinkDir)) {
      while (struct dirent* ep = ::readdir(dp)) {
        if (ep->d_type == DT_LNK) {
          path = symlinkDir;
          path += '/';
          path += ep->d_name;
          char* target = ::realpath(path.c_str(), nullptr);
          if (target) {
            wpi::StringRef fname = wpi::sys::path::filename(target);
            unsigned int dev = 0;
            if (fname.startswith("video") &&
                !fname.substr(5).getAsInteger(10, dev) && dev < retval.size()) {
              retval[dev].otherPaths.emplace_back(path.str());
            }
            std::free(target);
          }
        }
      }
      ::closedir(dp);
    }
  }

  // remove devices with empty names
  retval.erase(
      std::remove_if(retval.begin(), retval.end(),
                     [](const UsbCameraInfo& x) { return x.name.empty(); }),
      retval.end());

  return retval;
}

}  // namespace cs
