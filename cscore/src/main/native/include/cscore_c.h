/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CSCORE_C_H_
#define CSCORE_CSCORE_C_H_

#include <stdint.h>

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct CvMat;

/**
 * @defgroup cscore_c_api cscore C API
 *
 * Handle-based interface for C.
 *
 * <p>Sources and sinks are reference counted internally to the library.
 * Any time a source or sink handle is returned or provided to a callback,
 * the reference count is incremented.
 *
 * <p>Calling CS_ReleaseSource() or CS_ReleaseSink() decrements the reference
 * count, and when the reference count reaches zero, the object is destroyed.
 *
 * <p>Connecting a source to a sink increments the reference count of the
 * source, and when the sink is destroyed (its reference count reaches zero),
 * the source reference count is decremented.
 *
 * @{
 */

/**
 * @defgroup cscore_typedefs Typedefs
 * @{
 */
typedef int CS_Bool;
typedef int CS_Status;

typedef int CS_Handle;
typedef CS_Handle CS_Property;
typedef CS_Handle CS_Listener;
typedef CS_Handle CS_Sink;
typedef CS_Handle CS_Source;
/** @} */

/**
 * Status values
 */
enum CS_StatusValue {
  CS_PROPERTY_WRITE_FAILED = 2000,
  CS_OK = 0,
  CS_INVALID_HANDLE = -2000,  // handle was invalid (does not exist)
  CS_WRONG_HANDLE_SUBTYPE = -2001,
  CS_INVALID_PROPERTY = -2002,
  CS_WRONG_PROPERTY_TYPE = -2003,
  CS_READ_FAILED = -2004,
  CS_SOURCE_IS_DISCONNECTED = -2005,
  CS_EMPTY_VALUE = -2006,
  CS_BAD_URL = -2007,
  CS_TELEMETRY_NOT_ENABLED = -2008,
  CS_UNSUPPORTED_MODE = -2009
};

/**
 * Logging levels
 */
enum CS_LogLevel {
  CS_LOG_CRITICAL = 50,
  CS_LOG_ERROR = 40,
  CS_LOG_WARNING = 30,
  CS_LOG_INFO = 20,
  CS_LOG_DEBUG = 10,
  CS_LOG_DEBUG1 = 9,
  CS_LOG_DEBUG2 = 8,
  CS_LOG_DEBUG3 = 7,
  CS_LOG_DEBUG4 = 6
};

/**
 * Pixel formats
 */
enum CS_PixelFormat {
  CS_PIXFMT_UNKNOWN = 0,
  CS_PIXFMT_MJPEG,
  CS_PIXFMT_YUYV,
  CS_PIXFMT_RGB565,
  CS_PIXFMT_BGR,
  CS_PIXFMT_GRAY
};

/**
 * Video mode
 */
typedef struct CS_VideoMode {
  int pixelFormat;
  int width;
  int height;
  int fps;
} CS_VideoMode;

/**
 * Property kinds
 */
enum CS_PropertyKind {
  CS_PROP_NONE = 0,
  CS_PROP_BOOLEAN = 1,
  CS_PROP_INTEGER = 2,
  CS_PROP_STRING = 4,
  CS_PROP_ENUM = 8
};

/**
 * Source kinds
 */
enum CS_SourceKind {
  CS_SOURCE_UNKNOWN = 0,
  CS_SOURCE_USB = 1,
  CS_SOURCE_HTTP = 2,
  CS_SOURCE_CV = 4
};

/**
 * HTTP Camera kinds
 */
enum CS_HttpCameraKind {
  CS_HTTP_UNKNOWN = 0,
  CS_HTTP_MJPGSTREAMER = 1,
  CS_HTTP_CSCORE = 2,
  CS_HTTP_AXIS = 3
};

/**
 * Sink kinds
 */
enum CS_SinkKind { CS_SINK_UNKNOWN = 0, CS_SINK_MJPEG = 2, CS_SINK_CV = 4 };

/**
 * Listener event kinds
 */
