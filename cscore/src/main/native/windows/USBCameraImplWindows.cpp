
#include "cscore_cpp.h"
#include "c_util.h"

#include "UsbCameraImplWindows.h"
#include "PropertyImpl.h"
#include <memory>

#include "Log.h"
#include <math.h>

#include "WindowsMessagePump.h"
#include <ComPtr.h>

#include <shlwapi.h>
#include <Windows.h>
#include <windowsx.h>
#include <mfidl.h>
#include <mfapi.h>

#include <wpi/SmallString.h>
#include <wpi/memory.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "JpegUtil.h"
#include "Log.h"
#include "Notifier.h"
#include "Telemetry.h"
#include "UsbUtil.h"
#include "c_util.h"
#include "cscore_cpp.h"

#include "COMCreators.h"

#include <iostream>

#include <codecvt>

#include <vector>
#include <string>

#include <mfidl.h>
#include <mfapi.h>
#include <Dbt.h>
#include <ks.h>
#include <ksmedia.h>

#include "GUIDName.h"

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "Shlwapi.lib")

const int NewImageMessage = 0x0400 + 4488;
const int SetCameraMessage = 0x0400 + 254;

using namespace cs;

namespace cs {

UsbCameraImplWindows::UsbCameraImplWindows(const wpi::Twine& name, const wpi::Twine& path)
    : SourceImpl{name},
      m_path{path.str()} {
        //std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  m_widePath = utf8_conv.from_bytes(m_path.c_str());
}

UsbCameraImplWindows::UsbCameraImplWindows(const wpi::Twine& name, int deviceId)
    : SourceImpl{name},
      m_deviceId(deviceId) {

}

UsbCameraImplWindows::~UsbCameraImplWindows() {
  //m_active = false;
  std::cout << "Starting Destructor\n";

  m_messagePump = nullptr;
  std::cout << "Destructed loop\n";
}

void UsbCameraImplWindows::SetProperty(int property, int value, CS_Status* status) {}
void UsbCameraImplWindows::SetStringProperty(int property, const wpi::Twine& value,
                        CS_Status* status) {}

// Standard common camera properties
void UsbCameraImplWindows::SetBrightness(int brightness, CS_Status* status) {

}
int UsbCameraImplWindows::GetBrightness(CS_Status* status) const { return 0; }
void UsbCameraImplWindows::SetWhiteBalanceAuto(CS_Status* status) {}
void UsbCameraImplWindows::SetWhiteBalanceHoldCurrent(CS_Status* status) {}
void UsbCameraImplWindows::SetWhiteBalanceManual(int value, CS_Status* status) {}
void UsbCameraImplWindows::SetExposureAuto(CS_Status* status) {}
void UsbCameraImplWindows::SetExposureHoldCurrent(CS_Status* status) {}
void UsbCameraImplWindows::SetExposureManual(int value, CS_Status* status) {}

bool UsbCameraImplWindows::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  Message msg{Message::kCmdSetMode};
  msg.data[0] = mode.pixelFormat;
  msg.data[1] = mode.width;
  msg.data[2] = mode.height;
  msg.data[3] = mode.fps;
  auto result = SendMessage(m_messagePump->hwnd, SetCameraMessage, NULL, reinterpret_cast<LPARAM>(&msg));
  std::cout << result << std::endl;
  return true;
 }
bool UsbCameraImplWindows::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                    CS_Status* status) {return true;}
bool UsbCameraImplWindows::SetResolution(int width, int height, CS_Status* status) { return true; }
bool UsbCameraImplWindows::SetFPS(int fps, CS_Status* status) { return true; }

void UsbCameraImplWindows::NumSinksChanged() {}
void UsbCameraImplWindows::NumSinksEnabledChanged() {}

void UsbCameraImplWindows::Start() {
  // Kick off the message pump

  m_messagePump = std::make_unique<WindowsMessagePump>([this](HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam){
    return this->PumpMain(hwnd, uiMsg, wParam, lParam);
  });
}

