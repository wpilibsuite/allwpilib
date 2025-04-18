// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import "UvcControlImpl.h"
#import <AVFoundation/AVFoundation.h>

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
        NSLog(@"[UvcControlImpl] device is nil");
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSError* error = nil;
    NSRegularExpression* regex = [NSRegularExpression regularExpressionWithPattern:@"^UVC\\s+Camera\\s+VendorID\\_([0-9]+)\\s+ProductID\\_([0-9]+)$" 
                                                                         options:0 
                                                                           error:&error];
    if (error) {
        NSLog(@"[UvcControlImpl] failed to create regex: %@", error);
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSString* modelID = [device valueForKey:@"modelID"];
    if (!modelID) {
        NSLog(@"[UvcControlImpl] modelID is nil");
        *status = CS_UVC_STATUS_ERROR;
        return nil;
    }
    
    NSTextCheckingResult* match = [regex firstMatchInString:modelID 
                                                   options:0 
                                                     range:NSMakeRange(0, modelID.length)];
    if (!match || match.numberOfRanges != 3) {
        NSLog(@"[UvcControlImpl] modelID regex match failed");
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
        NSLog(@"[UvcControlImpl] failed to create UvcControlImpl, status=%d", *status);
    }
    return instance;
}

- (instancetype)initWithVendorId:(uint16_t)vid 
                      productId:(uint16_t)pid 
                      location:(uint32_t)location
                         status:(CS_Status*)status {
    self = [super init];
    if (self) {
        NSLog(@"[UvcControlImpl] Initializing with VID: 0x%04X, PID: 0x%04X, Location: 0x%08X", vid, pid, location);
        _deviceInterface = [self findDevice:vid productId:pid location:location];
        if (_deviceInterface == nullptr) {
            NSLog(@"[UvcControlImpl] Failed to find device");
            *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
            return nil;
        }
        
        _processingUnitID = [self getProcessingUnitID:_deviceInterface];
        NSLog(@"[UvcControlImpl] Processing Unit ID: %u", _processingUnitID);
        
        _controlInterface = [self createControlInterface:_deviceInterface];
        
        if (_controlInterface == nullptr) {
            NSLog(@"[UvcControlImpl] Failed to create control interface");
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
    NSLog(@"[UvcControlImpl] findDevice: called");

    CFMutableDictionaryRef dict = IOServiceMatching(kIOUSBDeviceClassName);
    
    io_iterator_t serviceIterator;
    kern_return_t result = IOServiceGetMatchingServices((mach_port_t)NULL, dict, &serviceIterator);
    if (result != kIOReturnSuccess) {
        NSLog(@"[UvcControlImpl] findDevice: IOServiceGetMatchingServices failed: %d", result);
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
            NSLog(@"[UvcControlImpl] findDevice: Camera control error: %d", result);
            IOObjectRelease(device);
            continue;
        }
        
        HRESULT hr = (*plugInInterface)->QueryInterface(plugInInterface,
            CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID),
            (LPVOID*)&deviceInterface);
            
        if (hr || (deviceInterface == nullptr)) {
            (*plugInInterface)->Release(plugInInterface);
            IOObjectRelease(device);
            NSLog(@"[UvcControlImpl] findDevice: QueryInterface failed");
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

    NSLog(@"[UvcControlImpl] getProcessingUnitID: USB descriptor:\n");
    NSLog(@"  length    = %08X\n", configDesc->bLength);
    NSLog(@"  type      = %08X\n", configDesc->bDescriptorType);
    NSLog(@"  totalLen  = %08X\n", configDesc->wTotalLength);
    NSLog(@"  interfaces = %08X\n", configDesc->bNumInterfaces);
    
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
                    NSLog(@"[UvcControlImpl] getProcessingUnitID: Endpoint\n");
                    break;
                case 0x02:  // Configuration descriptor ID
                    NSLog(@"[UvcControlImpl] getProcessingUnitID: Configuration\n");
                    break;
                case 0x04: // Interface descriptor ID
                    NSLog(@"[UvcControlImpl] getProcessingUnitID: Interface");
                    iface = (IOUSBInterfaceDescriptor*)&ptr[idx];
                    if ((iface->bInterfaceClass == 14) && 
                        (iface->bInterfaceSubClass == 1) &&
                        (iface->bInterfaceProtocol == 0))
                    {
                        inVideoControlInterfaceDescriptor = true;
                        NSLog(@" VIDEO/CONTROL\n");
                    }
                    else
                    {
                        inVideoControlInterfaceDescriptor = false;
                        NSLog(@"\n");
                    }
                    break;
                case 0x24: // class-specific ID
                    pud = (ProcessingUnitDescriptor*)&ptr[idx];
                    if (inVideoControlInterfaceDescriptor)
                    {
                        if (pud->bDescriptorSubtype == 0x05)
                        {
                            NSLog(@"Processing Unit ID: %d\n", 
                                pud->bUnitID);
                            return pud->bUnitID;
                        }
                    }
                    //LOG(LOG_VERBOSE,"\n");
                    break;
                default:
                    //LOG(LOG_VERBOSE,"?\n");
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
            NSLog(@"[UvcControlImpl] createControlInterface: cannot create plug-in %08X",
                kr);
            return nullptr;
        }
        
        HRESULT hr = (*plugInInterface)->QueryInterface(plugInInterface,
            CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID),
            (LPVOID*) &controlInterface);
            
        (*plugInInterface)->Release(plugInInterface);
        
        if (hr || !controlInterface) {
            NSLog(@"[UvcControlImpl] createControlInterface: cannot create device interface %08X",
                result);
            return nullptr;
        }
        
        NSLog(@"[UvcControlImpl] createControlInterface: created control interface");
        return controlInterface;
    }
    return nullptr;
}

