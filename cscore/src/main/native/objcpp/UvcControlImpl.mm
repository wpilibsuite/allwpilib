// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import "UvcControlImpl.h"
#import <AVFoundation/AVFoundation.h>

#include "Log.h"
#include "UsbCameraImpl.h"

template <typename S, typename... Args>
inline void NamedLog(UvcControlImpl* objc, unsigned int level,
                     const char* file, unsigned int line, const S& format,
                     Args&&... args) {
  auto sharedThis = objc.cppImpl.lock();
  if (!sharedThis) {
    return;
  }

  wpi::Logger& logger = sharedThis->objcGetLogger();
  std::string_view name = sharedThis->GetName();

  if (logger.HasLogger() && level >= logger.min_level()) {
    cs::NamedLogV(logger, level, file, line, name, format,
                  fmt::make_format_args(args...));
  }
}

#define UVCLOG(level, format, ...) \
  NamedLog(self, level, __FILE__, __LINE__, \
           format __VA_OPT__(, ) __VA_ARGS__)

#define UVCERROR(format, ...) \
  UVCLOG(::wpi::WPI_LOG_ERROR, format __VA_OPT__(, ) __VA_ARGS__)
#define UVCWARNING(format, ...) \
  UVCLOG(::wpi::WPI_LOG_WARNING, format __VA_OPT__(, ) __VA_ARGS__)
#define UVCINFO(format, ...) \
  UVCLOG(::wpi::WPI_LOG_INFO, format __VA_OPT__(, ) __VA_ARGS__)

#ifdef NDEBUG
#define UVCDEBUG(format, ...) \
  do {                         \
  } while (0)
#define UVCDEBUG1(format, ...) \
  do {                          \
  } while (0)
#define UVCDEBUG2(format, ...) \
  do {                          \
  } while (0)
#define UVCDEBUG3(format, ...) \
  do {                          \
  } while (0)
#define UVCDEBUG4(format, ...) \
  do {                          \
  } while (0)
#else
#define UVCDEBUG(format, ...) \
  UVCLOG(::wpi::WPI_LOG_DEBUG, format __VA_OPT__(, ) __VA_ARGS__)
#define UVCDEBUG1(format, ...) \
  UVCLOG(::wpi::WPI_LOG_DEBUG1, format __VA_OPT__(, ) __VA_ARGS__)
#define UVCDEBUG2(format, ...) \
  UVCLOG(::wpi::WPI_LOG_DEBUG2, format __VA_OPT__(, ) __VA_ARGS__)
#define UVCDEBUG3(format, ...) \
  UVCLOG(::wpi::WPI_LOG_DEBUG3, format __VA_OPT__(, ) __VA_ARGS__)
#define UVCDEBUG4(format, ...) \
  UVCLOG(::wpi::WPI_LOG_DEBUG4, format __VA_OPT__(, ) __VA_ARGS__)
#endif

// USB descriptor for UVC processing unit
struct ProcessingUnitDescriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;        // CS_INTERFACE 0x24
    uint8_t bDescriptorSubtype;     // VC_PROCESSING_UNIT 0x05
    uint8_t bUnitID;
};

struct propertyInfo_t
{
    uint32_t    selector;   // selector ID
    uint32_t    unit;       // unit (==0 for INPUT TERMINA:, ==1 for PROCESSING UNIT)
    uint32_t    length;     // length (bytes)
};

/** The order of the propertyInfo structure must
    be the same as the PROPID numbers in the
    openpnp-capture.h header */