bool UsbCameraImplWindows::CheckDeviceChange(WPARAM wParam, DEV_BROADCAST_HDR *pHdr, bool* connected) {
    DEV_BROADCAST_DEVICEINTERFACE *pDi = NULL;

    *connected = false;

    if (pHdr == NULL)
    {
        return false;
    }
    if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
    {
        return false;
    }

    // Compare the device name with the symbolic link.

    pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;

    std::cout << "Device change check\n";

    if (_stricmp(m_path.c_str(), pDi->dbcc_name) == 0)
    {
      std::cout << "Strcmp true\n";
        if (wParam == DBT_DEVICEARRIVAL) {
          *connected = true;
          std::cout <<"rettrue\n";
          return true;
        } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
          *connected = false;
          std::cout <<"rettrue\n";
          return true;
        }
    }
    return false;
}

void UsbCameraImplWindows::DeviceDisconnect() {
  if (m_connectVerbose) SINFO("Disconnected from " << m_path);
  SafeRelease(&m_sourceReader);
  SafeRelease(&m_mediaSource);
  SetConnected(false);
}

void UsbCameraImplWindows::ProcessFrame(_ComPtr<IMFSample>& videoSample) {


    do {
        if (!videoSample)
            break;

        _ComPtr<IMFMediaBuffer> buf = NULL;

        if (!SUCCEEDED(videoSample->ConvertToContiguousBuffer(&buf)))
        {
            DWORD bcnt = 0;
            if (!SUCCEEDED(videoSample->GetBufferCount(&bcnt)))
                break;
            if (bcnt == 0)
                break;
            if (!SUCCEEDED(videoSample->GetBufferByIndex(0, &buf)))
                break;
        }

        bool lock2d = false;
        BYTE* ptr = NULL;
        LONG pitch = 0;
        DWORD maxsize = 0, cursize = 0;

        // "For 2-D buffers, the Lock2D method is more efficient than the Lock method"
        // see IMFMediaBuffer::Lock method documentation: https://msdn.microsoft.com/en-us/library/windows/desktop/bb970366(v=vs.85).aspx
        _ComPtr<IMF2DBuffer> buffer2d;
        if (true)
        {
            if (SUCCEEDED(buf.As<IMF2DBuffer>(buffer2d)))
            {
                if (SUCCEEDED(buffer2d->Lock2D(&ptr, &pitch)))
                {
                    lock2d = true;
                }
            }
        }
        if (ptr == NULL)
        {
            if (!SUCCEEDED(buf->Lock(&ptr, &maxsize, &cursize)))
            {
                break;
            }
        }
        if (!ptr)
            break;

        cv::Mat tmpMat;

        switch (m_mode.pixelFormat) {
          case cs::VideoMode::PixelFormat::kMJPEG:
            // cv::Mat(m_height, m_width, CV_8UC3, ptr, pitch).copyTo(tmpMat);
            //Special case
            //PutFrame(m_pixelFormat, m_width, m_height, )
            goto end;
            break;
          case cs::VideoMode::PixelFormat::kGray:
            cv::Mat(m_mode.height, m_mode.width, CV_8UC1, ptr, pitch).copyTo(tmpMat);
            break;
          case cs::VideoMode::PixelFormat::kBGR:
            cv::Mat(m_mode.height, m_mode.width, CV_8UC3, ptr, pitch).copyTo(tmpMat);
            break;
          case cs::VideoMode::PixelFormat::kYUYV:
            cv::Mat(m_mode.height, m_mode.width, CV_8UC2, ptr, pitch).copyTo(tmpMat);
            break;
          default:
            std::cout << "default case\n";
            goto end;
            break;
        }

        //cv::Mat(m_height, m_width, CV_8UC2, ptr, pitch).copyTo(tmpMat);
        //cv::Mat inputMat(1, cursize, CV_8UC1, ptr, pitch);
        std::cout << "Putting frame in code\n";
        PutFrame(static_cast<VideoMode::PixelFormat>(m_mode.pixelFormat), m_mode.width, m_mode.height, wpi::StringRef((char*)tmpMat.data, (size_t)(tmpMat.dataend - tmpMat.datastart)), wpi::Now());

        end:

        if (lock2d)
            buffer2d->Unlock2D();
        else
            buf->Unlock();
    }
    while (0);



}

