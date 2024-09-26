// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#define _WINSOCKAPI_
#include "UsbCameraImpl.h"

#include <ks.h>
#include <ksmedia.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <shlwapi.h>
#include <windowsx.h>

#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <Dbt.h>
#include <Dshow.h>
#include <Windows.h>
#include <wpi/ConvertUTF.h>
#include <wpi/MemAlloc.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/timestamp.h>

#include "COMCreators.h"
#include "ComPtr.h"
#include "Handle.h"
#include "Instance.h"
#include "JpegUtil.h"
#include "Log.h"
#include "Notifier.h"
#include "PropertyImpl.h"
#include "Telemetry.h"
#include "WindowsMessagePump.h"
#include "c_util.h"
#include "cscore_cpp.h"

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "Shlwapi.lib")

#pragma warning(disable : 4996 4018 26451)

static constexpr int NewImageMessage = 0x0400 + 4488;
static constexpr int SetCameraMessage = 0x0400 + 254;
static constexpr int WaitForStartupMessage = 0x0400 + 294;
static constexpr int PumpReadyMessage = 0x0400 + 330;

static constexpr char const* kPropWbValue = "WhiteBalance";
static constexpr char const* kPropExValue = "Exposure";
static constexpr char const* kPropBrValue = "Brightness";
static constexpr char const* kPropConnectVerbose = "connect_verbose";

static constexpr unsigned kPropConnectVerboseId = 0;

using namespace cs;

namespace cs {

UsbCameraImpl::UsbCameraImpl(std::string_view name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             std::string_view path)
    : SourceImpl{name, logger, notifier, telemetry}, m_path{path} {
  wpi::SmallVector<wchar_t, 128> wideStorage;
  wpi::sys::windows::UTF8ToUTF16(m_path, wideStorage);
  m_widePath = std::wstring{wideStorage.data(), wideStorage.size()};
  m_deviceId = -1;
  StartMessagePump();
}

UsbCameraImpl::UsbCameraImpl(std::string_view name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             int deviceId)
    : SourceImpl{name, logger, notifier, telemetry}, m_deviceId(deviceId) {
  StartMessagePump();
}

UsbCameraImpl::~UsbCameraImpl() {
  m_messagePump = nullptr;
}

void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {
  Message msg{Message::kCmdSetProperty};
  msg.data[0] = property;
  msg.data[1] = value;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
}
void UsbCameraImpl::SetStringProperty(int property, std::string_view value,
                                      CS_Status* status) {
  Message msg{Message::kCmdSetPropertyStr};
  msg.data[0] = property;
  msg.dataStr = value;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
}

// Standard common camera properties
void UsbCameraImpl::SetBrightness(int brightness, CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropBrValue), brightness, status);
}
int UsbCameraImpl::GetBrightness(CS_Status* status) const {
  return GetProperty(GetPropertyIndex(kPropBrValue), status);
}
void UsbCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {
  // TODO
}
void UsbCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {
  // TODO
}
void UsbCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropWbValue), value, status);
}
void UsbCameraImpl::SetExposureAuto(CS_Status* status) {
  // TODO
}
void UsbCameraImpl::SetExposureHoldCurrent(CS_Status* status) {
  // TODO
}
void UsbCameraImpl::SetExposureManual(int value, CS_Status* status) {
  SetProperty(GetPropertyIndex(kPropExValue), value, status);
}

bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  if (mode.pixelFormat == VideoMode::kUnknown) {
    *status = CS_UNSUPPORTED_MODE;
    return false;
  }

  Message msg{Message::kCmdSetMode};
  msg.data[0] = mode.pixelFormat;
  msg.data[1] = mode.width;
  msg.data[2] = mode.height;
  msg.data[3] = mode.fps;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
  return result == 0;
}

bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                   CS_Status* status) {
  if (pixelFormat == VideoMode::kUnknown) {
    *status = CS_UNSUPPORTED_MODE;
    return false;
  }
  Message msg{Message::kCmdSetPixelFormat};
  msg.data[0] = pixelFormat;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
  return result == 0;
}
bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {
  Message msg{Message::kCmdSetResolution};
  msg.data[0] = width;
  msg.data[1] = height;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
  return result == 0;
}
bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {
  Message msg{Message::kCmdSetFPS};
  msg.data[0] = fps;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
  return result == 0;
}

