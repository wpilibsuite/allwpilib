
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
  std::cout << "Posting Message\n";
  SendMessage(m_messagePump->hwnd, SetCameraMessage, NULL, reinterpret_cast<LPARAM>(&msg));
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
    this->PumpMain(hwnd, uiMsg, wParam, lParam);
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

void UsbCameraImplWindows::TryConnectCamera() {
  DeviceConnect();
}

void UsbCameraImplWindows::DisconnectCamera() {
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

        switch (m_pixelFormat) {
          case cs::VideoMode::PixelFormat::kMJPEG:
            // cv::Mat(m_height, m_width, CV_8UC3, ptr, pitch).copyTo(tmpMat);
            //Special case
            //PutFrame(m_pixelFormat, m_width, m_height, )
            goto end;
            break;
          case cs::VideoMode::PixelFormat::kGray:
            cv::Mat(m_height, m_width, CV_8UC1, ptr, pitch).copyTo(tmpMat);
            break;
          case cs::VideoMode::PixelFormat::kBGR:
            cv::Mat(m_height, m_width, CV_8UC3, ptr, pitch).copyTo(tmpMat);
            break;
          case cs::VideoMode::PixelFormat::kYUYV:
            cv::Mat(m_height, m_width, CV_8UC2, ptr, pitch).copyTo(tmpMat);
            break;
          default:
            std::cout << "default case\n";
            goto end;
            break;
        }

        //cv::Mat(m_height, m_width, CV_8UC2, ptr, pitch).copyTo(tmpMat);
        //cv::Mat inputMat(1, cursize, CV_8UC1, ptr, pitch);
        std::cout << "Putting frame in code\n";
        PutFrame(m_pixelFormat, m_width, m_height, wpi::StringRef((char*)tmpMat.data, (size_t)(tmpMat.dataend - tmpMat.datastart)), wpi::Now());

        end:

        if (lock2d)
            buffer2d->Unlock2D();
        else
            buf->Unlock();
    }
    while (0);



}

void UsbCameraImplWindows::PumpMain(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
  switch (uiMsg) {
    case WM_CLOSE:
      SafeRelease(&m_sourceReader);
      SafeRelease(&m_mediaSource);
      SafeRelease(&m_imageCallback);
      break;
    case WM_CREATE:
      // Pump Created and ready to go
      m_imageCallback = CreateSourceReaderCB(hwnd);
      TryConnectCamera();
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
            TryConnectCamera();
          } else {
            // Disconnected
            DisconnectCamera();
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
      std::cout << "Set Camera Message" << std::endl;
      break;
    }
    case WM_TIMER:
        // Reconnect timer
        TryConnectCamera();
      break;
  }
}

void UsbCameraImplWindows::CameraThreadMain() {
  // Register to respond to Device change events

  bool wasStreaming = false;
  m_streaming = false;

  // while (m_active) {
  //   // Device Connect
  //   if (!m_mediaSource) DeviceConnect();
  // }
  // std::cout << "Thread Died" << std::endl;
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

  m_width = 0;
  m_height = 0;

  // Enumerate Known Modes
  DeviceCacheVideoModes();

  IMFMediaType* nativeType = NULL;
  // Get the default media type of the camera
  m_sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, &nativeType);

  GUID nativeGuid = {0};
  nativeType->GetGUID(MF_MT_SUBTYPE, &nativeGuid);
  wpi::StringRef guidRef = cs::GetGUIDNameConstNew(nativeGuid);

  // Compare GUID to one of the supported ones
  if (guidRef.equals("MFVideoFormat_NV12")) {
    // GrayScale
    m_pixelFormat = cs::VideoMode::PixelFormat::kGray;
    std::cout << "NV12\n";
  } else if (guidRef.equals("MFVideoFormat_YUY2")) {
    m_pixelFormat = cs::VideoMode::PixelFormat::kYUYV;
    std::cout << "YUY2\n";
  } else if (guidRef.equals("MFVideoFormat_RGB24")) {
    m_pixelFormat = cs::VideoMode::PixelFormat::kBGR;
    std::cout << "RGB24\n";
  } else if (guidRef.equals("MFVideoFormat_MJPG")) {
    m_pixelFormat = cs::VideoMode::PixelFormat::kMJPEG;
    std::cout << "MJEPG\n";
  } else {
    // TODO: Search and find the first available GUID
    std::cout << "Unknown\n";
  }

    //GUID subtype;

    std::cout << cs::GetGUIDNameConstNew(nativeGuid) << std::endl;

    //subtype = MFVideoFormat_RGB24;
    //subtype = MFVideoFormat_YUY2;

    //nativeType->SetGUID(MF_MT_SUBTYPE, subtype);

    //std::cout << guidToString(nativeGuid) << "\n";

  UINT32 width, height;
  ::MFGetAttributeSize(nativeType, MF_MT_FRAME_SIZE, &width, &height);

  m_width = width;
  m_height = height;

  std::cout << "width: " << width << " height: " << height << "\n";

  std::cout << (int)m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, nativeType) << "\n";

  //std::cout << (int)ConfigureDecoder(m_sourceReader, MF_SOURCE_READER_FIRST_VIDEO_STREAM) << "\n";

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

void UsbCameraImplWindows::DeviceCacheVideoModes() {
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