enum CS_EventKind {
  CS_SOURCE_CREATED = 0x0001,
  CS_SOURCE_DESTROYED = 0x0002,
  CS_SOURCE_CONNECTED = 0x0004,
  CS_SOURCE_DISCONNECTED = 0x0008,
  CS_SOURCE_VIDEOMODES_UPDATED = 0x0010,
  CS_SOURCE_VIDEOMODE_CHANGED = 0x0020,
  CS_SOURCE_PROPERTY_CREATED = 0x0040,
  CS_SOURCE_PROPERTY_VALUE_UPDATED = 0x0080,
  CS_SOURCE_PROPERTY_CHOICES_UPDATED = 0x0100,
  CS_SINK_SOURCE_CHANGED = 0x0200,
  CS_SINK_CREATED = 0x0400,
  CS_SINK_DESTROYED = 0x0800,
  CS_SINK_ENABLED = 0x1000,
  CS_SINK_DISABLED = 0x2000,
  CS_NETWORK_INTERFACES_CHANGED = 0x4000,
  CS_TELEMETRY_UPDATED = 0x8000,
  CS_SINK_PROPERTY_CREATED = 0x10000,
  CS_SINK_PROPERTY_VALUE_UPDATED = 0x20000,
  CS_SINK_PROPERTY_CHOICES_UPDATED = 0x40000
};

/**
 * Telemetry kinds
 */
enum CS_TelemetryKind {
  CS_SOURCE_BYTES_RECEIVED = 1,
  CS_SOURCE_FRAMES_RECEIVED = 2
};

/** Connection strategy */
enum CS_ConnectionStrategy {
  /**
   * Automatically connect or disconnect based on whether any sinks are
   * connected to this source.  This is the default behavior.
   */
  CS_CONNECTION_AUTO_MANAGE = 0,

  /**
   * Try to keep the connection open regardless of whether any sinks are
   * connected.
   */
  CS_CONNECTION_KEEP_OPEN,

  /**
   * Never open the connection.  If this is set when the connection is open,
   * close the connection.
   */
  CS_CONNECTION_FORCE_CLOSE
};

/**
 * Listener event
 */
struct CS_Event {
  enum CS_EventKind kind;

  // Valid for CS_SOURCE_* and CS_SINK_* respectively
  CS_Source source;
  CS_Sink sink;

  // Source/sink/property name
  const char* name;

  // Fields for CS_SOURCE_VIDEOMODE_CHANGED event
  CS_VideoMode mode;

  // Fields for CS_SOURCE_PROPERTY_* events
  CS_Property property;
  enum CS_PropertyKind propertyKind;
  int value;
  const char* valueStr;
};

/**
 * USB camera infomation
 */
typedef struct CS_UsbCameraInfo {
  int dev;
  char* path;
  char* name;
  int otherPathsCount;
  char** otherPaths;
} CS_UsbCameraInfo;

/**
 * @defgroup cscore_property_cfunc Property Functions
 * @{
 */
enum CS_PropertyKind CS_GetPropertyKind(CS_Property property,
                                        CS_Status* status);
char* CS_GetPropertyName(CS_Property property, CS_Status* status);
int CS_GetProperty(CS_Property property, CS_Status* status);
void CS_SetProperty(CS_Property property, int value, CS_Status* status);
int CS_GetPropertyMin(CS_Property property, CS_Status* status);
int CS_GetPropertyMax(CS_Property property, CS_Status* status);
int CS_GetPropertyStep(CS_Property property, CS_Status* status);
int CS_GetPropertyDefault(CS_Property property, CS_Status* status);
char* CS_GetStringProperty(CS_Property property, CS_Status* status);
void CS_SetStringProperty(CS_Property property, const char* value,
                          CS_Status* status);
char** CS_GetEnumPropertyChoices(CS_Property property, int* count,
                                 CS_Status* status);
/** @} */

/**
 * @defgroup cscore_source_create_cfunc Source Creation Functions
 * @{
 */
CS_Source CS_CreateUsbCameraDev(const char* name, int dev, CS_Status* status);
CS_Source CS_CreateUsbCameraPath(const char* name, const char* path,
                                 CS_Status* status);
CS_Source CS_CreateHttpCamera(const char* name, const char* url,
                              enum CS_HttpCameraKind kind, CS_Status* status);
CS_Source CS_CreateHttpCameraMulti(const char* name, const char** urls,
                                   int count, enum CS_HttpCameraKind kind,
                                   CS_Status* status);
CS_Source CS_CreateCvSource(const char* name, const CS_VideoMode* mode,
                            CS_Status* status);
/** @} */

/**
 * @defgroup cscore_source_cfunc Source Functions
 * @{
 */
enum CS_SourceKind CS_GetSourceKind(CS_Source source, CS_Status* status);
char* CS_GetSourceName(CS_Source source, CS_Status* status);
char* CS_GetSourceDescription(CS_Source source, CS_Status* status);
uint64_t CS_GetSourceLastFrameTime(CS_Source source, CS_Status* status);
void CS_SetSourceConnectionStrategy(CS_Source source,
                                    enum CS_ConnectionStrategy strategy,
                                    CS_Status* status);
