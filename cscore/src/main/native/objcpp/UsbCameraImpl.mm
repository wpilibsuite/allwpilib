#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include <iostream>

#include <vector>
#include <string>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Log.h"
#include "Notifier.h"
#include "Instance.h"
#include "c_util.h"
#include "cscore_cpp.h"
#include "opencv2/imgproc.hpp"
#include "UsbCameraImpl.h"

using namespace cs;


@interface UsbCameraDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
// {
//     @public
//     UsbCameraImpl* m_source;
//     //IplImage* m_inputImageHeader;
//     //IplImage* m_outputImageHeader;
// }
- (void) captureOutput: ( AVCaptureOutput* ) captureOutput didOutputSampleBuffer: ( CMSampleBufferRef )sampleBuffer fromConnection: (AVCaptureConnection*) connection;
@end

@implementation UsbCameraDelegate

- (id)init {
    self = [super init];
    return self;
}

- (void) captureOutput: ( AVCaptureOutput* ) captureOutput didOutputSampleBuffer: ( CMSampleBufferRef )sampleBuffer fromConnection: (AVCaptureConnection*) connection {
    (void)captureOutput;
    (void)sampleBuffer;
    (void)connection;

    NSLog(@"Got Capture");
}

@end

@interface UsbCameraImplObjc : NSObject

@property (nonatomic) dispatch_queue_t sessionQueue;
@property (nonatomic) NSString* path;
@property (nonatomic) int deviceId;

@property (nonatomic)  AVCaptureDevice* videoDevice;
@property (nonatomic)  AVCaptureDeviceInput* videoInput ;
 @property (nonatomic) UsbCameraDelegate* callback ;
 @property (nonatomic) AVCaptureVideoDataOutput* videoOutput;
 @property (nonatomic) AVCaptureSession* session;

- (void) startCapture;
- (void) captureConfiguration;
- (void) getCurrentPath: (std::string*) path;
- (void) getCameraName: (std::string*) name;
- (void) setNewPath: (std::string_view*) path;

@end

@implementation UsbCameraImplObjc

- (void) getCurrentPath: (std::string*) path {
  dispatch_async_and_wait(self.sessionQueue, ^{
      if (self.videoDevice == nil) {
      return;
    }
    *path = [self.videoDevice.uniqueID UTF8String];
  });
}

- (void) getCameraName: (std::string*) name
{
  dispatch_async_and_wait(self.sessionQueue, ^{
      if (self.videoDevice == nil) {
      return;
    }
    *name = [self.videoDevice.localizedName UTF8String];
  });
}

- (void) setNewPath: (std::string_view*) path
{
  dispatch_async_and_wait(self.sessionQueue, ^{
    NSString* nsPath = [[NSString alloc] initWithBytes:path->data() length: path->size() encoding: NSUTF8StringEncoding];
    if (self.path != nil && [self.path isEqualToString: nsPath]) {
      return;
    }
    self.session = nil;
    self.videoOutput = nil;
    self.callback = nil;
    self.videoInput = nil;
    self.videoDevice = nil;
    self.path = nsPath;
    [self captureConfiguration];
  });
}

- (id)init  {
  self = [super init];
  self.sessionQueue = dispatch_queue_create("session queue", DISPATCH_QUEUE_SERIAL);
  return self;
}

- (void) startCapture {
    switch([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]) {
      case AVAuthorizationStatusAuthorized:
        NSLog(@"Already Authorized");
        break;
      default:
        NSLog(@"Authorization Failed");
        // TODO log
        break;
      case AVAuthorizationStatusNotDetermined:
        dispatch_suspend(self.sessionQueue);
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
          (void)granted;
          dispatch_resume(self.sessionQueue);
        }];
        break;

    }
    dispatch_async(self.sessionQueue, ^{
      [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(cameraAdded:) name: AVCaptureDeviceWasConnectedNotification object:nil];
      [self captureConfiguration];
    });
}

- (void) sessionRuntimeError:(NSNotification*)notification
{
    NSError* error = notification.userInfo[AVCaptureSessionErrorKey];
    NSLog(@"Capture session runtime error: %@", error);
}