LRESULT UsbCameraImplWindows::PumpMain(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
  switch (uiMsg) {
    case WM_CLOSE:
      SafeRelease(&m_sourceReader);
      SafeRelease(&m_mediaSource);
      SafeRelease(&m_imageCallback);
      break;
    case WM_CREATE:
      // Pump Created and ready to go
      m_imageCallback = CreateSourceReaderCB(hwnd);
      DeviceConnect();
      break;
    case WM_DEVICECHANGE: {
        // Device potentially changed
        PDEV_BROADCAST_HDR parameter = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
        // Check if we're waiting on a device path, and this is a connection
        if (m_path.empty() && wParam == DBT_DEVICEARRIVAL &&
            parameter->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
          // If path is empty, we attempted to connect with a device ID. Enumerate and check
          CS_Status status = 0;
          auto devices = cs::EnumerateUsbCameras(&status);
          if (devices.size() > m_deviceId) {
            // If has device ID, use the device ID from the event
            // because of windows bug
            auto&& device = devices[m_deviceId];
            DEV_BROADCAST_DEVICEINTERFACE *pDi = (DEV_BROADCAST_DEVICEINTERFACE*)parameter;
            m_path = pDi->dbcc_name;
            std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
            m_widePath = utf8_conv.from_bytes(m_path.c_str());
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
      }
      break;
    case NewImageMessage: {// New image
            _ComPtr<IMFSample> videoSample = m_imageCallback->GetLatestSample();
            m_sourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                0, NULL, NULL, NULL, NULL);
            ProcessFrame(videoSample);
        break;
        }
    case SetCameraMessage: {
      {
        Message* msg = reinterpret_cast<Message*>(lParam);
        std::unique_lock<wpi::mutex> lock(m_mutex);
        return DeviceProcessCommand(lock, *msg);
      }
      break;
    }
  }
  return 0l;
}

static std::string guidToString(GUID guid) {
    std::array<char,40> output;
    snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(output.data());
}

static cs::VideoMode::PixelFormat GetFromGUID(const GUID& guid) {
  wpi::StringRef guidRef = cs::GetGUIDNameConstNew(guid);
  // Compare GUID to one of the supported ones
  if (guidRef.equals("MFVideoFormat_NV12")) {
    // GrayScale
    return cs::VideoMode::PixelFormat::kGray;
  } else if (guidRef.equals("MFVideoFormat_YUY2")) {
    return cs::VideoMode::PixelFormat::kYUYV;
  } else if (guidRef.equals("MFVideoFormat_RGB24")) {
    return cs::VideoMode::PixelFormat::kBGR;
  } else if (guidRef.equals("MFVideoFormat_MJPG")) {
    return cs::VideoMode::PixelFormat::kMJPEG;
  } else {
    std::cout << guidRef << std::endl;
    return cs::VideoMode::PixelFormat::kUnknown;
  }
}

bool UsbCameraImplWindows::DeviceConnect() {
  if (m_mediaSource && m_sourceReader) return true;

  if (m_connectVerbose) SINFO("Connecting to USB camera on " << m_path);

  SDEBUG3("opening device");

  const wchar_t* path = m_widePath.c_str();
  m_mediaSource = CreateVideoCaptureDevice(path);


  if (!m_mediaSource) return false;

  m_sourceReader = CreateSourceReader(m_mediaSource, m_imageCallback);

  if (!m_sourceReader) {
    SafeRelease(&m_mediaSource);
    return false;
  }

  if (!m_properties_cached) {
    SDEBUG3("caching properties");
    //DeviceCacheProperties();
    DeviceCacheVideoModes();
    DeviceCacheMode();
    m_properties_cached = true;
  } else {
    SDEBUG3("restoring video mode");
    DeviceSetMode();
  }

  m_sourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0, NULL, NULL, NULL, NULL);

  SetConnected(true);
  return true;
}

std::unique_ptr<PropertyImpl> UsbCameraImplWindows::CreateEmptyProperty(
      const wpi::Twine& name) const {
  return nullptr;
}

bool UsbCameraImplWindows::CacheProperties(CS_Status* status) const {
  return true;
}

CS_StatusValue UsbCameraImplWindows::DeviceProcessCommand(
    std::unique_lock<wpi::mutex>& lock, const Message& msg) {
  if (msg.kind == Message::kCmdSetMode ||
      msg.kind == Message::kCmdSetPixelFormat ||
      msg.kind == Message::kCmdSetResolution ||
      msg.kind == Message::kCmdSetFPS) {
    return DeviceCmdSetMode(lock, msg);
  } else if (msg.kind == Message::kCmdSetProperty ||
             msg.kind == Message::kCmdSetPropertyStr) {
    //return DeviceCmdSetProperty(lock, msg);
    return CS_OK;
  } else if (msg.kind == Message::kNumSinksChanged ||
             msg.kind == Message::kNumSinksEnabledChanged) {
    return CS_OK;
  } else {
    return CS_OK;
  }
}