void UsbCameraImpl::NumSinksChanged() {
  m_messagePump->PostWindowMessage<Message::Kind, Message*>(
      SetCameraMessage, Message::kNumSinksChanged, nullptr);
}
void UsbCameraImpl::NumSinksEnabledChanged() {
  m_messagePump->PostWindowMessage<Message::Kind, Message*>(
      SetCameraMessage, Message::kNumSinksEnabledChanged, nullptr);
}

void UsbCameraImpl::SetPath(std::string_view path, CS_Status* status) {
  Message msg{Message::kCmdSetPath};
  msg.dataStr = path;
  auto result =
      m_messagePump->SendWindowMessage<CS_Status, Message::Kind, Message*>(
          SetCameraMessage, msg.kind, &msg);
  *status = result;
}

std::string UsbCameraImpl::GetPath() const {
  std::scoped_lock lock(m_mutex);
  return m_path;
}

void UsbCameraImpl::StartMessagePump() {
  m_messagePump = std::make_unique<WindowsMessagePump>(
      [this](HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
        return this->PumpMain(hwnd, uiMsg, wParam, lParam);
      });
}

void UsbCameraImpl::Start() {
  m_messagePump->PostWindowMessage(PumpReadyMessage, nullptr, nullptr);
}

void UsbCameraImpl::PostRequestNewFrame() {
  m_messagePump->PostWindowMessage(NewImageMessage, nullptr, nullptr);
}

bool UsbCameraImpl::CheckDeviceChange(WPARAM wParam, DEV_BROADCAST_HDR* pHdr,
                                      bool* connected) {
  DEV_BROADCAST_DEVICEINTERFACE_A* pDi = NULL;

  *connected = false;

  if (pHdr == NULL) {
    return false;
  }
  if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
    return false;
  }

  // Compare the device name with the symbolic link.

  pDi = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE_A*>(pHdr);

  if (wpi::equals_lower(m_path, pDi->dbcc_name)) {
    if (wParam == DBT_DEVICEARRIVAL) {
      *connected = true;
      return true;
    } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
      *connected = false;
      return true;
    }
  }
  return false;
}

void UsbCameraImpl::DeviceDisconnect() {
  if (m_connectVerbose)
    SINFO("Disconnected from {}", m_path);
  m_sourceReader.Reset();
  m_mediaSource.Reset();
  if (m_imageCallback) {
    m_imageCallback->InvalidateCapture();
  }
  m_imageCallback.Reset();
  m_streaming = false;
  SetConnected(false);
}

static bool IsPercentageProperty(std::string_view name) {
  if (wpi::starts_with(name, "raw_")) {
    name = wpi::substr(name, 4);
  }
  return name == "Brightness" || name == "Contrast" || name == "Saturation" ||
         name == "Hue" || name == "Sharpness" || name == "Gain" ||
         name == "Exposure";
}