CS_Bool CS_IsSourceConnected(CS_Source source, CS_Status* status);
CS_Bool CS_IsSourceEnabled(CS_Source source, CS_Status* status);
CS_Property CS_GetSourceProperty(CS_Source source, const char* name,
                                 CS_Status* status);
CS_Property* CS_EnumerateSourceProperties(CS_Source source, int* count,
                                          CS_Status* status);
void CS_GetSourceVideoMode(CS_Source source, CS_VideoMode* mode,
                           CS_Status* status);
CS_Bool CS_SetSourceVideoMode(CS_Source source, const CS_VideoMode* mode,
                              CS_Status* status);
CS_Bool CS_SetSourceVideoModeDiscrete(CS_Source source,
                                      enum CS_PixelFormat pixelFormat,
                                      int width, int height, int fps,
                                      CS_Status* status);
CS_Bool CS_SetSourcePixelFormat(CS_Source source,
                                enum CS_PixelFormat pixelFormat,
                                CS_Status* status);
CS_Bool CS_SetSourceResolution(CS_Source source, int width, int height,
                               CS_Status* status);
CS_Bool CS_SetSourceFPS(CS_Source source, int fps, CS_Status* status);
CS_Bool CS_SetSourceConfigJson(CS_Source source, const char* config,
                               CS_Status* status);
char* CS_GetSourceConfigJson(CS_Source source, CS_Status* status);
CS_VideoMode* CS_EnumerateSourceVideoModes(CS_Source source, int* count,
                                           CS_Status* status);
CS_Sink* CS_EnumerateSourceSinks(CS_Source source, int* count,
                                 CS_Status* status);