- (void) cameraAdded:(NSNotification*)notification
{
  AVCaptureDevice* device = notification.object;
    NSLog(@"Device Connected: %@", device.uniqueID);
    if (self.path == nil || [device.uniqueID isEqualToString:self.path]) {
      [self captureConfiguration];
    }
}

- (void) captureConfiguration {


  if (self.path == nil) {
    NSLog(@"Starting for device id %d", self.deviceId);
    // Enumerate Devices
    CS_Status status = 0;
    auto cameras = cs::EnumerateUsbCameras(&status);
    if (static_cast<int>(cameras.size()) <= self.deviceId) {
      NSLog(@"Path for index %d not found. Only %d exist", self.deviceId, static_cast<int>(cameras.size()));
      return;
    }
    std::string& path = cameras[self.deviceId].path;
    self.path = [[NSString alloc] initWithBytes:path.data() length: path.size() encoding: NSUTF8StringEncoding];
  }

  NSLog(@"Starting for path %@", self.path);

  self.videoDevice = [AVCaptureDevice deviceWithUniqueID:self.path];
  if (self.videoDevice == nil) {
    NSLog(@"Device Not found");
    goto err;
  }

  self.videoInput = [AVCaptureDeviceInput deviceInputWithDevice: self.videoDevice error: nil];
  if (self.videoInput == nil) {
    NSLog(@"Video Input Failed");
    goto err;
  }

  self.callback = [[UsbCameraDelegate alloc] init];
  if (self.callback == nil) {
    NSLog(@"Callback failed");
    goto err;
  }

  self.videoOutput = [[AVCaptureVideoDataOutput alloc] init];
  if (self.videoOutput == nil) {
    NSLog(@"Video Output Failed");
    goto err;
  }

  [self.videoOutput setSampleBufferDelegate: self.callback queue:self.sessionQueue];

  self.session = [[AVCaptureSession alloc] init];
  if (self.session == nil) {
    NSLog(@"Session failed");
    goto err;
  }

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionRuntimeError:) name:AVCaptureSessionRuntimeErrorNotification object:self.session];

  [self.session addInput: self.videoInput];
  [self.session addOutput: self.videoOutput];
  [self.session startRunning];
  NSLog(@"Starting Video %@", self.path);

  return;

err:
  self.session = nil;
  self.videoOutput = nil;
  self.callback = nil;
  self.videoInput = nil;
  self.videoDevice = nil;
}

@end

// @interface UsbCameraDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
// {
//     @public
//     UsbCameraImpl* m_source;
//     //IplImage* m_inputImageHeader;
//     //IplImage* m_outputImageHeader;
// }
// - (void) captureOutput: ( AVCaptureOutput* ) captureOutput didOutputSampleBuffer: ( CMSampleBufferRef )sampleBuffer fromConnection: (AVCaptureConnection*) connection;

// @end
// @implementation UsbCameraDelegate

// - (id)init {
//     self = [super init];
//     m_inputImageHeader = cvCreateImageHeader(cvSize(1280, 960), IPL_DEPTH_8U, 4);
//     m_outputImageHeader = cvCreateImageHeader(cvSize(1280, 960), IPL_DEPTH_8U, 3);
//     return self;
// }

// - (void)dealloc {
//     cvReleaseImage(&m_outputImageHeader);
//     cvReleaseImage(&m_inputImageHeader);
// }

// - (void) captureOutput: ( AVCaptureOutput* ) captureOutput didOutputSampleBuffer: ( CMSampleBufferRef )sampleBuffer fromConnection: (AVCaptureConnection*) connection {
//     (void)captureOutput;
//     (void)sampleBuffer;
//     (void)connection;

//     //std::cout << "Got Buffer" << std::endl;

//     //return;

//     // Convert buffer to BGR
//     // Assume it comes in as 32BGRA

//     auto imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

//     CVPixelBufferLockBaseAddress(imageBuffer, 0);

//     void* baseaddress = CVPixelBufferGetBaseAddress(imageBuffer);