void UsbCameraImpl::ProcessFrame(IMFSample* videoSample,
                                 const VideoMode& mode) {
  if (!videoSample) {
    return;
  }

  auto currentTime = wpi::Now();

  ComPtr<IMFMediaBuffer> buf;

  if (!SUCCEEDED(videoSample->ConvertToContiguousBuffer(buf.GetAddressOf()))) {
    DWORD bcnt = 0;
    if (!SUCCEEDED(videoSample->GetBufferCount(&bcnt))) {
      return;
    }
    if (bcnt == 0) {
      return;
    }
    if (!SUCCEEDED(videoSample->GetBufferByIndex(0, buf.GetAddressOf()))) {
      return;
    }
  }

  BYTE* ptr = NULL;
  LONG pitch = 0;
  DWORD length = 0;

  // First try to access using Lock2DSize, then try Lock2D, then fallback
  // https://docs.microsoft.com/en-us/windows/win32/api/mfobjects/nf-mfobjects-imfmediabuffer-lock

  ComPtr<IMF2DBuffer> buffer2d = buf.As<IMF2DBuffer>();
  if (buffer2d) {
    BYTE* scanline0 = nullptr;
    HRESULT result;
    ComPtr<IMF2DBuffer2> buffer2d2 = buf.As<IMF2DBuffer2>();
    if (buffer2d2) {
      BYTE* datastart;
      result = buffer2d2->Lock2DSize(MF2DBuffer_LockFlags_Read, &scanline0,
                                     &pitch, &datastart, &length);
    } else {
      result = buffer2d->Lock2D(&scanline0, &pitch);
    }
    if (SUCCEEDED(result)) {
      BOOL isContiguous;
      if (pitch > 0 && SUCCEEDED(buffer2d->IsContiguousFormat(&isContiguous)) &&
          isContiguous &&
          (length || SUCCEEDED(buffer2d->GetContiguousLength(&length)))) {
        // Use the buffer pointer.
        ptr = scanline0;
      } else {
        // Release the buffer and fall back to Lock().
        buffer2d->Unlock2D();
      }
    }
  }
  if (ptr == NULL) {
    buffer2d = nullptr;
    DWORD maxsize = 0;
    if (!SUCCEEDED(buf->Lock(&ptr, &maxsize, &length))) {
      return;
    }
  }

  std::string_view data_view{reinterpret_cast<char*>(ptr), length};
  SourceImpl::PutFrame(static_cast<VideoMode::PixelFormat>(mode.pixelFormat),
                       mode.width, mode.height, data_view, currentTime);

  if (buffer2d) {
    buffer2d->Unlock2D();
  } else {
    buf->Unlock();
  }
}

LRESULT UsbCameraImpl::PumpMain(HWND hwnd, UINT uiMsg, WPARAM wParam,
                                LPARAM lParam) {
  switch (uiMsg) {
    case WM_CLOSE:
      m_sourceReader.Reset();
      m_mediaSource.Reset();
      if (m_imageCallback) {
        m_imageCallback->InvalidateCapture();
      }
      m_imageCallback.Reset();
      break;
    case PumpReadyMessage:
      // Pump Created and ready to go
      DeviceConnect();
      break;
    case WaitForStartupMessage:
      DeviceConnect();
      return CS_OK;
    case WM_DEVICECHANGE: {
      // Device potentially changed
      PDEV_BROADCAST_HDR parameter =
          reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
      // Check if we're waiting on a device path, and this is a connection
      if (m_path.empty() && wParam == DBT_DEVICEARRIVAL &&
          parameter->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
        // If path is empty, we attempted to connect with a device ID. Enumerate
        // and check
        CS_Status status = 0;
        auto devices = cs::EnumerateUsbCameras(&status);
        if (devices.size() > m_deviceId) {
          // If has device ID, use the device ID from the event
          // because of windows bug
          auto&& device = devices[m_deviceId];
          DEV_BROADCAST_DEVICEINTERFACE_A* pDi =
              reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE_A*>(parameter);
          m_path = pDi->dbcc_name;
          wpi::SmallVector<wchar_t, 128> wideStorage;
          wpi::sys::windows::UTF8ToUTF16(m_path, wideStorage);
          m_widePath = std::wstring{wideStorage.data(), wideStorage.size()};
        } else {
          // This device not found
          break;
        }
      }
      bool connected = false;
      if (CheckDeviceChange(wParam, parameter, &connected)) {
        if (connected) {
          DeviceConnect();
        } else {
          // Disconnected
          DeviceDisconnect();
        }
      }
    } break;
    case NewImageMessage: {  // New image
      if (m_streaming && m_sourceReader) {
        m_sourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL,
                                   NULL, NULL, NULL);
      }
      break;
    }
    case SetCameraMessage: {
      {
        Message* msg = reinterpret_cast<Message*>(lParam);
        Message::Kind msgKind = static_cast<Message::Kind>(wParam);
        std::unique_lock lock(m_mutex);
        auto retVal = DeviceProcessCommand(lock, msgKind, msg);
        return retVal;
      }
      break;
    }
  }
  return 0l;
}