CS_Source CS_CopySource(CS_Source source, CS_Status* status);
void CS_ReleaseSource(CS_Source source, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_source_prop_cfunc Camera Source Common Property Fuctions
 * @{
 */
void CS_SetCameraBrightness(CS_Source source, int brightness,
                            CS_Status* status);
int CS_GetCameraBrightness(CS_Source source, CS_Status* status);
void CS_SetCameraWhiteBalanceAuto(CS_Source source, CS_Status* status);
void CS_SetCameraWhiteBalanceHoldCurrent(CS_Source source, CS_Status* status);
void CS_SetCameraWhiteBalanceManual(CS_Source source, int value,
                                    CS_Status* status);
void CS_SetCameraExposureAuto(CS_Source source, CS_Status* status);
void CS_SetCameraExposureHoldCurrent(CS_Source source, CS_Status* status);
void CS_SetCameraExposureManual(CS_Source source, int value, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_usbcamera_cfunc UsbCamera Source Functions
 * @{
 */
char* CS_GetUsbCameraPath(CS_Source source, CS_Status* status);
CS_UsbCameraInfo* CS_GetUsbCameraInfo(CS_Source source, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_httpcamera_cfunc HttpCamera Source Functions
 * @{
 */
enum CS_HttpCameraKind CS_GetHttpCameraKind(CS_Source source,
                                            CS_Status* status);
void CS_SetHttpCameraUrls(CS_Source source, const char** urls, int count,
                          CS_Status* status);
char** CS_GetHttpCameraUrls(CS_Source source, int* count, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_opencv_source_cfunc OpenCV Source Functions
 * @{
 */
void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status);
void CS_NotifySourceError(CS_Source source, const char* msg, CS_Status* status);
void CS_SetSourceConnected(CS_Source source, CS_Bool connected,
                           CS_Status* status);
void CS_SetSourceDescription(CS_Source source, const char* description,
                             CS_Status* status);
CS_Property CS_CreateSourceProperty(CS_Source source, const char* name,
                                    enum CS_PropertyKind kind, int minimum,
                                    int maximum, int step, int defaultValue,
                                    int value, CS_Status* status);
void CS_SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                     const char** choices, int count,
                                     CS_Status* status);
/** @} */

/**
 * @defgroup cscore_sink_create_cfunc Sink Creation Functions
 * @{
 */
CS_Sink CS_CreateMjpegServer(const char* name, const char* listenAddress,
                             int port, CS_Status* status);
CS_Sink CS_CreateCvSink(const char* name, CS_Status* status);
CS_Sink CS_CreateCvSinkCallback(const char* name, void* data,
                                void (*processFrame)(void* data, uint64_t time),
                                CS_Status* status);
/** @} */

/**
 * @defgroup cscore_sink_cfunc Sink Functions
 * @{
 */
enum CS_SinkKind CS_GetSinkKind(CS_Sink sink, CS_Status* status);
char* CS_GetSinkName(CS_Sink sink, CS_Status* status);
char* CS_GetSinkDescription(CS_Sink sink, CS_Status* status);
CS_Property CS_GetSinkProperty(CS_Sink sink, const char* name,
                               CS_Status* status);
CS_Property* CS_EnumerateSinkProperties(CS_Sink sink, int* count,
                                        CS_Status* status);
void CS_SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status);
CS_Property CS_GetSinkSourceProperty(CS_Sink sink, const char* name,
                                     CS_Status* status);
CS_Bool CS_SetSinkConfigJson(CS_Sink sink, const char* config,
                             CS_Status* status);
char* CS_GetSinkConfigJson(CS_Sink sink, CS_Status* status);
CS_Source CS_GetSinkSource(CS_Sink sink, CS_Status* status);
CS_Sink CS_CopySink(CS_Sink sink, CS_Status* status);
void CS_ReleaseSink(CS_Sink sink, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_mjpegserver_cfunc MjpegServer Sink Functions
 * @{
 */
char* CS_GetMjpegServerListenAddress(CS_Sink sink, CS_Status* status);
int CS_GetMjpegServerPort(CS_Sink sink, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_opencv_sink_cfunc OpenCV Sink Functions
 * @{
 */
void CS_SetSinkDescription(CS_Sink sink, const char* description,
                           CS_Status* status);
uint64_t CS_GrabSinkFrame(CS_Sink sink, struct CvMat* image, CS_Status* status);
uint64_t CS_GrabSinkFrameTimeout(CS_Sink sink, struct CvMat* image,
                                 double timeout, CS_Status* status);
char* CS_GetSinkError(CS_Sink sink, CS_Status* status);
void CS_SetSinkEnabled(CS_Sink sink, CS_Bool enabled, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_listener_cfunc Listener Functions
 * @{
 */
void CS_SetListenerOnStart(void (*onStart)(void* data), void* data);
void CS_SetListenerOnExit(void (*onExit)(void* data), void* data);
CS_Listener CS_AddListener(
    void* data, void (*callback)(void* data, const struct CS_Event* event),
    int eventMask, int immediateNotify, CS_Status* status);

void CS_RemoveListener(CS_Listener handle, CS_Status* status);
/** @} */

int CS_NotifierDestroyed(void);

/**
 * @defgroup cscore_telemetry_cfunc Telemetry Functions
 * @{
 */
void CS_SetTelemetryPeriod(double seconds);
double CS_GetTelemetryElapsedTime(void);
int64_t CS_GetTelemetryValue(CS_Handle handle, enum CS_TelemetryKind kind,
                             CS_Status* status);
double CS_GetTelemetryAverageValue(CS_Handle handle, enum CS_TelemetryKind kind,
                                   CS_Status* status);
/** @} */

/**
 * @defgroup cscore_logging_cfunc Logging Functions
 * @{
 */
typedef void (*CS_LogFunc)(unsigned int level, const char* file,
                           unsigned int line, const char* msg);
void CS_SetLogger(CS_LogFunc func, unsigned int min_level);
void CS_SetDefaultLogger(unsigned int min_level);
/** @} */

/**
 * @defgroup cscore_shutdown_cfunc Library Shutdown Function
 * @{
 */
void CS_Shutdown(void);
/** @} */

/**
 * @defgroup cscore_utility_cfunc Utility Functions
 * @{
 */

CS_UsbCameraInfo* CS_EnumerateUsbCameras(int* count, CS_Status* status);
void CS_FreeEnumeratedUsbCameras(CS_UsbCameraInfo* cameras, int count);

CS_Source* CS_EnumerateSources(int* count, CS_Status* status);
void CS_ReleaseEnumeratedSources(CS_Source* sources, int count);

CS_Sink* CS_EnumerateSinks(int* count, CS_Status* status);
void CS_ReleaseEnumeratedSinks(CS_Sink* sinks, int count);

void CS_FreeString(char* str);
void CS_FreeEnumPropertyChoices(char** choices, int count);
void CS_FreeUsbCameraInfo(CS_UsbCameraInfo* info);
void CS_FreeHttpCameraUrls(char** urls, int count);

void CS_FreeEnumeratedProperties(CS_Property* properties, int count);
void CS_FreeEnumeratedVideoModes(CS_VideoMode* modes, int count);

char* CS_GetHostname();

char** CS_GetNetworkInterfaces(int* count);
void CS_FreeNetworkInterfaces(char** interfaces, int count);
/** @} */

/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CSCORE_CSCORE_C_H_