CS_StatusValue UsbCameraImplWindows::DeviceCmdSetMode(
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
      newMode.width != m_mode.width || newMode.height != m_mode.height ||
      newMode.fps != m_mode.fps) {
      m_mode = newMode;
      lock.unlock();
      bool wasStreaming = true;
      //if (wasStreaming) DeviceStreamOff();
      if (m_sourceReader) {
        DeviceDisconnect();
        DeviceConnect();
      }
      //if (wasStreaming) DeviceStreamOn();
      Notifier::GetInstance().NotifySourceVideoMode(*this, newMode);
      lock.lock();
  }

  return CS_OK;
}

void UsbCameraImplWindows::DeviceCacheMode() {
  if (!m_sourceReader) return;

  IMFMediaType* nativeType = NULL;
  // Get the default media type of the camera
  m_sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, &nativeType);

  GUID nativeGuid = {0};
  nativeType->GetGUID(MF_MT_SUBTYPE, &nativeGuid);
  wpi::StringRef guidRef = cs::GetGUIDNameConstNew(nativeGuid);

  VideoMode::PixelFormat pixelFormat;

  // Compare GUID to one of the supported ones
  if (guidRef.equals("MFVideoFormat_NV12")) {
    // GrayScale
    pixelFormat = cs::VideoMode::PixelFormat::kGray;
    std::cout << "NV12\n";
  } else if (guidRef.equals("MFVideoFormat_YUY2")) {
    pixelFormat = cs::VideoMode::PixelFormat::kYUYV;
    std::cout << "YUY2\n";
  } else if (guidRef.equals("MFVideoFormat_RGB24")) {
    pixelFormat = cs::VideoMode::PixelFormat::kBGR;
    std::cout << "RGB24\n";
  } else if (guidRef.equals("MFVideoFormat_MJPG")) {
    pixelFormat = cs::VideoMode::PixelFormat::kMJPEG;
    std::cout << "MJEPG\n";
  } else {
    // TODO: Search and find the first available GUID
    pixelFormat = cs::VideoMode::PixelFormat::kUnknown;
    std::cout << "Unknown\n";
  }

  UINT32 width, height;
  ::MFGetAttributeSize(nativeType, MF_MT_FRAME_SIZE, &width, &height);

  // Get FPS
  UINT32 num, dom;
  ::MFGetAttributeRatio(nativeType, MF_MT_FRAME_RATE, &num, &dom);

  int fps = 30;

  if (dom != 0) {
    fps = ceil(num / (double)dom);
  }

  bool formatChanged = false;

  if (m_modeSetPixelFormat) {
    if (pixelFormat != m_mode.pixelFormat) {
      formatChanged = true;
      pixelFormat = static_cast<VideoMode::PixelFormat>(m_mode.pixelFormat);
    }
  }
  // TODO default to something sane

  if (m_modeSetResolution) {
    // User set resolution
    if (width != m_mode.width || height != m_mode.height) {
      formatChanged = true;
      width = m_mode.width;
      height = m_mode.height;
    }
  }

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

  if (formatChanged || fpsChanged) DeviceSetMode();

  Notifier::GetInstance().NotifySourceVideoMode(*this, m_mode);
}

void UsbCameraImplWindows::DeviceSetMode() {
  _ComPtr<IMFMediaType> mediaTypeOut;

  IMFMediaType* nativeType = NULL;
  // Get the default media type of the camera
  m_sourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &nativeType);

  switch (m_mode.pixelFormat) {
    case cs::VideoMode::PixelFormat::kGray:
      nativeType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
      break;
    case cs::VideoMode::PixelFormat::kMJPEG:
      nativeType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_MJPG);
      break;
    case cs::VideoMode::PixelFormat::kBGR:
      nativeType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24);
      break;
    case cs::VideoMode::PixelFormat::kYUYV:
      nativeType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YUY2);
      break;
  }

  ::MFSetAttributeSize(nativeType, MF_MT_FRAME_SIZE, m_mode.width, m_mode.height);
  ::MFSetAttributeRatio(nativeType, MF_MT_FRAME_RATE, m_mode.fps, 1);

  m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, nativeType);


}