//     size_t width = CVPixelBufferGetWidth(imageBuffer);
//     size_t height = CVPixelBufferGetHeight(imageBuffer);
//     size_t rowBytes = CVPixelBufferGetBytesPerRow(imageBuffer);
//     OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);
//     //std::cout << "Row bytes " <<rowBytes << std::endl;

//     if (rowBytes == 0 ) {
//         //TODO: Print error
//         CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
//         return;
//     }
//     auto source = m_source;
//     //auto source = m_source.lock();
//     if (!source) {
//         CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
//         return;
//     }

//     //std::cout << width << " " << height << std::endl;

//     m_outputImageHeader->width = (int)width;
//     m_outputImageHeader->height = (int)height;
//     m_outputImageHeader->nChannels = 3;
//     m_outputImageHeader->depth = IPL_DEPTH_8U;
//     m_outputImageHeader->widthStep = (int)(width * 3);

//     size_t currSize = width * 3 * height;

//     m_outputImageHeader->imageSize = (int)currSize;

//     auto image = source->AllocImage(cs::VideoMode::PixelFormat::kBGR, width, height, currSize);
//     m_outputImageHeader->imageData = image->data();

//     if ( pixelFormat == kCVPixelFormatType_32BGRA ) {
//         m_inputImageHeader->width = int(width);
//         m_inputImageHeader->height = int(height);
//         m_inputImageHeader->nChannels = 4;
//         m_inputImageHeader->depth = IPL_DEPTH_8U;
//         m_inputImageHeader->widthStep = int(rowBytes);
//         m_inputImageHeader->imageData = reinterpret_cast<char *>(baseaddress);
//         m_inputImageHeader->imageSize = int(rowBytes*height);
// //std::cout << " Cvt Color BGRA" << std::endl;
//         cvCvtColor(m_inputImageHeader, m_outputImageHeader, CV_BGRA2BGR);
//     } else if ( pixelFormat == kCVPixelFormatType_422YpCbCr8 ) {
//         m_inputImageHeader->width = int(width);
//         m_inputImageHeader->height = int(height);
//         m_inputImageHeader->nChannels = 2;
//         m_inputImageHeader->depth = IPL_DEPTH_8U;
//         m_inputImageHeader->widthStep = int(rowBytes);
//         m_inputImageHeader->imageData = reinterpret_cast<char *>(baseaddress);
//         m_inputImageHeader->imageSize = int(rowBytes*height);

//         //std::cout << " Cvt Color YUV" << std::endl;
//         cvCvtColor(m_inputImageHeader, m_outputImageHeader, CV_YUV2BGR_UYVY);
//     } else {
//         // TODO: Error
//         CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
//         return;
//     }
//     std::cout << "put frame" <<std::endl;
//     source->PutFrameInternal(std::move(image), wpi::Now());

//     CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
// }

// @end

// static constexpr const char* DeviceNotConnectedString = "DEVICENOTCONNECTEDATSTARTUP";




namespace cs {

  UsbCameraImpl::UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier, Telemetry& telemetry, std::string_view path)
    : SourceImpl{name, logger, notifier, telemetry} {
      UsbCameraImplObjc* objc = [[UsbCameraImplObjc alloc] init];
      objc.path = [[NSString alloc] initWithBytes:path.data() length: path.size() encoding: NSUTF8StringEncoding];
      m_objc = objc;
  }
    UsbCameraImpl::UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier, Telemetry& telemetry, int deviceId)
    : SourceImpl{name, logger, notifier, telemetry} {
      UsbCameraImplObjc* objc = [[UsbCameraImplObjc alloc] init];
      objc.path = nil;
      objc.deviceId = deviceId;
      m_objc = objc;
  }
  UsbCameraImpl::~UsbCameraImpl() {
  }

  void UsbCameraImpl::Start() {
    [m_objc startCapture];

    // switch([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]) {
    //   case AVAuthorizationStatusAuthorized:
    //     break;
    //   case AVAuthorizationStatusNotDetermined:
    //     dispatch_suspend(m_sessionQueue);
    //     [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
    //       (void)granted;
    //       dispatch_resume(m_sessionQueue);
    //     }];
    //     break;
    //   default:
    //     // TODO log
    //     break;
    // }
    // dispatch_async(m_sessionQueue, ^{

    // });
  }