- (bool)sendControlRequest:(IOUSBDevRequest)req {
    if (_controlInterface == nullptr) {
        NSLog(@"[UvcControlImpl] sendControlRequest: control interface is NULL");
        return false;
    }

    kern_return_t kr;
    if (@available(macOS 12.0, *)) {
        // macOS 12 doesn't like if we're trying to open USB interface here...
    } else {
        kr = (*_controlInterface)->USBInterfaceOpen(_controlInterface);
        if (kr != kIOReturnSuccess) {
            NSLog(@"[UvcControlImpl] sendControlRequest: USBInterfaceOpen failed with error: 0x%08X", kr);
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
                NSLog(@"[UvcControlImpl] sendControlRequest: Pipe ref not recognised");
                break;
            case kIOUSBTooManyPipesErr:
                NSLog(@"[UvcControlImpl] sendControlRequest: Too many pipes");
                break;
            case kIOUSBEndpointNotFound:
                NSLog(@"[UvcControlImpl] sendControlRequest: Endpoint not found");
                break;
            case kIOUSBConfigNotFound:
                NSLog(@"[UvcControlImpl] sendControlRequest: USB configuration not found");
                break;
            case kIOUSBPipeStalled:
                //Note: we don't report this as an error as this happens when
                //      an unsupported or locked property is set.            
                NSLog(@"[UvcControlImpl] sendControlRequest: Pipe has stalled, error needs to be cleared");
                break;
            case kIOUSBInterfaceNotFound:
                NSLog(@"[UvcControlImpl] sendControlRequest: USB control interface not found");
                break;
            default:
                NSLog(@"[UvcControlImpl] sendControlRequest: ControlRequest failed (KR=sys:sub:code) = %02Xh:%03Xh:%04Xh)", 
                    sys, sub, code);
                break; 
        }

        if (@available(macOS 12.0, *)) {
            // macOS 12 doesn't like if we're trying to close USB interface here...
        } else {
            kr = (*_controlInterface)->USBInterfaceClose(_controlInterface);
            if (kr != kIOReturnSuccess) {
                NSLog(@"[UvcControlImpl] sendControlRequest: USBInterfaceClose failed");
            }
        }
        return false;
    }

    if (@available(macOS 12.0, *)) {
        // macOS 12 doesn't like if we're trying to close USB interface here either...
    } else {
        kr = (*_controlInterface)->USBInterfaceClose(_controlInterface);

        if (kr != kIOReturnSuccess) {
            NSLog(@"[UvcControlImpl] sendControlRequest: USBInterfaceClose failed");
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
        NSLog(@"[UvcControlImpl] setProperty: control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    bool ok = false;
    if (propID < CAPPROPID_LAST) {
        uint32_t unit = (propertyInfo[propID].unit == 0) ? UVC_INPUT_TERMINAL_ID : _processingUnitID;
        NSLog(@"[UvcControlImpl] Setting property %u to value %d (unit: %u)", propID, value, unit);
        ok = [self setData:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:value];
        if (!ok) {
            NSLog(@"[UvcControlImpl] Failed to set property %u", propID);
        }
    } else {
        NSLog(@"[UvcControlImpl] Invalid property ID: %u", propID);
    }
    return ok;
}

- (bool)getProperty:(uint32_t)propID withValue:(int32_t*)value status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        NSLog(@"[UvcControlImpl] getProperty: control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    bool ok = false;
    if (propID < CAPPROPID_LAST) {
        uint32_t unit = (propertyInfo[propID].unit == 0) ? UVC_INPUT_TERMINAL_ID : _processingUnitID;
        NSLog(@"[UvcControlImpl] Getting property %u (unit: %u)", propID, unit);
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
        if (ok) {
            NSLog(@"[UvcControlImpl] Property %u value: %d", propID, *value);
        } else {
            NSLog(@"[UvcControlImpl] Failed to get property %u", propID);
        }
    } else {
        NSLog(@"[UvcControlImpl] Invalid property ID: %u", propID);
    }
    return ok;
}

- (bool)setAutoProperty:(uint32_t)propID enabled:(bool)enabled status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        NSLog(@"[UvcControlImpl] setAutoProperty: control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    int32_t value = enabled ? 1 : 0;
    switch(propID) {
        case CAPPROPID_EXPOSURE:
            NSLog(@"[UvcControlImpl] Setting auto property %u to %@", propID, enabled ? @"enabled" : @"disabled");
            return [self setData:CT_AE_MODE_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:enabled ? 0x8 : 0x1];
        case CAPPROPID_WHITEBALANCE:
            NSLog(@"[UvcControlImpl] Setting auto property %u to %@", propID, enabled ? @"enabled" : @"disabled");
            return [self setData:PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL unit:_processingUnitID length:1 data:value];
        case CAPPROPID_FOCUS:
            NSLog(@"[UvcControlImpl] Setting auto property %u to %@", propID, enabled ? @"enabled" : @"disabled");
            return [self setData:CT_FOCUS_AUTO_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:value];
        default:
            return false;
    }
}

- (bool)getAutoProperty:(uint32_t)propID enabled:(bool*)enabled status:(CS_Status*)status {
    if (_controlInterface == nullptr) {
        NSLog(@"[UvcControlImpl] getAutoProperty: control interface is NULL");
        *status = CS_UVC_STATUS_DEVICE_DISCONNECTED;
        return false;
    }

    int32_t value;
    NSLog(@"[UvcControlImpl] Getting auto property %u", propID);
    
    switch(propID) {
        case CAPPROPID_EXPOSURE:
            if ([self getData:CT_AE_MODE_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:&value]) {
                NSLog(@"[UvcControlImpl] Exposure auto mode: %@", *enabled ? @"enabled" : @"disabled");
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
                NSLog(@"[UvcControlImpl] White balance auto mode: %@", *enabled ? @"enabled" : @"disabled");
                return true;
            }
            return false;
        case CAPPROPID_FOCUS:
            if ([self getData:CT_FOCUS_AUTO_CONTROL unit:UVC_INPUT_TERMINAL_ID length:1 data:&value]) {
                value &= 0xFF;
                *enabled = (value==1) ? true : false;
                NSLog(@"[UvcControlImpl] Focus auto mode: %@", *enabled ? @"enabled" : @"disabled");
                return true;
            }
            return false;
        default:
            NSLog(@"[UvcControlImpl] Unsupported auto property ID: %u", propID);
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
            NSLog(@"[UvcControlImpl] getPropertyLimits: getMinData failed");
            ok = false;
        }

        if (![self getMaxData:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:max]) {
            NSLog(@"[UvcControlImpl] getPropertyLimits: getMaxData failed");
            ok = false;
        }

        if (![self getDefault:propertyInfo[propID].selector unit:unit length:propertyInfo[propID].length data:defValue]) {
            NSLog(@"[UvcControlImpl] getPropertyLimits: getDefault failed");
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
        NSLog(@"[UvcControlImpl] getPropertyLimits: property ID out of bounds");
        ok = false;
    }
    return ok;
}

- (void)reportCapabilities:(uint32_t)selector unit:(uint32_t)unit {
    uint32_t info;
    [self getInfo:selector unit:unit data:&info];
    if (info & 0x01) {
        NSLog(@"GET ");
    }
    if (info & 0x02) {
        NSLog(@"SET ");
    }
    if (info & 0x04) {
        NSLog(@"DISABLED ");
    }
    if (info & 0x08) {
        NSLog(@"AUTO-UPD ");
    }
    if (info & 0x10) {
        NSLog(@"ASYNC ");
    }
    if (info & 0x20) {
        NSLog(@"DISCOMMIT");
    }
    NSLog(@"\n");
}

@end 