static cs::VideoMode::PixelFormat GetFromGUID(const GUID& guid) {
  // Compare GUID to one of the supported ones
  if (IsEqualGUID(guid, MFVideoFormat_L8)) {
    return cs::VideoMode::PixelFormat::kGray;
  } else if (IsEqualGUID(guid, MFVideoFormat_L16)) {
    return cs::VideoMode::PixelFormat::kY16;
  } else if (IsEqualGUID(guid, MFVideoFormat_YUY2)) {
    return cs::VideoMode::PixelFormat::kYUYV;
  } else if (IsEqualGUID(guid, MFVideoFormat_MJPG)) {
    return cs::VideoMode::PixelFormat::kMJPEG;
  } else if (IsEqualGUID(guid, MFVideoFormat_RGB565)) {
    return cs::VideoMode::PixelFormat::kRGB565;
  } else if (IsEqualGUID(guid, MFVideoFormat_UYVY)) {
    return cs::VideoMode::PixelFormat::kUYVY;
  } else {
    return cs::VideoMode::PixelFormat::kUnknown;
  }
}

bool UsbCameraImpl::DeviceConnect() {
  if (m_mediaSource && m_sourceReader) {
    return true;
  }

  if (m_connectVerbose) {
    SINFO("Attempting to connect to USB camera on {}", m_path);
  }

  SDEBUG3("opening device");

  const wchar_t* path = m_widePath.c_str();
  m_mediaSource = CreateVideoCaptureDevice(path);

  if (!m_mediaSource) {
    return false;
  }
  auto weakThis = weak_from_this();
  auto sharedThis = weakThis.lock();
  if (sharedThis) {
    m_imageCallback = CreateSourceReaderCB(sharedThis, m_mode);
  } else {
    return false;
  }

  m_sourceReader =
      CreateSourceReader(m_mediaSource.Get(), m_imageCallback.Get());

  if (!m_sourceReader) {
    m_mediaSource.Reset();
    return false;
  }

  if (m_connectVerbose) {
    SINFO("Connected to USB camera on {}", m_path);
  }

  CS_Status st = 0;
  auto devices = EnumerateUsbCameras(&st);

  for (auto&& device : devices) {
    if (device.path == m_path) {
      SetDescription(device.name);
    }
  }

  if (!m_properties_cached) {
    SDEBUG3("caching properties");
    DeviceCacheProperties();
    DeviceCacheVideoModes();
    DeviceCacheMode();
    m_properties_cached = true;
  } else {
    SDEBUG3("restoring video mode");
    DeviceSetMode();
  }

  SetConnected(true);

  // Turn off streaming if not enabled, and turn it on if enabled
  if (IsEnabled()) {
    DeviceStreamOn();
  }
  return true;
}

std::unique_ptr<PropertyImpl> UsbCameraImpl::CreateEmptyProperty(
    std::string_view name) const {
  return nullptr;
}

bool UsbCameraImpl::CacheProperties(CS_Status* status) const {
  // Wait for Camera Thread to be started
  auto result = m_messagePump->SendWindowMessage<CS_Status>(
      WaitForStartupMessage, nullptr, nullptr);
  *status = result;
  if (*status != CS_OK) {
    return false;
  }
  if (!m_properties_cached) {
    *status = CS_SOURCE_IS_DISCONNECTED;
    return false;
  }
  return true;
}

template <typename TagProperty, typename IAM>
void UsbCameraImpl::DeviceAddProperty(std::string_view name_, TagProperty tag,
                                      IAM* pProcAmp) {
  // First see if properties exist
  bool isValid = false;
  auto property = std::make_unique<UsbCameraProperty>(name_, tag, false,
                                                      pProcAmp, &isValid);
  if (isValid) {
    DeviceCacheProperty(std::move(property), m_sourceReader.Get());
  }
}

template void UsbCameraImpl::DeviceAddProperty(std::string_view name_,
                                               tagVideoProcAmpProperty tag,
                                               IAMVideoProcAmp* pProcAmp);

template void UsbCameraImpl::DeviceAddProperty(std::string_view name_,
                                               tagCameraControlProperty tag,
                                               IAMCameraControl* pProcAmp);

