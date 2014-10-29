//==============================================================================
//
//  Title	  : NIIMAQdx.h
//  Created	  : 1403685834 seconds after 1/1/1970 12:00:00 UTC 
//  Copyright : © Copyright 2006, National Instruments Corporation, All rights reserved
// 	Purpose	  : Include file for NI-IMAQdx library support.
//
//==============================================================================
#ifndef ___niimaqdx_h___
#define ___niimaqdx_h___


#ifdef __cplusplus
extern "C" {
#endif


#if !defined(niimaqdx_types)
#define niimaqdx_types

#ifdef _CVI_
    #pragma  EnableLibraryRuntimeChecking
#endif

//==============================================================================
//  Typedefs
//==============================================================================
#ifndef _NI_uInt8_DEFINED_
    #define _NI_uInt8_DEFINED_
    typedef unsigned char       uInt8;
#endif

#ifndef _NI_uInt16_DEFINED_
    #define _NI_uInt16_DEFINED_
    typedef unsigned short int  uInt16;
#endif

#ifndef _NI_uInt32_DEFINED_
    #define _NI_uInt32_DEFINED_
    #if defined(_MSC_VER)
        typedef unsigned long       uInt32;
    #elif __GNUC__
        #if __x86_64__
            typedef unsigned int    uInt32;
        #else
            typedef unsigned long   uInt32;
        #endif
    #endif
#endif

#ifndef _NI_uInt64_DEFINED_
    #define _NI_uInt64_DEFINED_
    #if defined(_MSC_VER) || _CVI_ >= 700
        typedef unsigned __int64    uInt64;
    #elif __GNUC__
        typedef unsigned long long  uInt64;
    #endif
#endif

#ifndef _NI_Int8_DEFINED_
    #define _NI_Int8_DEFINED_
    typedef char                Int8;
#endif

#ifndef _NI_Int16_DEFINED_
    #define _NI_Int16_DEFINED_
    typedef short int           Int16;
#endif

#ifndef _NI_Int32_DEFINED_
    #define _NI_Int32_DEFINED_
    #if defined(_MSC_VER)
        typedef long            Int32;
    #elif __GNUC__
        #if __x86_64__
            typedef int         Int32;
        #else
            typedef long        Int32;
        #endif
    #endif
#endif  

#ifndef _NI_Int64_DEFINED_
    #define _NI_Int64_DEFINED_
    #if defined(_MSC_VER) || _CVI_ >= 700
        typedef __int64        Int64;
    #elif __GNUC__
        typedef long long int  Int64;
    #endif
#endif

#ifndef _NI_float32_DEFINED_
    #define _NI_float32_DEFINED_
    typedef float               float32;
#endif

#ifndef _NI_float64_DEFINED_
    #define _NI_float64_DEFINED_
    typedef double              float64;
#endif

#ifndef TRUE
    #define TRUE                (1L)
#endif

#ifndef FALSE
    #define FALSE               (0L)
#endif

#ifndef _NI_GUIDHNDL_DEFINED
    typedef uInt32              GUIHNDL;
#endif

#if (defined(_MSC_VER) || defined(_CVI_))
    #ifndef _NI_FUNC_DEFINED
        #define NI_FUNC         __stdcall
    #endif
    
    #ifndef _NI_FUNCC_DEFINED
        #define NI_FUNCC        __cdecl
    #endif
#elif defined(__GNUC__)
    #ifndef _NI_FUNC_DEFINED
        #define NI_FUNC
    #endif
    
    #ifndef _NI_FUNCC_DEFINED
        #define NI_FUNCC
    #endif
#endif

#ifndef _NI_bool32_DEFINED_
    #define _NI_bool32_DEFINED_
    typedef uInt32              bool32;
#endif

#ifndef _NI_IMAQdxSession_DEFINED_
    #define _NI_IMAQdxSession_DEFINED_
    typedef uInt32              IMAQdxSession;
#endif

#define IMAQDX_MAX_API_STRING_LENGTH 512

//==============================================================================
//  Forward Declare Data Structures
//==============================================================================
typedef struct Image_struct Image;


//==============================================================================
//  Error Codes Enumeration
//==============================================================================
typedef enum IMAQdxError_enum {
    IMAQdxErrorSuccess = 0x0,                   // Success
    IMAQdxErrorSystemMemoryFull = 0xBFF69000,   // Not enough memory
    IMAQdxErrorInternal,                        // Internal error
    IMAQdxErrorInvalidParameter,                // Invalid parameter
    IMAQdxErrorInvalidPointer,                  // Invalid pointer
    IMAQdxErrorInvalidInterface,                // Invalid camera session
    IMAQdxErrorInvalidRegistryKey,              // Invalid registry key
    IMAQdxErrorInvalidAddress,                  // Invalid address
    IMAQdxErrorInvalidDeviceType,               // Invalid device type
    IMAQdxErrorNotImplemented,                  // Not implemented
    IMAQdxErrorCameraNotFound,                  // Camera not found
    IMAQdxErrorCameraInUse,                     // Camera is already in use.
    IMAQdxErrorCameraNotInitialized,            // Camera is not initialized.
    IMAQdxErrorCameraRemoved,                   // Camera has been removed.
    IMAQdxErrorCameraRunning,                   // Acquisition in progress.
    IMAQdxErrorCameraNotRunning,                // No acquisition in progress.
    IMAQdxErrorAttributeNotSupported,           // Attribute not supported by the camera.
    IMAQdxErrorAttributeNotSettable,            // Unable to set attribute.
    IMAQdxErrorAttributeNotReadable,            // Unable to get attribute.
    IMAQdxErrorAttributeOutOfRange,             // Attribute value is out of range.
    IMAQdxErrorBufferNotAvailable,              // Requested buffer is unavailable.
    IMAQdxErrorBufferListEmpty,                 // Buffer list is empty. Add one or more buffers.
    IMAQdxErrorBufferListLocked,                // Buffer list is already locked. Reconfigure acquisition and try again.
    IMAQdxErrorBufferListNotLocked,             // No buffer list. Reconfigure acquisition and try again.
    IMAQdxErrorResourcesAllocated,              // Transfer engine resources already allocated. Reconfigure acquisition and try again.
    IMAQdxErrorResourcesUnavailable,            // Insufficient transfer engine resources.
    IMAQdxErrorAsyncWrite,                      // Unable to perform asychronous register write.
    IMAQdxErrorAsyncRead,                       // Unable to perform asychronous register read.
    IMAQdxErrorTimeout,                         // Timeout.
    IMAQdxErrorBusReset,                        // Bus reset occurred during a transaction.
    IMAQdxErrorInvalidXML,                      // Unable to load camera's XML file.
    IMAQdxErrorFileAccess,                      // Unable to read/write to file.
    IMAQdxErrorInvalidCameraURLString,          // Camera has malformed URL string.
    IMAQdxErrorInvalidCameraFile,               // Invalid camera file.
    IMAQdxErrorGenICamError,                    // Unknown Genicam error.
    IMAQdxErrorFormat7Parameters,               // For format 7: The combination of speed, image position, image size, and color coding is incorrect.
    IMAQdxErrorInvalidAttributeType,            // The attribute type is not compatible with the passed variable type.
    IMAQdxErrorDLLNotFound,                     // The DLL could not be found.
    IMAQdxErrorFunctionNotFound,                // The function could not be found.
    IMAQdxErrorLicenseNotActivated,             // License not activated.
    IMAQdxErrorCameraNotConfiguredForListener,  // The camera is not configured properly to support a listener.
    IMAQdxErrorCameraMulticastNotAvailable,     // Unable to configure the system for multicast support.
    IMAQdxErrorBufferHasLostPackets,            // The requested buffer has lost packets and the user requested an error to be generated.
    IMAQdxErrorGiGEVisionError,                 // Unknown GiGE Vision error.
    IMAQdxErrorNetworkError,                    // Unknown network error.
    IMAQdxErrorCameraUnreachable,               // Unable to connect to the camera.
    IMAQdxErrorHighPerformanceNotSupported,     // High performance acquisition is not supported on the specified network interface. Connect the camera to a network interface running the high performance driver.
    IMAQdxErrorInterfaceNotRenamed,             // Unable to rename interface. Invalid or duplicate name specified.
    IMAQdxErrorNoSupportedVideoModes,           // The camera does not have any video modes which are supported.
    IMAQdxErrorSoftwareTriggerOverrun,          // Software trigger overrun.
    IMAQdxErrorTestPacketNotReceived,           // The system did not receive a test packet from the camera. The packet size may be too large for the network configuration or a firewall may be enabled.
    IMAQdxErrorCorruptedImageReceived,          // The camera returned a corrupted image.
    IMAQdxErrorCameraConfigurationHasChanged,   // The camera did not return an image of the correct type it was configured for previously.
    IMAQdxErrorCameraInvalidAuthentication,     // The camera is configured with password authentication and either the user name and password were not configured or they are incorrect.
    IMAQdxErrorUnknownHTTPError,                // The camera returned an unknown HTTP error.
    IMAQdxErrorKernelDriverUnavailable,         // Unable to attach to the kernel mode driver.
    IMAQdxErrorPixelFormatDecoderUnavailable,   // No decoder available for selected pixel format.
    IMAQdxErrorFirmwareUpdateNeeded,            // The acquisition hardware needs a firmware update before it can be used.
    IMAQdxErrorFirmwareUpdateRebootNeeded,      // The firmware on the acquisition hardware has been updated and the system must be rebooted before use.
    IMAQdxErrorLightingCurrentOutOfRange,       // The requested current level from the lighting controller is not possible.
    IMAQdxErrorUSB3VisionError,                 // Unknown USB3 Vision error.
    IMAQdxErrorInvalidU3VUSBDescriptor,         // The camera has a USB descriptor that is incompatible with the USB3 Vision specification.
    IMAQdxErrorU3VInvalidControlInterface,      // The USB3 Vision control interface is not implemented or is invalid on this camera.
    IMAQdxErrorU3VControlInterfaceError,        // There was an error from the control interface of the USB3 Vision camera.
    IMAQdxErrorU3VInvalidEventInterface,        // The USB3 Vision event interface is not implemented or is invalid on this camera.
    IMAQdxErrorU3VEventInterfaceError,          // There was an error from the event interface of the USB3 Vision camera.
    IMAQdxErrorU3VInvalidStreamInterface,       // The USB3 Vision stream interface is not implemented or is invalid on this camera.
    IMAQdxErrorU3VStreamInterfaceError,         // There was an error from the stream interface of the USB3 Vision camera.
    IMAQdxErrorU3VUnsupportedConnectionSpeed,   // The USB connection speed is not supported by the camera.  Check whether the camera is plugged into a USB 2.0 port instead of a USB 3.0 port.  If so, verify that the camera supports this use case.       
    IMAQdxErrorU3VInsufficientPower,            // The USB3 Vision camera requires more current than can be supplied by the USB port in use.
    IMAQdxErrorU3VInvalidMaxCurrent,            // The U3V_MaximumCurrentUSB20_mA registry value is not valid for the connected USB3 Vision camera.
    IMAQdxErrorBufferIncompleteData,            // The requested buffer has incomplete data and the user requested an error to be generated.
    IMAQdxErrorCameraAcquisitionConfigFailed,   // The camera returned an error starting the acquisition.
    IMAQdxErrorCameraClosePending,              // The camera still has outstanding references and will be closed when these operations complete.
    IMAQdxErrorSoftwareFault,                   // An unexpected software error occurred.
    IMAQdxErrorCameraPropertyInvalid,           // The value for an invalid camera property was requested.
    IMAQdxErrorJumboFramesNotEnabled,           // Jumbo frames are not enabled on the host.  Maximum packet size is 1500 bytes.
    IMAQdxErrorBayerPixelFormatNotSelected,     // This operation requires that the camera has a Bayer pixel format selected.
    IMAQdxErrorGuard = 0xFFFFFFFF,
} IMAQdxError;


//==============================================================================
//  Bus Type Enumeration
//==============================================================================
typedef enum IMAQdxBusType_enum {
    IMAQdxBusTypeFireWire = 0x31333934,
    IMAQdxBusTypeEthernet = 0x69707634,
    IMAQdxBusTypeSimulator = 0x2073696D,
    IMAQdxBusTypeDirectShow = 0x64736877,
    IMAQdxBusTypeIP = 0x4950636D,
    IMAQdxBusTypeSmartCam2 = 0x53436132,
    IMAQdxBusTypeUSB3Vision = 0x55534233,
    IMAQdxBusTypeUVC = 0x55564320,
    IMAQdxBusTypeGuard = 0xFFFFFFFF,
} IMAQdxBusType;


//==============================================================================
//  Camera Control Mode Enumeration
//==============================================================================
typedef enum IMAQdxCameraControlMode_enum {
    IMAQdxCameraControlModeController,
    IMAQdxCameraControlModeListener,
    IMAQdxCameraControlModeGuard = 0xFFFFFFFF,
} IMAQdxCameraControlMode;


//==============================================================================
//  Buffer Number Mode Enumeration
//==============================================================================
typedef enum IMAQdxBufferNumberMode_enum {
    IMAQdxBufferNumberModeNext,
    IMAQdxBufferNumberModeLast,
    IMAQdxBufferNumberModeBufferNumber,
    IMAQdxBufferNumberModeGuard = 0xFFFFFFFF,
} IMAQdxBufferNumberMode;


//==============================================================================
//  Plug n Play Event Enumeration
//==============================================================================
typedef enum IMAQdxPnpEvent_enum {
    IMAQdxPnpEventCameraAttached,
    IMAQdxPnpEventCameraDetached,
    IMAQdxPnpEventBusReset,
    IMAQdxPnpEventGuard = 0xFFFFFFFF,
} IMAQdxPnpEvent;


//==============================================================================
//  Bayer Pattern Enumeration
//==============================================================================
typedef enum IMAQdxBayerPattern_enum {
    IMAQdxBayerPatternNone,
    IMAQdxBayerPatternGB,
    IMAQdxBayerPatternGR,
    IMAQdxBayerPatternBG,
    IMAQdxBayerPatternRG,
    IMAQdxBayerPatternHardware,
    IMAQdxBayerPatternGuard = 0xFFFFFFFF,
} IMAQdxBayerPattern;


//==============================================================================
//  Bayer Decode Algorithm Enumeration
//==============================================================================
typedef enum IMAQdxBayerAlgorithm_enum {
    IMAQdxBayerAlgorithmBilinear,
    IMAQdxBayerAlgorithmVNG,
    IMAQdxBayerAlgorithmGuard = 0xFFFFFFFF,
} IMAQdxBayerAlgorithm;


//==============================================================================
//  Output Image Types -- Values match Vision Development Module image types
//==============================================================================
typedef enum IMAQdxOutputImageType_enum {
    IMAQdxOutputImageTypeU8    = 0,
    IMAQdxOutputImageTypeI16   = 1,
    IMAQdxOutputImageTypeU16   = 7,
    IMAQdxOutputImageTypeRGB32 = 4,
    IMAQdxOutputImageTypeRGB64 = 6,
    IMAQdxOutputImageTypeAuto  = 0x7FFFFFFF,
    IMAQdxOutputImageTypeGuard = 0xFFFFFFFF,
} IMAQdxOutputImageType;


//==============================================================================
//  Controller Destination Mode Enumeration
//==============================================================================
typedef enum IMAQdxDestinationMode_enum {
    IMAQdxDestinationModeUnicast,
    IMAQdxDestinationModeBroadcast,
    IMAQdxDestinationModeMulticast,
    IMAQdxDestinationModeGuard = 0xFFFFFFFF,
} IMAQdxDestinationMode;


//==============================================================================
//   Attribute Type Enumeration
//==============================================================================
typedef enum IMAQdxAttributeType_enum {    
    IMAQdxAttributeTypeU32,
    IMAQdxAttributeTypeI64,
    IMAQdxAttributeTypeF64,
    IMAQdxAttributeTypeString,
    IMAQdxAttributeTypeEnum,
    IMAQdxAttributeTypeBool,
    IMAQdxAttributeTypeCommand,
    IMAQdxAttributeTypeBlob,
    IMAQdxAttributeTypeGuard = 0xFFFFFFFF,
} IMAQdxAttributeType;


//==============================================================================
//  Value Type Enumeration
//==============================================================================
typedef enum IMAQdxValueType_enum {    
    IMAQdxValueTypeU32,
    IMAQdxValueTypeI64,
    IMAQdxValueTypeF64,
    IMAQdxValueTypeString,
    IMAQdxValueTypeEnumItem,
    IMAQdxValueTypeBool,
    IMAQdxValueTypeDisposableString,
    IMAQdxValueTypeGuard = 0xFFFFFFFF,
} IMAQdxValueType;


//==============================================================================
//  Interface File Flags Enumeration
//==============================================================================
typedef enum IMAQdxInterfaceFileFlags_enum {
    IMAQdxInterfaceFileFlagsConnected = 0x1,
    IMAQdxInterfaceFileFlagsDirty = 0x2,
    IMAQdxInterfaceFileFlagsGuard = 0xFFFFFFFF,
} IMAQdxInterfaceFileFlags;


//==============================================================================
//  Overwrite Mode Enumeration
//==============================================================================
typedef enum IMAQdxOverwriteMode_enum {
    IMAQdxOverwriteModeGetOldest = 0x0,
    IMAQdxOverwriteModeFail = 0x2,
    IMAQdxOverwriteModeGetNewest = 0x3,
    IMAQdxOverwriteModeGuard = 0xFFFFFFFF,
} IMAQdxOverwriteMode;


//==============================================================================
//  Incomplete Buffer Mode Enumeration
//==============================================================================
typedef enum IMAQdxIncompleteBufferMode_enum {
    IMAQdxIncompleteBufferModeIgnore,
    IMAQdxIncompleteBufferModeFail,
    IMAQdxIncompleteBufferModeGuard = 0xFFFFFFFF,
} IMAQdxIncompleteBufferMode;


//==============================================================================
//  Lost Packet Mode Enumeration
//==============================================================================
typedef enum IMAQdxLostPacketMode_enum {
    IMAQdxLostPacketModeIgnore,
    IMAQdxLostPacketModeFail,
    IMAQdxLostPacketModeGuard = 0xFFFFFFFF,
} IMAQdxLostPacketMode;


//==============================================================================
//  Attribute Visibility Enumeration
//==============================================================================
typedef enum IMAQdxAttributeVisibility_enum {
    IMAQdxAttributeVisibilitySimple = 0x00001000,
    IMAQdxAttributeVisibilityIntermediate = 0x00002000,
    IMAQdxAttributeVisibilityAdvanced = 0x00004000,
    IMAQdxAttributeVisibilityGuard = 0xFFFFFFFF,
} IMAQdxAttributeVisibility;


//==============================================================================
//  Stream Channel Mode Enumeration
//==============================================================================
typedef enum IMAQdxStreamChannelMode_enum {
    IMAQdxStreamChannelModeAutomatic,
    IMAQdxStreamChannelModeManual,
    IMAQdxStreamChannelModeGuard = 0xFFFFFFFF,
} IMAQdxStreamChannelMode;


//==============================================================================
//  Pixel Signedness Enumeration
//==============================================================================
typedef enum IMAQdxPixelSignedness_enum {
    IMAQdxPixelSignednessUnsigned,
    IMAQdxPixelSignednessSigned,
    IMAQdxPixelSignednessHardware,
    IMAQdxPixelSignednessGuard = 0xFFFFFFFF,
} IMAQdxPixelSignedness;


//==============================================================================
//  USB Connection Speed Enumeration
//==============================================================================
typedef enum IMAQdxUSBConnectionSpeed_enum {
    IMAQdxUSBConnectionSpeedLow = 1,
    IMAQdxUSBConnectionSpeedFull = 2,
    IMAQdxUSBConnectionSpeedHigh = 4,
    IMAQdxUSBConnectionSpeedSuper = 8,
    IMAQdxUSBConnectionSpeedGuard = 0xFFFFFFFF,
} IMAQdxUSBConnectionSpeed;


//==============================================================================
//  CVI Structures
//==============================================================================
#pragma pack(push, 4)


//==============================================================================
//  Camera Information Structure
//==============================================================================
typedef struct IMAQdxCameraInformation_struct {
    uInt32 Type;
    uInt32 Version;
    uInt32 Flags;
    uInt32 SerialNumberHi;
    uInt32 SerialNumberLo;
    IMAQdxBusType BusType;
    char InterfaceName[IMAQDX_MAX_API_STRING_LENGTH];
    char VendorName[IMAQDX_MAX_API_STRING_LENGTH];
    char ModelName[IMAQDX_MAX_API_STRING_LENGTH];
    char CameraFileName[IMAQDX_MAX_API_STRING_LENGTH];
    char CameraAttributeURL[IMAQDX_MAX_API_STRING_LENGTH];
} IMAQdxCameraInformation;


//==============================================================================
//  Camera File Structure
//==============================================================================
typedef struct IMAQdxCameraFile_struct {
    uInt32 Type;
    uInt32 Version;
    char FileName[IMAQDX_MAX_API_STRING_LENGTH];
} IMAQdxCameraFile;


//==============================================================================
//  Attribute Information Structure
//==============================================================================
typedef struct IMAQdxAttributeInformation_struct {
    IMAQdxAttributeType Type;
    bool32 Readable;
    bool32 Writable;
    char Name[IMAQDX_MAX_API_STRING_LENGTH];
} IMAQdxAttributeInformation;


//==============================================================================
//  Enumeration Item Structure
//==============================================================================
typedef struct IMAQdxEnumItem_struct {
    uInt32 Value;
    uInt32 Reserved;
    char Name[IMAQDX_MAX_API_STRING_LENGTH];
} IMAQdxEnumItem;


//==============================================================================
//  Camera Information Structure
//==============================================================================
typedef IMAQdxEnumItem IMAQdxVideoMode;


#pragma pack(pop)


//==============================================================================
//  Callbacks
//==============================================================================
typedef     uInt32 (NI_FUNC *FrameDoneEventCallbackPtr)(IMAQdxSession id, uInt32 bufferNumber, void* callbackData);
typedef     uInt32 (NI_FUNC *PnpEventCallbackPtr)(IMAQdxSession id, IMAQdxPnpEvent pnpEvent, void* callbackData);
typedef     void (NI_FUNC *AttributeUpdatedEventCallbackPtr)(IMAQdxSession id, const char* name, void* callbackData);

#endif //niimaqdx_types
//==============================================================================
//  Attributes
//==============================================================================
#define IMAQdxAttributeBaseAddress               "CameraInformation::BaseAddress"         // Read only. Gets the base address of the camera registers.
#define IMAQdxAttributeBusType                   "CameraInformation::BusType"             // Read only. Gets the bus type of the camera.
#define IMAQdxAttributeModelName                 "CameraInformation::ModelName"           // Read only. Returns the model name.
#define IMAQdxAttributeSerialNumberHigh          "CameraInformation::SerialNumberHigh"    // Read only. Gets the upper 32-bits of the camera 64-bit serial number.
#define IMAQdxAttributeSerialNumberLow           "CameraInformation::SerialNumberLow"     // Read only. Gets the lower 32-bits of the camera 64-bit serial number.
#define IMAQdxAttributeVendorName                "CameraInformation::VendorName"          // Read only. Returns the vendor name.
#define IMAQdxAttributeHostIPAddress             "CameraInformation::HostIPAddress"       // Read only. Returns the host adapter IP address.
#define IMAQdxAttributeIPAddress                 "CameraInformation::IPAddress"           // Read only. Returns the IP address.
#define IMAQdxAttributePrimaryURLString          "CameraInformation::PrimaryURLString"    // Read only. Gets the camera's primary URL string.
#define IMAQdxAttributeSecondaryURLString        "CameraInformation::SecondaryURLString"  // Read only. Gets the camera's secondary URL string.
#define IMAQdxAttributeAcqInProgress             "StatusInformation::AcqInProgress"       // Read only. Gets the current state of the acquisition. TRUE if acquiring; otherwise FALSE.
#define IMAQdxAttributeLastBufferCount           "StatusInformation::LastBufferCount"     // Read only. Gets the number of transferred buffers.
#define IMAQdxAttributeLastBufferNumber          "StatusInformation::LastBufferNumber"    // Read only. Gets the last cumulative buffer number transferred.
#define IMAQdxAttributeLostBufferCount           "StatusInformation::LostBufferCount"     // Read only. Gets the number of lost buffers during an acquisition session.
#define IMAQdxAttributeLostPacketCount           "StatusInformation::LostPacketCount"     // Read only. Gets the number of lost packets during an acquisition session.
#define IMAQdxAttributeRequestedResendPackets    "StatusInformation::RequestedResendPacketCount" // Read only. Gets the number of packets requested to be resent during an acquisition session.
#define IMAQdxAttributeReceivedResendPackets     "StatusInformation::ReceivedResendPackets" // Read only. Gets the number of packets that were requested to be resent during an acquisition session and were completed.
#define IMAQdxAttributeHandledEventCount         "StatusInformation::HandledEventCount"   // Read only. Gets the number of handled events during an acquisition session.
#define IMAQdxAttributeLostEventCount            "StatusInformation::LostEventCount"      // Read only. Gets the number of lost events during an acquisition session.
#define IMAQdxAttributeBayerGainB                "AcquisitionAttributes::Bayer::GainB"    // Sets/gets the white balance gain for the blue component of the Bayer conversion.
#define IMAQdxAttributeBayerGainG                "AcquisitionAttributes::Bayer::GainG"    // Sets/gets the white balance gain for the green component of the Bayer conversion.
#define IMAQdxAttributeBayerGainR                "AcquisitionAttributes::Bayer::GainR"    // Sets/gets the white balance gain for the red component of the Bayer conversion.
#define IMAQdxAttributeBayerPattern              "AcquisitionAttributes::Bayer::Pattern"  // Sets/gets the Bayer pattern to use.
#define IMAQdxAttributeStreamChannelMode         "AcquisitionAttributes::Controller::StreamChannelMode" // Gets/sets the mode for allocating a FireWire stream channel.
#define IMAQdxAttributeDesiredStreamChannel      "AcquisitionAttributes::Controller::DesiredStreamChannel" // Gets/sets the stream channel to manually allocate.
#define IMAQdxAttributeFrameInterval             "AcquisitionAttributes::FrameInterval"   // Read only. Gets the duration in milliseconds between successive frames.
#define IMAQdxAttributeIgnoreFirstFrame          "AcquisitionAttributes::IgnoreFirstFrame" // Gets/sets the video delay of one frame between starting the camera and receiving the video feed.
#define IMAQdxAttributeOffsetX                   "OffsetX"                                // Gets/sets the left offset of the image.
#define IMAQdxAttributeOffsetY                   "OffsetY"                                // Gets/sets the top offset of the image.
#define IMAQdxAttributeWidth                     "Width"                                  // Gets/sets the width of the image.
#define IMAQdxAttributeHeight                    "Height"                                 // Gets/sets the height of the image.
#define IMAQdxAttributePixelFormat               "PixelFormat"                            // Gets/sets the pixel format of the source sensor.
#define IMAQdxAttributePacketSize                "PacketSize"                             // Gets/sets the packet size in bytes.
#define IMAQdxAttributePayloadSize               "PayloadSize"                            // Gets/sets the frame size in bytes.
#define IMAQdxAttributeSpeed                     "AcquisitionAttributes::Speed"           // Gets/sets the transfer speed in Mbps for a FireWire packet.
#define IMAQdxAttributeShiftPixelBits            "AcquisitionAttributes::ShiftPixelBits"  // Gets/sets the alignment of 16-bit cameras. Downshift the pixel bits if the camera returns most significant bit-aligned data.
#define IMAQdxAttributeSwapPixelBytes            "AcquisitionAttributes::SwapPixelBytes"  // Gets/sets the endianness of 16-bit cameras. Swap the pixel bytes if the camera returns little endian data.
#define IMAQdxAttributeOverwriteMode             "AcquisitionAttributes::OverwriteMode"   // Gets/sets the overwrite mode, used to determine acquisition when an image transfer cannot be completed due to an overwritten internal buffer.
#define IMAQdxAttributeTimeout                   "AcquisitionAttributes::Timeout"         // Gets/sets the timeout value in milliseconds, used to abort an acquisition when the image transfer cannot be completed within the delay.
#define IMAQdxAttributeVideoMode                 "AcquisitionAttributes::VideoMode"       // Gets/sets the video mode for a camera.
#define IMAQdxAttributeBitsPerPixel              "AcquisitionAttributes::BitsPerPixel"    // Gets/sets the actual bits per pixel. For 16-bit components, this represents the actual bit depth (10-, 12-, 14-, or 16-bit).
#define IMAQdxAttributePixelSignedness           "AcquisitionAttributes::PixelSignedness" // Gets/sets the signedness of the pixel. For 16-bit components, this represents the actual pixel signedness (Signed, or Unsigned).
#define IMAQdxAttributeReserveDualPackets        "AcquisitionAttributes::ReserveDualPackets" // Gets/sets if dual packets will be reserved for a very large FireWire packet.
#define IMAQdxAttributeReceiveTimestampMode      "AcquisitionAttributes::ReceiveTimestampMode" // Gets/sets the mode for timestamping images received by the driver.
#define IMAQdxAttributeActualPeakBandwidth       "AcquisitionAttributes::AdvancedEthernet::BandwidthControl::ActualPeakBandwidth" // Read only. Returns the actual maximum peak bandwidth the camera will be configured to use.
#define IMAQdxAttributeDesiredPeakBandwidth      "AcquisitionAttributes::AdvancedEthernet::BandwidthControl::DesiredPeakBandwidth" // Gets/sets the desired maximum peak bandwidth the camera should use.
#define IMAQdxAttributeDestinationMode           "AcquisitionAttributes::AdvancedEthernet::Controller::DestinationMode" // Gets/Sets where the camera is instructed to send the image stream.
#define IMAQdxAttributeDestinationMulticastAddress "AcquisitionAttributes::AdvancedEthernet::Controller::DestinationMulticastAddress" // Gets/Sets the multicast address the camera should send data in multicast mode.
#define IMAQdxAttributeEventsEnabled             "AcquisitionAttributes::AdvancedEthernet::EventParameters::EventsEnabled" // Gets/Sets if events will be handled.
#define IMAQdxAttributeMaxOutstandingEvents      "AcquisitionAttributes::AdvancedEthernet::EventParameters::MaxOutstandingEvents" // Gets/Sets the maximum number of outstanding events to queue.
#define IMAQdxAttributeTestPacketEnabled         "AcquisitionAttributes::AdvancedEthernet::TestPacketParameters::TestPacketEnabled" // Gets/Sets whether the driver will validate the image streaming settings using test packets prior to an acquisition
#define IMAQdxAttributeTestPacketTimeout         "AcquisitionAttributes::AdvancedEthernet::TestPacketParameters::TestPacketTimeout" // Gets/Sets the timeout for validating test packet reception (if enabled)
#define IMAQdxAttributeMaxTestPacketRetries      "AcquisitionAttributes::AdvancedEthernet::TestPacketParameters::MaxTestPacketRetries" // Gets/Sets the number of retries for validating test packet reception (if enabled)
#define IMAQdxAttributeChunkDataDecodingEnabled  "AcquisitionAttributes::ChunkDataDecoding::ChunkDataDecodingEnabled" // Gets/Sets whether the driver will decode any chunk data in the image stream
#define IMAQdxAttributeChunkDataDecodingMaxElementSize "AcquisitionAttributes::ChunkDataDecoding::MaximumChunkCopySize" // Gets/Sets the maximum size of any single chunk data element that will be made available
#define IMAQdxAttributeLostPacketMode            "AcquisitionAttributes::AdvancedEthernet::LostPacketMode" // Gets/sets the behavior when the user extracts a buffer that has missing packets.
#define IMAQdxAttributeMemoryWindowSize          "AcquisitionAttributes::AdvancedEthernet::ResendParameters::MemoryWindowSize" // Gets/sets the size of the memory window of the camera in kilobytes. Should match the camera's internal buffer size.
#define IMAQdxAttributeResendsEnabled            "AcquisitionAttributes::AdvancedEthernet::ResendParameters::ResendsEnabled" // Gets/sets if resends will be issued for missing packets.
#define IMAQdxAttributeResendThresholdPercentage "AcquisitionAttributes::AdvancedEthernet::ResendParameters::ResendThresholdPercentage" // Gets/sets the threshold of the packet processing window that will trigger packets to be resent.
#define IMAQdxAttributeResendBatchingPercentage  "AcquisitionAttributes::AdvancedEthernet::ResendParameters::ResendBatchingPercentage" // Gets/sets the percent of the packet resend threshold that will be issued as one group past the initial threshold sent in a single request.
#define IMAQdxAttributeMaxResendsPerPacket       "AcquisitionAttributes::AdvancedEthernet::ResendParameters::MaxResendsPerPacket" // Gets/sets the maximum number of resend requests that will be issued for a missing packet.
#define IMAQdxAttributeResendResponseTimeout     "AcquisitionAttributes::AdvancedEthernet::ResendParameters::ResendResponseTimeout" // Gets/sets the time to wait for a resend request to be satisfied before sending another.
#define IMAQdxAttributeNewPacketTimeout          "AcquisitionAttributes::AdvancedEthernet::ResendParameters::NewPacketTimeout" // Gets/sets the time to wait for new packets to arrive in a partially completed image before assuming the rest of the image was lost.
#define IMAQdxAttributeMissingPacketTimeout      "AcquisitionAttributes::AdvancedEthernet::ResendParameters::MissingPacketTimeout" // Gets/sets the time to wait for a missing packet before issuing a resend.
#define IMAQdxAttributeResendTimerResolution     "AcquisitionAttributes::AdvancedEthernet::ResendParameters::ResendTimerResolution" // Gets/sets the resolution of the packet processing system that is used for all packet-related timeouts.


//==============================================================================
//  Functions
//==============================================================================
IMAQdxError NI_FUNC IMAQdxSnap(IMAQdxSession id, Image* image);
IMAQdxError NI_FUNC IMAQdxConfigureGrab(IMAQdxSession id);
IMAQdxError NI_FUNC IMAQdxGrab(IMAQdxSession id, Image* image, bool32 waitForNextBuffer, uInt32* actualBufferNumber);
IMAQdxError NI_FUNC IMAQdxSequence(IMAQdxSession id, Image* images[], uInt32 count);
IMAQdxError NI_FUNC IMAQdxDiscoverEthernetCameras(const char* address, uInt32 timeout);
IMAQdxError NI_FUNC IMAQdxEnumerateCameras(IMAQdxCameraInformation cameraInformationArray[], uInt32* count, bool32 connectedOnly);
IMAQdxError NI_FUNC IMAQdxResetCamera(const char* name, bool32 resetAll);
IMAQdxError NI_FUNC IMAQdxOpenCamera(const char* name, IMAQdxCameraControlMode mode, IMAQdxSession* id);
IMAQdxError NI_FUNC IMAQdxCloseCamera(IMAQdxSession id);
IMAQdxError NI_FUNC IMAQdxConfigureAcquisition(IMAQdxSession id, bool32 continuous, uInt32 bufferCount);
IMAQdxError NI_FUNC IMAQdxStartAcquisition(IMAQdxSession id);
IMAQdxError NI_FUNC IMAQdxGetImage(IMAQdxSession id, Image* image, IMAQdxBufferNumberMode mode, uInt32 desiredBufferNumber, uInt32* actualBufferNumber);
IMAQdxError NI_FUNC IMAQdxGetImageData(IMAQdxSession id, void* buffer, uInt32 bufferSize, IMAQdxBufferNumberMode mode, uInt32 desiredBufferNumber, uInt32* actualBufferNumber);
IMAQdxError NI_FUNC IMAQdxStopAcquisition(IMAQdxSession id);
IMAQdxError NI_FUNC IMAQdxUnconfigureAcquisition(IMAQdxSession id);
IMAQdxError NI_FUNC IMAQdxEnumerateVideoModes(IMAQdxSession id, IMAQdxVideoMode videoModeArray[], uInt32* count, uInt32* currentMode);
IMAQdxError NI_FUNC IMAQdxEnumerateAttributes(IMAQdxSession id, IMAQdxAttributeInformation attributeInformationArray[], uInt32* count, const char* root);
IMAQdxError NI_FUNC IMAQdxGetAttribute(IMAQdxSession id, const char* name, IMAQdxValueType type, void* value);
IMAQdxError NI_FUNCC IMAQdxSetAttribute(IMAQdxSession id, const char* name, IMAQdxValueType type, ...);
IMAQdxError NI_FUNC IMAQdxGetAttributeMinimum(IMAQdxSession id, const char* name, IMAQdxValueType type, void* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeMaximum(IMAQdxSession id, const char* name, IMAQdxValueType type, void* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeIncrement(IMAQdxSession id, const char* name, IMAQdxValueType type, void* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeType(IMAQdxSession id, const char* name, IMAQdxAttributeType* type);
IMAQdxError NI_FUNC IMAQdxIsAttributeReadable(IMAQdxSession id, const char* name, bool32* readable);
IMAQdxError NI_FUNC IMAQdxIsAttributeWritable(IMAQdxSession id, const char* name, bool32* writable);
IMAQdxError NI_FUNC IMAQdxEnumerateAttributeValues(IMAQdxSession id, const char* name, IMAQdxEnumItem list[], uInt32* size);
IMAQdxError NI_FUNC IMAQdxGetAttributeTooltip(IMAQdxSession id, const char* name, char* tooltip, uInt32 length);
IMAQdxError NI_FUNC IMAQdxGetAttributeUnits(IMAQdxSession id, const char* name, char* units, uInt32 length);
IMAQdxError NI_FUNC IMAQdxRegisterFrameDoneEvent(IMAQdxSession id, uInt32 bufferInterval, FrameDoneEventCallbackPtr callbackFunction, void* callbackData);
IMAQdxError NI_FUNC IMAQdxRegisterPnpEvent(IMAQdxSession id, IMAQdxPnpEvent event, PnpEventCallbackPtr callbackFunction, void* callbackData);
IMAQdxError NI_FUNC IMAQdxWriteRegister(IMAQdxSession id, uInt32 offset, uInt32 value);
IMAQdxError NI_FUNC IMAQdxReadRegister(IMAQdxSession id, uInt32 offset, uInt32* value);
IMAQdxError NI_FUNC IMAQdxWriteMemory(IMAQdxSession id, uInt32 offset, const char* values, uInt32 count);
IMAQdxError NI_FUNC IMAQdxReadMemory(IMAQdxSession id, uInt32 offset, char* values, uInt32 count);
IMAQdxError NI_FUNC IMAQdxGetErrorString(IMAQdxError error, char* message, uInt32 messageLength);
IMAQdxError NI_FUNC IMAQdxWriteAttributes(IMAQdxSession id, const char* filename);
IMAQdxError NI_FUNC IMAQdxReadAttributes(IMAQdxSession id, const char* filename);
IMAQdxError NI_FUNC IMAQdxResetEthernetCameraAddress(const char* name, const char* address, const char* subnet, const char* gateway, uInt32 timeout);
IMAQdxError NI_FUNC IMAQdxEnumerateAttributes2(IMAQdxSession id, IMAQdxAttributeInformation attributeInformationArray[], uInt32* count, const char* root, IMAQdxAttributeVisibility visibility);
IMAQdxError NI_FUNC IMAQdxGetAttributeVisibility(IMAQdxSession id, const char* name, IMAQdxAttributeVisibility* visibility);
IMAQdxError NI_FUNC IMAQdxGetAttributeDescription(IMAQdxSession id, const char* name, char* description, uInt32 length);
IMAQdxError NI_FUNC IMAQdxGetAttributeDisplayName(IMAQdxSession id, const char* name, char* displayName, uInt32 length);
IMAQdxError NI_FUNC IMAQdxDispose(void* buffer);
IMAQdxError NI_FUNC IMAQdxRegisterAttributeUpdatedEvent(IMAQdxSession id, const char* name, AttributeUpdatedEventCallbackPtr callbackFunction, void* callbackData);
IMAQdxError NI_FUNC IMAQdxEnumerateAttributes3(IMAQdxSession id, IMAQdxAttributeInformation attributeInformationArray[], uInt32* count, const char* root, IMAQdxAttributeVisibility visibility);


#ifdef __cplusplus
}
#endif


#endif // ___niimaqdx_h___