void UsbCameraImplWindows::DeviceCacheVideoModes() {
  if (!m_sourceReader) return;

  std::vector<VideoMode> modes;


    IMFMediaType* nativeType = NULL;
    int count = 0;
    while (true) {
        auto hr = m_sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, count, &nativeType);
        if (FAILED(hr)) {
          break;
        }
          GUID guid = {0};
          nativeType->GetGUID(MF_MT_SUBTYPE, &guid);

        //auto name = cs::GetGUIDNameConstNew(guid);
        auto format = GetFromGUID(guid);
        UINT32 width, height;
        ::MFGetAttributeSize(nativeType, MF_MT_FRAME_SIZE, &width, &height);

        UINT32 num, dom;
        ::MFGetAttributeRatio(nativeType, MF_MT_FRAME_RATE, &num, &dom);

        int fps = 30;

        if (dom != 0) {
          fps = ceil(num / (double)dom);
        }

        modes.emplace_back(format, static_cast<int>(width), static_cast<int>(height), fps);

        nativeType->Release();
        count++;
    }

    {
      std::lock_guard<wpi::mutex> lock(m_mutex);
      m_videoModes.swap(modes);
    }
    Notifier::GetInstance().NotifySource(*this, CS_SOURCE_VIDEOMODES_UPDATED);

}


std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  std::vector<UsbCameraInfo> retval;

  // Ensure we are initialized by grabbing the message pump
  //GetMessagePump();


  IMFMediaSource* ppSource = NULL;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  IMFMediaSource *pSource = NULL;
  IMFAttributes *pAttributes = NULL;
  IMFActivate **ppDevices = NULL;

  // Create an attribute store to specify the enumeration parameters.
  HRESULT hr = MFCreateAttributes(&pAttributes, 1);
  if (FAILED(hr))
  {
    goto done;
  }

  // Source type: video capture devices
  hr = pAttributes->SetGUID(
    MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
    MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
  );
  if (FAILED(hr))
  {
    goto done;
  }

  // Enumerate devices.
  UINT32 count;
  hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
  if (FAILED(hr))
  {
    goto done;
  }

  if (count == 0)
  {
    hr = E_FAIL;
    goto done;
  }

  for (UINT32 i = 0; i < count; i++) {
    UsbCameraInfo info;
    info.dev = i;
    WCHAR buf[512];
    ppDevices[i]->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, buf, sizeof(buf), NULL);
    info.name = utf8_conv.to_bytes(buf);
    ppDevices[i]->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, buf, sizeof(buf), NULL);
    info.path = utf8_conv.to_bytes(buf);
    retval.emplace_back(std::move(info));
  }

done:
  SafeRelease(&pAttributes);

  for (DWORD i = 0; i < count; i++)
  {
    SafeRelease(&ppDevices[i]);
  }
  CoTaskMemFree(ppDevices);
  SafeRelease(&pSource);
  return retval;
}


CS_Source CreateUsbCameraDev(const wpi::Twine& name, int dev,
                             CS_Status* status) {
  // First check if device exists
  auto devices = cs::EnumerateUsbCameras(status);
  if (devices.size() > dev) {
    return CreateUsbCameraPath(name, devices[dev].path, status);
  }
  auto source = std::make_shared<UsbCameraImplWindows>(name, dev);
  auto handle = Sources::GetInstance().Allocate(CS_SOURCE_USB, source);
  Notifier::GetInstance().NotifySource(name, handle, CS_SOURCE_CREATED);
  // Start thread after the source created event to ensure other events
  // come after it.
  source->Start();
  return handle;
}

CS_Source CreateUsbCameraPath(const wpi::Twine& name, const wpi::Twine& path,
                              CS_Status* status) {
  auto source = std::make_shared<UsbCameraImplWindows>(name, path);
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
  return static_cast<UsbCameraImplWindows&>(*data->source).GetPath();
}

}

extern "C" {

CS_UsbCameraInfo* CS_EnumerateUsbCameras(int* count, CS_Status* status) {
  auto cameras = cs::EnumerateUsbCameras(status);
  CS_UsbCameraInfo* out = static_cast<CS_UsbCameraInfo*>(
      wpi::CheckedMalloc(cameras.size() * sizeof(CS_UsbCameraInfo)));
  *count = cameras.size();
  for (size_t i = 0; i < cameras.size(); ++i) {
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

}