#define CREATEPROPERTY(val) \
  DeviceAddProperty(#val, VideoProcAmp_##val, pProcAmp);

#define CREATECONTROLPROPERTY(val) \
  DeviceAddProperty(#val, CameraControl_##val, pCamControl);

void UsbCameraImpl::DeviceCacheProperties() {
  if (!m_sourceReader) {
    return;
  }

  IAMVideoProcAmp* pProcAmp = NULL;

  if (SUCCEEDED(m_sourceReader->GetServiceForStream(
          (DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL,
          IID_PPV_ARGS(&pProcAmp)))) {
    CREATEPROPERTY(Brightness)
    CREATEPROPERTY(Contrast)
    CREATEPROPERTY(Hue)
    CREATEPROPERTY(Saturation)
    CREATEPROPERTY(Sharpness)
    CREATEPROPERTY(Gamma)
    CREATEPROPERTY(ColorEnable)
    CREATEPROPERTY(WhiteBalance)
    CREATEPROPERTY(BacklightCompensation)
    CREATEPROPERTY(Gain)
    pProcAmp->Release();
  }

  IAMCameraControl* pCamControl = NULL;

  if (SUCCEEDED(m_sourceReader->GetServiceForStream(
          (DWORD)MF_SOURCE_READER_MEDIASOURCE, GUID_NULL,
          IID_PPV_ARGS(&pCamControl)))) {
    CREATECONTROLPROPERTY(Pan)
    CREATECONTROLPROPERTY(Tilt)
    CREATECONTROLPROPERTY(Roll)
    CREATECONTROLPROPERTY(Zoom)
    CREATECONTROLPROPERTY(Exposure)
    CREATECONTROLPROPERTY(Iris)
    CREATECONTROLPROPERTY(Focus)
    pCamControl->Release();
  }
}

int UsbCameraImpl::RawToPercentage(const UsbCameraProperty& rawProp,
                                   int rawValue) {
  return 100.0 * (rawValue - rawProp.minimum) /
         (rawProp.maximum - rawProp.minimum);
}

int UsbCameraImpl::PercentageToRaw(const UsbCameraProperty& rawProp,
                                   int percentValue) {
  return rawProp.minimum +
         (rawProp.maximum - rawProp.minimum) * (percentValue / 100.0);
}

void UsbCameraImpl::DeviceCacheProperty(
    std::unique_ptr<UsbCameraProperty> rawProp, IMFSourceReader* pProcAmp) {
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
        std::make_unique<UsbCameraProperty>(rawProp->name, 0, *rawProp, 0, 0);
    rawProp->name = "raw_" + perProp->name;
  }

  std::unique_lock lock(m_mutex);
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
    if (!rawProp->DeviceGet(lock, pProcAmp))
      SWARNING("failed to get property {}", rawProp->name);

    if (perProp) {
      perProp->SetValue(RawToPercentage(*rawProp, rawProp->value));
      perProp->valueStr = rawProp->valueStr;  // copy
    }
  }

  // Set value on device if user-configured
  if (rawProp->valueSet) {
    if (!rawProp->DeviceSet(lock, pProcAmp))
      SWARNING("failed to set property {}", rawProp->name);
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
  if (perPropPtr && perIndex)
    NotifyPropertyCreated(*perIndex, *perPropPtr);
}

CS_StatusValue UsbCameraImpl::DeviceProcessCommand(
    std::unique_lock<wpi::mutex>& lock, Message::Kind msgKind,
    const Message* msg) {
  if (msgKind == Message::kCmdSetMode ||
      msgKind == Message::kCmdSetPixelFormat ||
      msgKind == Message::kCmdSetResolution || msgKind == Message::kCmdSetFPS) {
    return DeviceCmdSetMode(lock, *msg);
  } else if (msgKind == Message::kCmdSetProperty ||
             msgKind == Message::kCmdSetPropertyStr) {
    return DeviceCmdSetProperty(lock, *msg);
    return CS_OK;
  } else if (msgKind == Message::kNumSinksChanged ||
             msgKind == Message::kNumSinksEnabledChanged) {
    // Turn On Streams
    if (!IsEnabled()) {
      DeviceStreamOff();
    } else if (!m_streaming && IsEnabled()) {
      DeviceStreamOn();
    }
    return CS_OK;
  } else if (msgKind == Message::kCmdSetPath) {
    {
      std::scoped_lock lock(m_mutex);
      m_path = msg->dataStr;
      wpi::SmallVector<wchar_t, 128> wideStorage;
      wpi::sys::windows::UTF8ToUTF16(m_path, wideStorage);
      m_widePath = std::wstring{wideStorage.data(), wideStorage.size()};
    }
    DeviceDisconnect();
    DeviceConnect();
    return CS_OK;
  } else {
    return CS_OK;
  }
}

CS_StatusValue UsbCameraImpl::DeviceCmdSetProperty(
    std::unique_lock<wpi::mutex>& lock, const Message& msg) {
  bool setString = (msg.kind == Message::kCmdSetPropertyStr);
  int property = msg.data[0];
  int value = msg.data[1];
  std::string_view valueStr = msg.dataStr;

  // Look up
  auto prop = static_cast<UsbCameraProperty*>(GetProperty(property));
  if (!prop) {
    return CS_INVALID_PROPERTY;
  }

  // If setting before we get, guess initial type based on set
  if (prop->propKind == CS_PROP_NONE) {
    if (setString) {
      prop->propKind = CS_PROP_STRING;
    } else {
      prop->propKind = CS_PROP_INTEGER;
    }
  }

  // Check kind match
  if ((setString && prop->propKind != CS_PROP_STRING) ||
      (!setString && (prop->propKind & (CS_PROP_BOOLEAN | CS_PROP_INTEGER |
                                        CS_PROP_ENUM)) == 0)) {
    return CS_WRONG_PROPERTY_TYPE;
  }

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
    if (prop->id == kPropConnectVerboseId) {
      m_connectVerbose = value;
    }
  } else {
    if (!prop->DeviceSet(lock, m_sourceReader.Get())) {
      return CS_PROPERTY_WRITE_FAILED;
    }
  }

  // Cache the set values
  UpdatePropertyValue(property, setString, value, valueStr);
  if (percentageProperty != 0)
    UpdatePropertyValue(percentageProperty, setString, percentageValue,
                        valueStr);

  return CS_OK;
}

ComPtr<IMFMediaType> UsbCameraImpl::DeviceCheckModeValid(
    const VideoMode& toCheck) {
  // Find the matching mode
  auto match =
      std::find_if(m_windowsVideoModes.begin(), m_windowsVideoModes.end(),
                   [&](std::pair<VideoMode, ComPtr<IMFMediaType>>& input) {
                     return input.first == toCheck;
                   });

  if (match == m_windowsVideoModes.end()) {
    return nullptr;
  }
  return match->second;
}

CS_StatusValue UsbCameraImpl::DeviceCmdSetMode(
    std::unique_lock<wpi::mutex>& lock, const Message& msg) {
  VideoMode newMode;
  if (msg.kind == Message::kCmdSetMode) {
    newMode.pixelFormat = msg.data[0];
    newMode.width = msg.data[1];
    newMode.height = msg.data[2];
    newMode.fps = msg.data[3];
  } else if (msg.kind == Message::kCmdSetPixelFormat) {
    newMode = m_mode;
    newMode.pixelFormat = msg.data[0];
  } else if (msg.kind == Message::kCmdSetResolution) {
    newMode = m_mode;
    newMode.width = msg.data[0];
    newMode.height = msg.data[1];
  } else if (msg.kind == Message::kCmdSetFPS) {
    newMode = m_mode;
    newMode.fps = msg.data[0];
  }

  // Check if the device is not connected, if not just apply and leave
  if (!m_properties_cached) {
    m_mode = newMode;
    return CS_OK;
  }

  // If the pixel format or resolution changed, we need to disconnect and
  // reconnect
  if (newMode != m_mode) {
    // First check if the new mode is valid
    auto newModeType = DeviceCheckModeValid(newMode);
    if (!newModeType) {
      return CS_UNSUPPORTED_MODE;
    }

    m_currentMode = std::move(newModeType);
    m_mode = newMode;
#pragma warning(push)
#pragma warning(disable : 26110)
    lock.unlock();
#pragma warning(pop)
    if (m_sourceReader) {
      DeviceDisconnect();
      DeviceConnect();
    }
    m_notifier.NotifySourceVideoMode(*this, newMode);
    lock.lock();
  }

  return CS_OK;
}

bool UsbCameraImpl::DeviceStreamOn() {
  if (m_streaming) {
    return false;
  }
  if (!m_deviceValid) {
    return false;
  }
  m_streaming = true;
  m_sourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL,
                             NULL, NULL);
  return true;
}