//     class ObjCData {
//   public:
//     AVCaptureDevice* device;
//     AVCaptureDeviceInput* input;
//     AVCaptureVideoDataOutput* output;

//     AVCaptureSession* session;
//     UsbCameraDelegate* delegate;
// };

//     void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {}
//   void UsbCameraImpl::SetStringProperty(int property, llvm::StringRef value,
//                          CS_Status* status) {}

//   void UsbCameraImpl::SetBrightness(int brightness, CS_Status* status) {}
//   int UsbCameraImpl::GetBrightness(CS_Status* status) const {return 0;}
//   void UsbCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {}
//   void UsbCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {}
//   void UsbCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {}
//   void UsbCameraImpl::SetExposureAuto(CS_Status* status) {}
//   void UsbCameraImpl::SetExposureHoldCurrent(CS_Status* status) {}
//   void UsbCameraImpl::SetExposureManual(int value, CS_Status* status) {}

//   bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {return true;}
//   bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
//                       CS_Status* status) {return true;}
//   bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {return true;}
//   bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {return true;}

//   void UsbCameraImpl::NumSinksChanged() {}
//   void UsbCameraImpl::NumSinksEnabledChanged(){}

//    std::unique_ptr<PropertyImpl> UsbCameraImpl::CreateEmptyProperty(
//       llvm::StringRef name) const {return nullptr;}

//   // Cache properties.  Immediately successful if properties are already cached.
//   // If they are not, tries to connect to the camera to do so; returns false and
//   // sets status to CS_SOURCE_IS_DISCONNECTED if that too fails.
//   bool UsbCameraImpl::CacheProperties(CS_Status* status) const {return true;}

// UsbCameraImpl::UsbCameraImpl(llvm::StringRef name, llvm::StringRef path)
//         : SourceImpl{name}, m_path{path}, m_active{true} {

// }

// UsbCameraImpl::~UsbCameraImpl() {
//     m_active = false;

//     m_responseCv.notify_all();

//     m_cameraCv.notify_all();

//     if (m_cameraThread.joinable()) m_cameraThread.join();


// }

// void UsbCameraImpl::Start() {
//     m_cameraThread = std::thread(&UsbCameraImpl::CameraThreadMain, this);
// }

// void UsbCameraImpl::CameraThreadMain() {

//     ObjCData objcData{};
//     /*

//     objcData.session = [[AVCaptureSession alloc] init];

//     objcData.delegate = [[UsbCameraDelegate alloc] init];

//     objcData.output = [[AVCaptureVideoDataOutput alloc] init];

//     objcData.delegate->m_source = this;

//     auto queue = dispatch_queue_create("captureQueue", DISPATCH_QUEUE_SERIAL);

//     [ objcData.output setSampleBufferDelegate: objcData.delegate queue:queue];

//     OSType pixelFormat = kCVPixelFormatType_32BGRA;

//     NSDictionary* pixelBufferOptions = @{
//         (id)kCVPixelBufferPixelFormatTypeKey: @(pixelFormat)
//     };

//     objcData.output.videoSettings = pixelBufferOptions;

//     [objcData.session addOutput: objcData.output];
//     */


//     while (m_active) {
//         if (!objcData.device) {
//             DeviceConnect(&objcData);
//         }

//         auto timeoutTime =
//             std::chrono::steady_clock::now() + std::chrono::duration<double>(1.0);
//         {
//             std::unique_lock<std::mutex> dataLock(m_cameraMutex);
//             m_cameraCv.wait_until(dataLock, timeoutTime);
//         }
//         auto now = wpi::Now();
//         if (objcData.device && m_lastCameraUpdateTime + 10000000 < now) {
//         // Reset macData to cause a reconnect
//             DeviceDisconnect(&objcData);
//         }

//     }

// }