const propertyInfo_t propertyInfo[] =
{
    {0,0,0},
    {CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   , 0, 4},
    {CT_FOCUS_ABSOLUTE_CONTROL           , 0, 2},
    {CT_ZOOM_ABSOLUTE_CONTROL            , 0, 2},
    {PU_WHITE_BALANCE_TEMPERATURE_CONTROL, 1, 2},
    {PU_GAIN_CONTROL                     , 1, 2},
    {PU_BRIGHTNESS_CONTROL               , 1, 2},
    {PU_CONTRAST_CONTROL                 , 1, 2},
    {PU_SATURATION_CONTROL               , 1, 2},
    {PU_GAMMA_CONTROL                    , 1, 2},
    {PU_HUE_CONTROL                      , 1, 2},
    {PU_SHARPNESS_CONTROL                , 1, 2},
    {PU_BACKLIGHT_COMPENSATION_CONTROL   , 1, 2},
    {PU_POWER_LINE_FREQUENCY_CONTROL     , 1, 1}
};

@implementation UvcControlImpl {
    IOUSBDeviceInterface** _deviceInterface;
}


+ (instancetype)createFromAVCaptureDevice:(AVCaptureDevice*)device status:(CS_Status*)status {
    if (!device) {
        NSLog(@"UVC: device is nil");
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSError* error = nil;
    NSRegularExpression* regex = [NSRegularExpression regularExpressionWithPattern:@"^UVC\\s+Camera\\s+VendorID\\_([0-9]+)\\s+ProductID\\_([0-9]+)$" 
                                                                         options:0 
                                                                           error:&error];
    if (error) {
        NSLog(@"UVC: failed to create regex: %@", error);
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSString* modelID = [device valueForKey:@"modelID"];
    if (!modelID) {
        NSLog(@"UVC: modelID is nil");
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSTextCheckingResult* match = [regex firstMatchInString:modelID 
                                                   options:0 
                                                     range:NSMakeRange(0, modelID.length)];
    if (!match || match.numberOfRanges != 3) {
        NSLog(@"UVC: modelID regex match failed");
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSString* vendorIDStr = [modelID substringWithRange:[match rangeAtIndex:1]];
    NSString* productIDStr = [modelID substringWithRange:[match rangeAtIndex:2]];
    uint16_t vendorID = (uint16_t)strtoul([vendorIDStr UTF8String], NULL, 10);
    uint16_t productID = (uint16_t)strtoul([productIDStr UTF8String], NULL, 10);
    
    uint32_t locationID = 0;
    CFMutableDictionaryRef dict = IOServiceMatching(kIOUSBDeviceClassName);
    CFDictionarySetValue(dict, CFSTR("idVendor"), (__bridge CFNumberRef)@(vendorID));
    CFDictionarySetValue(dict, CFSTR("idProduct"), (__bridge CFNumberRef)@(productID));
    
    io_iterator_t iter = 0;
    kern_return_t ioResult = IOServiceGetMatchingServices(kIOMainPortDefault, dict, &iter);
    if (ioResult == kIOReturnSuccess) {
        io_service_t usbDevice = IOIteratorNext(iter);
        while (usbDevice != 0) {
            CFTypeRef locationIDRef = IORegistryEntryCreateCFProperty(usbDevice, 
                                                                    CFSTR("locationID"), 
                                                                    kCFAllocatorDefault, 
                                                                    0);
            if (locationIDRef) {
                locationID = [(__bridge NSNumber*)locationIDRef unsignedIntValue];
                CFRelease(locationIDRef);
                NSString* uniqueID = [device valueForKey:@"uniqueID"];
                NSString* locationIDHex = [NSString stringWithFormat:@"0x%x", locationID];
                if ([uniqueID hasPrefix:locationIDHex]) {
                    IOObjectRelease(usbDevice);
                    break;
                }
            }
            IOObjectRelease(usbDevice);
            usbDevice = IOIteratorNext(iter);
        }
        IOObjectRelease(iter);
    }
    
    UvcControlImpl *instance = [[UvcControlImpl alloc] initWithVendorId:vendorID 
                                        productId:productID 
                                        location:locationID 
                                           status:status];
    if (!instance) {
        NSLog(@"UVC: failed to create UvcControlImpl, status=%d", *status);
    }
    return instance;
}

- (instancetype)initWithVendorId:(uint16_t)vid 
                      productId:(uint16_t)pid 
                      location:(uint32_t)location
                         status:(CS_Status*)status {
    self = [super init];
    if (self) {
        // UVCINFO("Initializing with VID: 0x{:04X}, PID: 0x{:04X}, Location: 0x{:08X}", vid, pid, location);
        _deviceInterface = [self findDevice:vid productId:pid location:location];
        if (_deviceInterface == nullptr) {
            // UVCWARNING("Failed to find device");
            *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
            return nil;
        }
        
        _processingUnitID = [self getProcessingUnitID:_deviceInterface];
        
        _controlInterface = [self createControlInterface:_deviceInterface];
        
        if (_controlInterface == nullptr) {
            // UVCWARNING("Failed to create control interface");
            *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
            return nil;
        }
    }
    return self;
}

- (void)dealloc {
    if (_controlInterface != nullptr) {
        (*_controlInterface)->USBInterfaceClose(_controlInterface);
        (*_controlInterface)->Release(_controlInterface);
    }
    if (_deviceInterface != nullptr) {
        (*_deviceInterface)->Release(_deviceInterface);
    }
}

- (IOUSBDeviceInterface**)findDevice:(uint16_t)vid 
                          productId:(uint16_t)pid 
                          location:(uint32_t)location {

    CFMutableDictionaryRef dict = IOServiceMatching(kIOUSBDeviceClassName);
    
    io_iterator_t serviceIterator;
    kern_return_t result = IOServiceGetMatchingServices((mach_port_t)NULL, dict, &serviceIterator);
    if (result != kIOReturnSuccess) {
        UVCERROR("findDevice: IOServiceGetMatchingServices failed: {}", result);
        return nullptr;
    }
    
    io_service_t device;
    while((device = IOIteratorNext(serviceIterator)) != 0) {
        IOUSBDeviceInterface **deviceInterface = nullptr;
        IOCFPlugInInterface  **plugInInterface = nullptr;
        SInt32 score;
        
        kern_return_t result = IOCreatePlugInInterfaceForService(
            device, kIOUSBDeviceUserClientTypeID,
            kIOCFPlugInInterfaceID, &plugInInterface, &score);
            
        if ((result != kIOReturnSuccess) || (plugInInterface == nullptr)) {
            UVCERROR("findDevice: Camera control error: {}", result);
            IOObjectRelease(device);
            continue;
        }
        
        HRESULT hr = (*plugInInterface)->QueryInterface(plugInInterface,
            CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID),
            (LPVOID*)&deviceInterface);
            
        if (hr || (deviceInterface == nullptr)) {
            (*plugInInterface)->Release(plugInInterface);
            IOObjectRelease(device);
            UVCERROR("findDevice: QueryInterface failed");
            continue;
        }
        
        uint16_t vendorID, productID;
        uint32_t locationID;
        result = (*deviceInterface)->GetDeviceVendor(deviceInterface, &vendorID);
        result = (*deviceInterface)->GetDeviceProduct(deviceInterface, &productID);
        result = (*deviceInterface)->GetLocationID(deviceInterface, &locationID);
        
        // if 'location' is zero, we won't match on location
        // to achieve this, we simply set locationID to zero.
        if (location == 0) {
            locationID = 0;
        }
        
        if ((vendorID == vid) && (productID == pid) && (locationID == location)) {
            (*plugInInterface)->Release(plugInInterface);
            IOObjectRelease(device);
            IOObjectRelease(serviceIterator);
            return deviceInterface;
        }
        
        (*deviceInterface)->Release(deviceInterface);
        (*plugInInterface)->Release(plugInInterface);
        IOObjectRelease(device);
    }
    
    IOObjectRelease(serviceIterator);
    return nullptr;
}

- (uint32_t)getProcessingUnitID:(IOUSBDeviceInterface**)dev {
    IOReturn kr;
    IOUSBConfigurationDescriptorPtr configDesc;
    
    kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &configDesc);
    if (kr) {
        return 0;
    }

    UVCDEBUG4("USB descriptor:");
    UVCDEBUG4("  length    = 0x{:08X}", configDesc->bLength);
    UVCDEBUG4("  type      = 0x{:08X}", configDesc->bDescriptorType);
    UVCDEBUG4("  totalLen  = 0x{:08X}", configDesc->wTotalLength);
    UVCDEBUG4("  interfaces = 0x{:08X}", configDesc->bNumInterfaces);
    
    uint32_t idx = 0;
    uint8_t *ptr = (uint8_t*)configDesc;

    // Search for VIDEO/CONTROL interface descriptor
    // Class=14, Subclass=1, Protocol=0
    // and find the processing unit, if available..
        // DescriptorType 0x24, DescriptorSubType 0x5

    IOUSBInterfaceDescriptor *iface = NULL;
    ProcessingUnitDescriptor *pud   = NULL;
    bool inVideoControlInterfaceDescriptor = false;
    while(idx < configDesc->wTotalLength) {
        IOUSBDescriptorHeader *hdr = (IOUSBDescriptorHeader *)&ptr[idx];
        switch(hdr->bDescriptorType)
            {
                case 0x05:  // Endpoint descriptor ID
                    break;
                case 0x02:  // Configuration descriptor ID
                    break;
                case 0x04: // Interface descriptor ID
                    iface = (IOUSBInterfaceDescriptor*)&ptr[idx];
                    if ((iface->bInterfaceClass == 14) && 
                        (iface->bInterfaceSubClass == 1) &&
                        (iface->bInterfaceProtocol == 0))
                    {
                        inVideoControlInterfaceDescriptor = true;
                    }
                    else
                    {
                        inVideoControlInterfaceDescriptor = false;
                    }
                    break;
                case 0x24: // class-specific ID
                    pud = (ProcessingUnitDescriptor*)&ptr[idx];
                    if (inVideoControlInterfaceDescriptor)
                    {
                        if (pud->bDescriptorSubtype == 0x05)
                        {
                            return pud->bUnitID;
                        }
                    }
                    break;
                default:
                    break;
            }
        idx += hdr->bLength;
    }
    
    return 0;
}

- (IOUSBInterfaceInterface190**)createControlInterface:(IOUSBDeviceInterface**)deviceInterface {
    IOUSBInterfaceInterface190 **controlInterface;
    
    io_iterator_t interfaceIterator;
    IOUSBFindInterfaceRequest interfaceRequest;
    interfaceRequest.bInterfaceClass = UVC_CONTROL_INTERFACE_CLASS;
    interfaceRequest.bInterfaceSubClass = UVC_CONTROL_INTERFACE_SUBCLASS;
    interfaceRequest.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;
    
    IOReturn result = (*deviceInterface)->CreateInterfaceIterator(deviceInterface,
        &interfaceRequest, &interfaceIterator);
        
    if (result != kIOReturnSuccess) {
        return nullptr;
    }
    
    io_service_t usbInterface;
    if ((usbInterface = IOIteratorNext(interfaceIterator)) != 0) {
        IOCFPlugInInterface **plugInInterface = nullptr;
        SInt32 score;
        
        kern_return_t kr = IOCreatePlugInInterfaceForService(usbInterface,
            kIOUSBInterfaceUserClientTypeID,
            kIOCFPlugInInterfaceID,
            &plugInInterface,
            &score);
            
        kr = IOObjectRelease(usbInterface);
        if ((kr != kIOReturnSuccess) || !plugInInterface) {
            UVCERROR("createControlInterface: cannot create plug-in {:08X}",
                kr);
            return nullptr;
        }
        
        HRESULT hr = (*plugInInterface)->QueryInterface(plugInInterface,
            CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID),
            (LPVOID*) &controlInterface);
            
        (*plugInInterface)->Release(plugInInterface);
        
        if (hr || !controlInterface) {
            UVCERROR("createControlInterface: cannot create device interface {:08X}",
                result);
            return nullptr;
        }
        
        UVCDEBUG3("createControlInterface: created control interface");
        return controlInterface;
    }
    return nullptr;
}

- (bool)sendControlRequest:(IOUSBDevRequest)req {
    if (_controlInterface == nullptr) {
        UVCERROR("control interface is NULL");
        return false;
    }

    kern_return_t kr;
    if (@available(macOS 12.0, *)) {
        // macOS 12 doesn't like if we're trying to open USB interface here...
    } else {
        kr = (*_controlInterface)->USBInterfaceOpen(_controlInterface);
        if (kr != kIOReturnSuccess) {
            UVCERROR("USBInterfaceOpen failed with error: 0x{:08X}", kr);
            return false;
        }
    }

    kr = (*_controlInterface)->ControlRequest(_controlInterface, 0, &req);
    if (kr != kIOReturnSuccess) {
        // IOKIT error code
        #define err_get_system(err) (((err)>>26)&0x3f) 
        #define err_get_sub(err) (((err)>>14)&0xfff) 
        #define err_get_code(err) ((err)&0x3fff)
        
        uint32_t code = err_get_code(kr);
        uint32_t sys  = err_get_system(kr);
        uint32_t sub  = err_get_sub(kr);

        switch(kr)
        {
            case kIOUSBUnknownPipeErr:
                UVCERROR("Pipe ref not recognised");
                break;
            case kIOUSBTooManyPipesErr:
                UVCERROR("Too many pipes");
                break;
            case kIOUSBEndpointNotFound:
                UVCERROR("Endpoint not found");
                break;
            case kIOUSBConfigNotFound:
                UVCERROR("USB configuration not found");
                break;
            case kIOUSBPipeStalled:
                //Note: we don't report this as an error as this happens when
                //      an unsupported or locked property is set.            
                UVCDEBUG("Pipe has stalled, error needs to be cleared");
                break;
            case kIOUSBInterfaceNotFound:
                UVCERROR("USB control interface not found");
                break;
            default:
                UVCERROR("ControlRequest failed (KR=sys:sub:code) = {:02Xh}:{:03Xh}:{:04Xh}", 
                    sys, sub, code);
                break; 
        }

        if (@available(macOS 12.0, *)) {
            // macOS 12 doesn't like if we're trying to close USB interface here...
        } else {
            kr = (*_controlInterface)->USBInterfaceClose(_controlInterface);
            if (kr != kIOReturnSuccess) {
                UVCERROR("USBInterfaceClose failed");
            }
        }
        return false;
    }

    if (@available(macOS 12.0, *)) {
        // macOS 12 doesn't like if we're trying to close USB interface here either...
    } else {
        kr = (*_controlInterface)->USBInterfaceClose(_controlInterface);

        if (kr != kIOReturnSuccess) {
            UVCERROR("USBInterfaceClose failed");
        }
    }

    return true;
}

- (bool)setData:(uint32_t)selector unit:(uint32_t)unit length:(uint32_t)length data:(int32_t)data {
    IOUSBDevRequest req;
    req.bmRequestType = USBmakebmRequestType((UInt8)kUSBOut, (UInt8)kUSBClass, (UInt8)kUSBInterface);
    req.bRequest = UVC_SET_CUR;
    req.wValue = (selector << 8);
    req.wIndex = (unit << 8);
    req.wLength = length;
    req.wLenDone = 0;
    req.pData = &data;
    return [self sendControlRequest:req];
}

- (bool)getData:(uint32_t)selector unit:(uint32_t)unit length:(uint32_t)length data:(int32_t*)data {
    IOUSBDevRequest req;
    req.bmRequestType = USBmakebmRequestType((UInt8)kUSBIn, (UInt8)kUSBClass, (UInt8)kUSBInterface);
    req.bRequest = UVC_GET_CUR;
    req.wValue = (selector << 8);
    req.wIndex = (unit << 8);
    req.wLength = length;
    req.wLenDone = 0;
    req.pData = data;
    return [self sendControlRequest:req];
}

- (bool)getMaxData:(uint32_t)selector unit:(uint32_t)unit length:(uint32_t)length data:(int32_t*)data {
    IOUSBDevRequest req;
    *data = 0;
    req.bmRequestType = USBmakebmRequestType((UInt8)kUSBIn, (UInt8)kUSBClass, (UInt8)kUSBInterface);
    req.bRequest = UVC_GET_MAX;
    req.wValue = (selector << 8);
    req.wIndex = (unit << 8);
    req.wLength = length;
    req.wLenDone = 0;
    req.pData = data;
    return [self sendControlRequest:req];
}

- (bool)getMinData:(uint32_t)selector unit:(uint32_t)unit length:(uint32_t)length data:(int32_t*)data {
    IOUSBDevRequest req;
    *data = 0;
    req.bmRequestType = USBmakebmRequestType((UInt8)kUSBIn, (UInt8)kUSBClass, (UInt8)kUSBInterface);
    req.bRequest = UVC_GET_MIN;
    req.wValue = (selector << 8);
    req.wIndex = (unit << 8);
    req.wLength = length;
    req.wLenDone = 0;
    req.pData = data;
    return [self sendControlRequest:req];
}

- (bool)getDefault:(uint32_t)selector unit:(uint32_t)unit length:(uint32_t)length data:(int32_t*)data {
    IOUSBDevRequest req;
    *data = 0;
    req.bmRequestType = USBmakebmRequestType((UInt8)kUSBIn, (UInt8)kUSBClass, (UInt8)kUSBInterface);
    req.bRequest = UVC_GET_DEF;
    req.wValue = (selector << 8);
    req.wIndex = (unit << 8);
    req.wLength = length;
    req.wLenDone = 0;
    req.pData = data;
    return [self sendControlRequest:req];
}

- (bool)getInfo:(uint32_t)selector unit:(uint32_t)unit data:(uint32_t*)data {
    IOUSBDevRequest req;
    *data = 0;
    req.bmRequestType = USBmakebmRequestType((UInt8)kUSBIn, (UInt8)kUSBClass, (UInt8)kUSBInterface);
    req.bRequest = UVC_GET_INFO;
    req.wValue = (selector << 8);
    req.wIndex = (unit << 8);
    req.wLength = 1;
    req.wLenDone = 0;
    req.pData = data;
    return [self sendControlRequest:req];
}

- (bool)setProperty:(uint32_t)propID withValue:(int32_t)value status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        UVCERROR("control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    bool ok = false;
    if (propID < CAPPROPID_LAST) {
        uint32_t unit = (propertyInfo[propID].unit == 0) ? UVC_INPUT_TERMINAL_ID : _processingUnitID;
        ok = [self setData:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:value];
        if (!ok) {
            UVCWARNING("Failed to set property {}", propID);
        }
    } else {
        UVCWARNING("Invalid property ID: {}", propID);
    }
    return ok;
}

- (bool)getProperty:(uint32_t)propID withValue:(int32_t*)value status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        UVCERROR("control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    bool ok = false;
    if (propID < CAPPROPID_LAST) {
        uint32_t unit = (propertyInfo[propID].unit == 0) ? UVC_INPUT_TERMINAL_ID : _processingUnitID;
        ok = [self getData:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:value];

        switch(propertyInfo[propID].length) {
            case 2:
                *value = static_cast<int16_t>(*value);
                break;
            case 1:
                *value = static_cast<int8_t>(*value);
                break;
            default:
                break;
        }
        if (!ok) {
            UVCWARNING("Failed to get property {}", propID);
        }
    } else {
        UVCWARNING("Invalid property ID: {}", propID);
    }
    return ok;
}

- (bool)setAutoProperty:(uint32_t)propID enabled:(bool)enabled status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        UVCERROR("control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    int32_t value = enabled ? 1 : 0;
    switch(propID) {
        case CAPPROPID_EXPOSURE:
            return [self setData:CT_AE_MODE_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:enabled ? 0x8 : 0x1];
        case CAPPROPID_WHITEBALANCE:
            return [self setData:PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL unit:_processingUnitID length:1 data:value];
        case CAPPROPID_FOCUS:
            return [self setData:CT_FOCUS_AUTO_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:value];
        default:
            return false;
    }
}

- (bool)getAutoProperty:(uint32_t)propID enabled:(bool*)enabled status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        UVCERROR("control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    int32_t value;
    
    switch(propID) {
        case CAPPROPID_EXPOSURE:
            if ([self getData:CT_AE_MODE_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:&value]) {
                // value = 1 -> manual mode
                //         2 -> auto mode (I haven't seen this in the wild)
                //         4 -> shutter priority mode (haven't seen this)
                //         8 -> aperature prioritry mode (seen this used)
                value &= 0xFF;
                *enabled = (value==1) ? false : true;
                return true;
            }
            return false;
        case CAPPROPID_WHITEBALANCE:
            if ([self getData:PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL unit:_processingUnitID length:1 data:&value]) {
                value &= 0xFF;
                *enabled = (value==1) ? true : false;
                UVCDEBUG3("White balance auto mode: {}", *enabled ? "enabled" : "disabled");
                return true;
            }
            return false;
        case CAPPROPID_FOCUS:
            if ([self getData:CT_FOCUS_AUTO_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:&value]) {
                value &= 0xFF;
                *enabled = (value==1) ? true : false;
                UVCDEBUG3("Focus auto mode: {}", *enabled ? "enabled" : "disabled");
                return true;
            }
            return false;
        default:
            UVCWARNING("Unsupported auto property ID: {}", propID);
            return false;
    }
}

- (bool)getPropertyLimits:(uint32_t)propID min:(int32_t*)min max:(int32_t*)max defValue:(int32_t*)defValue status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    bool ok = true;
    if (propID < CAPPROPID_LAST) {
        uint32_t unit = (propertyInfo[propID].unit == 0) ? UVC_INPUT_TERMINAL_ID : _processingUnitID;
        
        if (![self getMinData:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:min]) {
            ok = false;
        }

        if (![self getMaxData:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:max]) {
            ok = false;
        }

        if (![self getDefault:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:defValue]) {
            ok = false;
        }

        switch(propertyInfo[propID].length) {
            case 2:
                *min = static_cast<int16_t>(*min);
                *max = static_cast<int16_t>(*max);
                *defValue = static_cast<int16_t>(*defValue);
                break;
            case 1:
                *min = static_cast<int8_t>(*min);
                *max = static_cast<int8_t>(*max);
                *defValue = static_cast<int8_t>(*defValue);
                break;
            default:
                break;
        }
    } else {
        UVCWARNING("getPropertyLimits: property ID out of bounds");
        ok = false;
    }
    return ok;
}

- (void)reportCapabilities:(uint32_t)selector unit:(uint32_t)unit {
    uint32_t info;
    [self getInfo:selector unit:unit data:&info];
    if (info & 0x01) {
        UVCDEBUG4("GET ");
    }
    if (info & 0x02) {
        UVCDEBUG4("SET ");
    }
    if (info & 0x04) {
        UVCDEBUG4("DISABLED ");
    }
    if (info & 0x08) {
        UVCDEBUG4("AUTO-UPD ");
    }
    if (info & 0x10) {
        UVCDEBUG4("ASYNC ");
    }
    if (info & 0x20) {
        UVCDEBUG4("DISCOMMIT");
    }
    UVCDEBUG4("");
}

@end 