bool UsbCameraImpl::DeviceStreamOff() {
  m_streaming = false;
  return true;
}

void UsbCameraImpl::DeviceCacheMode() {
  if (!m_sourceReader) {
    return;
  }

  if (m_windowsVideoModes.size() == 0) {
    return;
  }

  if (!m_currentMode) {
    // First, see if our set mode is valid
    m_currentMode = DeviceCheckModeValid(m_mode);
    if (!m_currentMode) {
      if (FAILED(m_sourceReader->GetCurrentMediaType(
              MF_SOURCE_READER_FIRST_VIDEO_STREAM,
              m_currentMode.GetAddressOf()))) {
        return;
      }
      // Find cached version
      DWORD compare = MF_MEDIATYPE_EQUAL_MAJOR_TYPES |
                      MF_MEDIATYPE_EQUAL_FORMAT_TYPES |
                      MF_MEDIATYPE_EQUAL_FORMAT_DATA;
      auto result = std::find_if(
          m_windowsVideoModes.begin(), m_windowsVideoModes.end(),
          [this, &compare](std::pair<VideoMode, ComPtr<IMFMediaType>>& input) {
            return input.second->IsEqual(m_currentMode.Get(), &compare) == S_OK;
          });

      if (result == m_windowsVideoModes.end()) {
        // Default mode is not supported. Grab first supported image
        auto&& firstSupported = m_windowsVideoModes[0];
        m_currentMode = firstSupported.second;
        std::scoped_lock lock(m_mutex);
        m_mode = firstSupported.first;
      } else {
        std::scoped_lock lock(m_mutex);
        m_mode = result->first;
      }
    }
  }

  DeviceSetMode();

  m_notifier.NotifySourceVideoMode(*this, m_mode);
}