// void UsbCameraImpl::DeviceDisconnect(ObjCData* objcData) {
//     std::cout << "running disconnect" << std::endl;
//     // On disconnect, stop session, and null out AVCaptureDevice
//     [objcData->session stopRunning];
//     [objcData->session removeInput:objcData->input];
//     objcData->device = nullptr;
//     objcData->input = nullptr;
//     SetConnected(false);
// }

// void UsbCameraImpl::DeviceConnect(ObjCData* objcData) {
//     // First check if path is a full path, or our dev detection string
//     llvm::StringRef path{m_path};

//     std::cout << "Path: " << m_path << std::endl;

//     AVCaptureDevice* device = nullptr;

//     if (path.startswith(DeviceNotConnectedString)) {
//         //We need to find our device based on an index, not an exact path
//         int index = path[strlen(DeviceNotConnectedString)] - '0';
//         std::cout <<  "Index: " << index << std::endl;
//         NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

//         std::cout << "Count: " << [devices count] << std::endl;

//         int count = 0;
//         for (id dev in devices) {
//             NSString* name = [dev localizedName];
//             std::cout << "Name: " << [name UTF8String] << std::endl;
//             if (count == index) {
//                 device = dev;
//                 break;
//             }
//             count++;
//         }
//         if (!device) {
//             return; // Did not find device at index
//         }
//     } else {
//         NSString* idStr = [NSString stringWithCString:m_path.c_str() encoding:[NSString defaultCStringEncoding]];

//         device = [AVCaptureDevice deviceWithUniqueID:idStr];

//         if (!device) {
//             return;
//         }
//     }

//     NSError* error = nil;
//     auto input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];

//     // Found our device.
//     objcData->input = input;
//     objcData->device = device;
//     [objcData->session addInput:objcData->input];
//     {
//         std::lock_guard<std::mutex> lock(m_cameraMutex);
//         // Add 3 seconds to now to ensure device starts
//         // TODO: Figure out notifications so I can programmatically do this.
//         m_lastCameraUpdateTime = wpi::Now() + 30000000;
//     }
//     [objcData->session startRunning];
//     SetConnected(true);
//     std::cout << "connected" <<std::endl;
// }


// void UsbCameraImpl::PutFrameInternal(std::unique_ptr<Image> image, Frame::Time time) {
//     PutFrame(std::move(image), time);
//     std::lock_guard<std::mutex> lock(m_cameraMutex);
//     m_lastCameraUpdateTime = time;
//     m_cameraCv.notify_all();
// }

// CS_Source CreateUsbCameraDev(llvm::StringRef name, int dev, CS_Status* status) {
//     NSArray* captureDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

//     dev -= 1;


//     std::cout << DeviceNotConnectedString << std::endl;
//     std::string path = DeviceNotConnectedString;
//     path = path + std::to_string(dev);
//     std::cout << path << std::endl;

//     return CreateUsbCameraPath(name, path, status);

// }

// CS_Source CreateUsbCameraPath(llvm::StringRef name, llvm::StringRef path,
//                               CS_Status* status) {
//     auto source = std::make_shared<UsbCameraImpl>(name, path);
//     auto handle = Sources::GetInstance().Allocate(CS_SOURCE_USB, source);
//     Notifier::GetInstance().NotifySource(name, handle, CS_SOURCE_CREATED);
//     // Start thread after the source created event to ensure other events
//     // come after it.
//     source->Start();
//     return handle;
// }

