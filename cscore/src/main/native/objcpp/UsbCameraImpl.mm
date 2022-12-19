#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include <iostream>

#include <vector>
#include <string>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Log.h"
#include "Notifier.h"
#include "c_util.h"
#include "cscore_cpp.h"
#include "opencv2/imgproc.hpp"
#include "UsbCameraImpl.h"

using namespace cs;

@interface UsbCameraDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    @public
    UsbCameraImpl* m_source;
    //IplImage* m_inputImageHeader;
    //IplImage* m_outputImageHeader;
}
- (void) captureOutput: ( AVCaptureOutput* ) captureOutput didOutputSampleBuffer: ( CMSampleBufferRef )sampleBuffer fromConnection: (AVCaptureConnection*) connection;

@end
@implementation UsbCameraDelegate

- (id)init {
    self = [super init];
    m_inputImageHeader = cvCreateImageHeader(cvSize(1280, 960), IPL_DEPTH_8U, 4);
    m_outputImageHeader = cvCreateImageHeader(cvSize(1280, 960), IPL_DEPTH_8U, 3);
    return self;
}

- (void)dealloc {
    cvReleaseImage(&m_outputImageHeader);
    cvReleaseImage(&m_inputImageHeader);
}

- (void) captureOutput: ( AVCaptureOutput* ) captureOutput didOutputSampleBuffer: ( CMSampleBufferRef )sampleBuffer fromConnection: (AVCaptureConnection*) connection {
    (void)captureOutput;
    (void)sampleBuffer;
    (void)connection;

    //std::cout << "Got Buffer" << std::endl;

    //return;

    // Convert buffer to BGR
    // Assume it comes in as 32BGRA

    auto imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    CVPixelBufferLockBaseAddress(imageBuffer, 0);

    void* baseaddress = CVPixelBufferGetBaseAddress(imageBuffer);

    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    size_t rowBytes = CVPixelBufferGetBytesPerRow(imageBuffer);
    OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);
    //std::cout << "Row bytes " <<rowBytes << std::endl;

    if (rowBytes == 0 ) {
        //TODO: Print error
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return;
    }
    auto source = m_source;
    //auto source = m_source.lock();
    if (!source) {
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return;
    }

    //std::cout << width << " " << height << std::endl;

    m_outputImageHeader->width = (int)width;
    m_outputImageHeader->height = (int)height;
    m_outputImageHeader->nChannels = 3;
    m_outputImageHeader->depth = IPL_DEPTH_8U;
    m_outputImageHeader->widthStep = (int)(width * 3);

    size_t currSize = width * 3 * height;

    m_outputImageHeader->imageSize = (int)currSize;

    auto image = source->AllocImage(cs::VideoMode::PixelFormat::kBGR, width, height, currSize);
    m_outputImageHeader->imageData = image->data();

    if ( pixelFormat == kCVPixelFormatType_32BGRA ) {
        m_inputImageHeader->width = int(width);
        m_inputImageHeader->height = int(height);
        m_inputImageHeader->nChannels = 4;
        m_inputImageHeader->depth = IPL_DEPTH_8U;
        m_inputImageHeader->widthStep = int(rowBytes);
        m_inputImageHeader->imageData = reinterpret_cast<char *>(baseaddress);
        m_inputImageHeader->imageSize = int(rowBytes*height);
//std::cout << " Cvt Color BGRA" << std::endl;
        cvCvtColor(m_inputImageHeader, m_outputImageHeader, CV_BGRA2BGR);
    } else if ( pixelFormat == kCVPixelFormatType_422YpCbCr8 ) {
        m_inputImageHeader->width = int(width);
        m_inputImageHeader->height = int(height);
        m_inputImageHeader->nChannels = 2;
        m_inputImageHeader->depth = IPL_DEPTH_8U;
        m_inputImageHeader->widthStep = int(rowBytes);
        m_inputImageHeader->imageData = reinterpret_cast<char *>(baseaddress);
        m_inputImageHeader->imageSize = int(rowBytes*height);

        //std::cout << " Cvt Color YUV" << std::endl;
        cvCvtColor(m_inputImageHeader, m_outputImageHeader, CV_YUV2BGR_UYVY);
    } else {
        // TODO: Error
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return;
    }
    std::cout << "put frame" <<std::endl;
    source->PutFrameInternal(std::move(image), wpi::Now());

    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}

@end

static constexpr const char* DeviceNotConnectedString = "DEVICENOTCONNECTEDATSTARTUP";


namespace cs {

    class ObjCData {
  public:
    AVCaptureDevice* device;
    AVCaptureDeviceInput* input;
    AVCaptureVideoDataOutput* output;

    AVCaptureSession* session;
    UsbCameraDelegate* delegate;
};

    void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {}
  void UsbCameraImpl::SetStringProperty(int property, llvm::StringRef value,
                         CS_Status* status) {}

  void UsbCameraImpl::SetBrightness(int brightness, CS_Status* status) {}
  int UsbCameraImpl::GetBrightness(CS_Status* status) const {return 0;}
  void UsbCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {}
  void UsbCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {}
  void UsbCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {}
  void UsbCameraImpl::SetExposureAuto(CS_Status* status) {}
  void UsbCameraImpl::SetExposureHoldCurrent(CS_Status* status) {}
  void UsbCameraImpl::SetExposureManual(int value, CS_Status* status) {}

  bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {return true;}
  bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                      CS_Status* status) {return true;}
  bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {return true;}
  bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {return true;}

  void UsbCameraImpl::NumSinksChanged() {}
  void UsbCameraImpl::NumSinksEnabledChanged(){}

   std::unique_ptr<PropertyImpl> UsbCameraImpl::CreateEmptyProperty(
      llvm::StringRef name) const {return nullptr;}

  // Cache properties.  Immediately successful if properties are already cached.
  // If they are not, tries to connect to the camera to do so; returns false and
  // sets status to CS_SOURCE_IS_DISCONNECTED if that too fails.
  bool UsbCameraImpl::CacheProperties(CS_Status* status) const {return true;}

UsbCameraImpl::UsbCameraImpl(llvm::StringRef name, llvm::StringRef path)
        : SourceImpl{name}, m_path{path}, m_active{true} {

}

UsbCameraImpl::~UsbCameraImpl() {
    m_active = false;

    m_responseCv.notify_all();

    m_cameraCv.notify_all();

    if (m_cameraThread.joinable()) m_cameraThread.join();


}

void UsbCameraImpl::Start() {
    m_cameraThread = std::thread(&UsbCameraImpl::CameraThreadMain, this);
}

void UsbCameraImpl::CameraThreadMain() {

    ObjCData objcData{};
    /*

    objcData.session = [[AVCaptureSession alloc] init];

    objcData.delegate = [[UsbCameraDelegate alloc] init];

    objcData.output = [[AVCaptureVideoDataOutput alloc] init];

    objcData.delegate->m_source = this;

    auto queue = dispatch_queue_create("captureQueue", DISPATCH_QUEUE_SERIAL);

    [ objcData.output setSampleBufferDelegate: objcData.delegate queue:queue];

    OSType pixelFormat = kCVPixelFormatType_32BGRA;

    NSDictionary* pixelBufferOptions = @{
        (id)kCVPixelBufferPixelFormatTypeKey: @(pixelFormat)
    };

    objcData.output.videoSettings = pixelBufferOptions;

    [objcData.session addOutput: objcData.output];
    */


    while (m_active) {
        if (!objcData.device) {
            DeviceConnect(&objcData);
        }

        auto timeoutTime =
            std::chrono::steady_clock::now() + std::chrono::duration<double>(1.0);
        {
            std::unique_lock<std::mutex> dataLock(m_cameraMutex);
            m_cameraCv.wait_until(dataLock, timeoutTime);
        }
        auto now = wpi::Now();
        if (objcData.device && m_lastCameraUpdateTime + 10000000 < now) {
        // Reset macData to cause a reconnect
            DeviceDisconnect(&objcData);
        }

    }

}

void UsbCameraImpl::DeviceDisconnect(ObjCData* objcData) {
    std::cout << "running disconnect" << std::endl;
    // On disconnect, stop session, and null out AVCaptureDevice
    [objcData->session stopRunning];
    [objcData->session removeInput:objcData->input];
    objcData->device = nullptr;
    objcData->input = nullptr;
    SetConnected(false);
}

void UsbCameraImpl::DeviceConnect(ObjCData* objcData) {
    // First check if path is a full path, or our dev detection string
    llvm::StringRef path{m_path};

    std::cout << "Path: " << m_path << std::endl;

    AVCaptureDevice* device = nullptr;

    if (path.startswith(DeviceNotConnectedString)) {
        //We need to find our device based on an index, not an exact path
        int index = path[strlen(DeviceNotConnectedString)] - '0';
        std::cout <<  "Index: " << index << std::endl;
        NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

        std::cout << "Count: " << [devices count] << std::endl;

        int count = 0;
        for (id dev in devices) {
            NSString* name = [dev localizedName];
            std::cout << "Name: " << [name UTF8String] << std::endl;
            if (count == index) {
                device = dev;
                break;
            }
            count++;
        }
        if (!device) {
            return; // Did not find device at index
        }
    } else {
        NSString* idStr = [NSString stringWithCString:m_path.c_str() encoding:[NSString defaultCStringEncoding]];

        device = [AVCaptureDevice deviceWithUniqueID:idStr];

        if (!device) {
            return;
        }
    }

    NSError* error = nil;
    auto input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];

    // Found our device.
    objcData->input = input;
    objcData->device = device;
    [objcData->session addInput:objcData->input];
    {
        std::lock_guard<std::mutex> lock(m_cameraMutex);
        // Add 3 seconds to now to ensure device starts
        // TODO: Figure out notifications so I can programmatically do this.
        m_lastCameraUpdateTime = wpi::Now() + 30000000;
    }
    [objcData->session startRunning];
    SetConnected(true);
    std::cout << "connected" <<std::endl;
}


void UsbCameraImpl::PutFrameInternal(std::unique_ptr<Image> image, Frame::Time time) {
    PutFrame(std::move(image), time);
    std::lock_guard<std::mutex> lock(m_cameraMutex);
    m_lastCameraUpdateTime = time;
    m_cameraCv.notify_all();
}

CS_Source CreateUsbCameraDev(llvm::StringRef name, int dev, CS_Status* status) {
    NSArray* captureDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

    dev -= 1;


    std::cout << DeviceNotConnectedString << std::endl;
    std::string path = DeviceNotConnectedString;
    path = path + std::to_string(dev);
    std::cout << path << std::endl;

    return CreateUsbCameraPath(name, path, status);

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
    NSArray* captureDevices = [AVCaptureDevice devices];

    int count = 0;
    for (id device in captureDevices) {
        NSString* name = [device localizedName];
        NSString* uniqueIdentifier = [(AVCaptureDevice*)device uniqueID];
        retval.push_back({count, [name UTF8String], [uniqueIdentifier UTF8String]});

        count++;
    }

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

}  // extern "C"