CS_StatusValue UsbCameraImpl::DeviceSetMode() {
  HRESULT setResult = m_sourceReader->SetCurrentMediaType(
      MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, m_currentMode.Get());

  m_deviceValid = SUCCEEDED(setResult);

  m_imageCallback->SetVideoMode(m_mode);

  switch (setResult) {
    case S_OK:
      return CS_OK;
      break;
    case MF_E_INVALIDMEDIATYPE:
      break;
    case MF_E_INVALIDREQUEST:
      break;
    case MF_E_INVALIDSTREAMNUMBER:
      break;
    case MF_E_TOPO_CODEC_NOT_FOUND:
      break;
  }

  return CS_UNSUPPORTED_MODE;
}

void UsbCameraImpl::DeviceCacheVideoModes() {
  if (!m_sourceReader) {
    return;
  }

  std::vector<VideoMode> modes;
  m_windowsVideoModes.clear();

  bool set = false;

  ComPtr<IMFMediaType> nativeType;
  int count = 0;
  while (true) {
    nativeType.Reset();
    auto hr = m_sourceReader->GetNativeMediaType(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM, count, nativeType.GetAddressOf());
    if (FAILED(hr)) {
      break;
    }
    GUID guid = {0};
    nativeType->GetGUID(MF_MT_SUBTYPE, &guid);

    auto format = GetFromGUID(guid);
    if (format == VideoMode::kUnknown) {
      count++;
      // Don't put in unknowns
      continue;
    }
    UINT32 width, height;
    ::MFGetAttributeSize(nativeType.Get(), MF_MT_FRAME_SIZE, &width, &height);

    UINT32 num, dom;
    ::MFGetAttributeRatio(nativeType.Get(), MF_MT_FRAME_RATE, &num, &dom);

    int fps = 30;

    if (dom != 0) {
      fps = std::ceil(num / static_cast<double>(dom));
    }

    VideoMode newMode = {format, static_cast<int>(width),
                         static_cast<int>(height), fps};

    modes.emplace_back(newMode);
    m_windowsVideoModes.emplace_back(newMode, nativeType);
    count++;
  }
  {
    std::scoped_lock lock(m_mutex);
    m_videoModes.swap(modes);
  }
  m_notifier.NotifySource(*this, CS_SOURCE_VIDEOMODES_UPDATED);
}