// std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
//     auto data = Sources::GetInstance().Get(source);
//     if (!data || data->kind != CS_SOURCE_USB) {
//         *status = CS_INVALID_HANDLE;
//         return std::string{};
//     }
//     return static_cast<UsbCameraImpl&>(*data->source).GetPath();
// }

  // Property functions
  void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {
    (void)property;
    (void)value;
    (void)status;
  }
  void UsbCameraImpl::SetStringProperty(int property, std::string_view value,
                         CS_Status* status) {
                              (void)property;
    (void)value;
    (void)status;
                         }

  // Standard common camera properties
  void UsbCameraImpl::SetBrightness(int brightness, CS_Status* status) {
        (void)brightness;
    (void)status;
  }
  int UsbCameraImpl::GetBrightness(CS_Status* status) const {(void)status;
  return 0;}
  void UsbCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {(void)status;}
  void UsbCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {(void)status;}
  void UsbCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {(void)status; (void)value;}
  void UsbCameraImpl::SetExposureAuto(CS_Status* status) {(void)status;}
  void UsbCameraImpl::SetExposureHoldCurrent(CS_Status* status) {(void)status;}
  void UsbCameraImpl::SetExposureManual(int value, CS_Status* status) {(void)status; (void)value;}

  bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {(void)status; (void)mode; return false;}
  bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                      CS_Status* status) {(void)status; (void)pixelFormat; return false;}
  bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {(void)status; (void)width; (void)height; return false;}
  bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {(void)status; (void)fps; return false;}

  void UsbCameraImpl::NumSinksChanged() {}
  void UsbCameraImpl::NumSinksEnabledChanged() {}

  void UsbCameraImpl::SetNewPath(std::string_view path, CS_Status* status) {
    UsbCameraImplObjc* objc = static_cast<UsbCameraImplObjc*>(m_objc);
    [objc setNewPath: &path];
    (void)status;
  }
  std::string UsbCameraImpl::GetCurrentPath() {
    std::string ret;
    UsbCameraImplObjc* objc = static_cast<UsbCameraImplObjc*>(m_objc);
    [objc getCurrentPath: &ret];
    return ret;
  }

  std::string UsbCameraImpl::GetCameraName() {
    std::string ret;
    UsbCameraImplObjc* objc = static_cast<UsbCameraImplObjc*>(m_objc);
    [objc getCameraName: &ret];
    return ret;
  }

CS_Source CreateUsbCameraDev(std::string_view name, int dev,
                             CS_Status* status) {
  std::vector<UsbCameraInfo> devices = cs::EnumerateUsbCameras(status);
  if (static_cast<int>(devices.size()) > dev) {
    return CreateUsbCameraPath(name, devices[dev].path, status);
  }
    auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_USB, std::make_shared<UsbCameraImpl>(
                                              name, inst.logger, inst.notifier,
                                              inst.telemetry, dev));
}


CS_Source CreateUsbCameraPath(std::string_view name, std::string_view path,
                              CS_Status* status) {
                                (void)status;
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_USB, std::make_shared<UsbCameraImpl>(
                                              name, inst.logger, inst.notifier,
                                              inst.telemetry, path));
}

std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  @autoreleasepool {
  (void)status;
    std::vector<UsbCameraInfo> retval;
    NSArray<AVCaptureDeviceType>* deviceTypes = @[AVCaptureDeviceTypeBuiltInWideAngleCamera, AVCaptureDeviceTypeExternalUnknown];
    AVCaptureDeviceDiscoverySession* session = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:deviceTypes mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionUnspecified];

    NSArray* captureDevices = [session devices];

    int count = 0;
    for (id device in captureDevices) {
        NSString* name = [device localizedName];
        NSString* uniqueIdentifier = [(AVCaptureDevice*)device uniqueID];
        retval.push_back({count, [uniqueIdentifier UTF8String], [name UTF8String], {}});

        count++;
    }

    return retval;
  }
}

void SetUsbCameraPath(CS_Source source, std::string_view path,
                      CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<UsbCameraImpl&>(*data->source).SetNewPath(path, status);
}

std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<UsbCameraImpl&>(*data->source).GetCurrentPath();
}

UsbCameraInfo GetUsbCameraInfo(CS_Source source, CS_Status* status) {
  UsbCameraInfo info;
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return info;
  }

  info.path = static_cast<UsbCameraImpl&>(*data->source).GetCurrentPath();
  info.name = static_cast<UsbCameraImpl&>(*data->source).GetCameraName();
  info.productId = 0;
  info.vendorId = 0;
  //ParseVidAndPid(info.path, &info.productId, &info.vendorId);
  info.dev = -1;  // We have lost dev information by this point in time.
  return info;
}

}  // namespace cs