static void ParseVidAndPid(std::string_view path, int* pid, int* vid) {
  auto vidIndex = wpi::find_lower(path, "vid_");
  auto pidIndex = wpi::find_lower(path, "pid_");

  if (vidIndex != std::string_view::npos) {
    auto vidSlice = wpi::slice(path, vidIndex + 4, vidIndex + 8);
    if (auto v = wpi::parse_integer<uint16_t>(vidSlice, 16)) {
      *vid = v.value();
    }
  }

  if (pidIndex != std::string_view::npos) {
    auto pidSlice = wpi::slice(path, pidIndex + 4, pidIndex + 8);
    if (auto v = wpi::parse_integer<uint16_t>(pidSlice, 16)) {
      *pid = v.value();
    }
  }
}

std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  std::vector<UsbCameraInfo> retval;

  // Ensure we are initialized by grabbing the message pump
  // GetMessagePump();

  wpi::SmallString<128> storage;
  WCHAR buf[512];
  ComPtr<IMFAttributes> pAttributes;
  IMFActivate** ppDevices = nullptr;
  UINT32 count = 0;

  // Create an attribute store to specify the enumeration parameters.
  HRESULT hr = MFCreateAttributes(pAttributes.GetAddressOf(), 1);
  if (FAILED(hr)) {
    goto done;
  }

  // Source type: video capture devices
  hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
  if (FAILED(hr)) {
    goto done;
  }

  // Enumerate devices.
  hr = MFEnumDeviceSources(pAttributes.Get(), &ppDevices, &count);
  if (FAILED(hr)) {
    goto done;
  }

  if (count == 0) {
    hr = E_FAIL;
    goto done;
  }

  for (UINT32 i = 0; i < count; i++) {
    UsbCameraInfo info;
    info.dev = i;

    UINT32 characters = 0;
    ppDevices[i]->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, buf,
                            sizeof(buf) / sizeof(WCHAR), &characters);
    storage.clear();
    wpi::sys::windows::UTF16ToUTF8(buf, characters, storage);
    info.name = std::string{storage};
    ppDevices[i]->GetString(
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, buf,
        sizeof(buf) / sizeof(WCHAR), &characters);
    storage.clear();
    wpi::sys::windows::UTF16ToUTF8(buf, characters, storage);
    info.path = std::string{storage};

    // Try to parse path from symbolic link
    ParseVidAndPid(info.path, &info.productId, &info.vendorId);

    retval.emplace_back(std::move(info));
  }

done:
  pAttributes.Reset();

  if (ppDevices) {
    for (DWORD i = 0; i < count; i++) {
      if (ppDevices[i]) {
        ppDevices[i]->Release();
        ppDevices[i] = nullptr;
      }
    }
  }

  CoTaskMemFree(ppDevices);
  return retval;
}

CS_Source CreateUsbCameraDev(std::string_view name, int dev,
                             CS_Status* status) {
  // First check if device exists
  auto devices = cs::EnumerateUsbCameras(status);
  if (devices.size() > dev) {
    return CreateUsbCameraPath(name, devices[dev].path, status);
  }
  auto& inst = Instance::GetInstance();
  auto source = std::make_shared<UsbCameraImpl>(
      name, inst.logger, inst.notifier, inst.telemetry, dev);
  return inst.CreateSource(CS_SOURCE_USB, source);
}

CS_Source CreateUsbCameraPath(std::string_view name, std::string_view path,
                              CS_Status* status) {
  auto& inst = Instance::GetInstance();
  auto source = std::make_shared<UsbCameraImpl>(
      name, inst.logger, inst.notifier, inst.telemetry, path);
  return inst.CreateSource(CS_SOURCE_USB, source);
}

void SetUsbCameraPath(CS_Source source, std::string_view path,
                      CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<UsbCameraImpl&>(*data->source).SetPath(path, status);
}

std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<UsbCameraImpl&>(*data->source).GetPath();
}

UsbCameraInfo GetUsbCameraInfo(CS_Source source, CS_Status* status) {
  UsbCameraInfo info;
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return info;
  }

  info.path = static_cast<UsbCameraImpl&>(*data->source).GetPath();
  wpi::SmallVector<char, 64> buf;
  info.name = static_cast<UsbCameraImpl&>(*data->source).GetDescription(buf);
  ParseVidAndPid(info.path, &info.productId, &info.vendorId);
  info.dev = -1;  // We have lost dev information by this point in time.
  return info;
}

}  // namespace cs
