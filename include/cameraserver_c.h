/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_C_H_
#define CAMERASERVER_C_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CvMat;

//
// The C API is handle-based.  Sources and sinks are reference counted
// internally to the library.  Any time a source or sink handle is returned
// or provided to a callback, the reference count is incremented.
// Calling CS_ReleaseSource() or CS_ReleaseSink() decrements the reference
// count, and when the reference count reaches zero, the object is destroyed.
// Connecting a source to a sink increments the reference count of the source,
// and when the sink is destroyed (its reference count reaches zero), the
// source reference count is decremented.
//

//
// Typedefs
//
typedef int CS_Bool;
typedef int CS_Property;
typedef int CS_Listener;
typedef int CS_Sink;
typedef int CS_Source;
typedef int CS_Status;

//
// Property Functions
//

enum CS_PropertyType {
  CS_PROP_NONE = 0,
  CS_PROP_BOOLEAN,
  CS_PROP_DOUBLE,
  CS_PROP_STRING,
  CS_PROP_ENUM
};

enum CS_PropertyType CS_GetPropertyType(CS_Property property,
                                        CS_Status* status);
char* CS_GetPropertyName(CS_Property property, CS_Status* status);
CS_Bool CS_GetBooleanProperty(CS_Property property, CS_Status* status);
void CS_SetBooleanProperty(CS_Property property, CS_Bool value,
                           CS_Status* status);
double CS_GetDoubleProperty(CS_Property property, CS_Status* status);
void CS_SetDoubleProperty(CS_Property property, double value,
                          CS_Status* status);
double CS_GetDoublePropertyMin(CS_Property property, CS_Status* status);
double CS_GetDoublePropertyMax(CS_Property property, CS_Status* status);
char* CS_GetStringProperty(CS_Property property, CS_Status* status);
void CS_SetStringProperty(CS_Property property, const char* value,
                          CS_Status* status);
int CS_GetEnumProperty(CS_Property property, CS_Status* status);
void CS_SetEnumProperty(CS_Property property, int value, CS_Status* status);
char** CS_GetEnumPropertyChoices(CS_Property property, int* count,
                                 CS_Status* status);

//
// Source Creation Functions
//
CS_Source CS_CreateUSBSourceDev(const char* name, int dev, CS_Status* status);
CS_Source CS_CreateUSBSourcePath(const char* name, const char* path,
                                 CS_Status* status);
CS_Source CS_CreateHTTPSource(const char* name, const char* url,
                              CS_Status* status);
CS_Source CS_CreateCvSource(const char* name, int numChannels,
                            CS_Status* status);

//
// Source Functions
//
char* CS_GetSourceName(CS_Source source, CS_Status* status);
char* CS_GetSourceDescription(CS_Source source, CS_Status* status);
uint64_t CS_GetSourceLastFrameTime(CS_Source source, CS_Status* status);
int CS_GetSourceNumChannels(CS_Source source, CS_Status* status);
CS_Bool CS_IsSourceConnected(CS_Source source, CS_Status* status);
CS_Property CS_GetSourceProperty(CS_Source source, const char* name,
                                 CS_Status* status);
CS_Property* CS_EnumerateSourceProperties(CS_Source source, int* count,
                                          CS_Status* status);
CS_Source CS_CopySource(CS_Source source, CS_Status* status);
void CS_ReleaseSource(CS_Source source, CS_Status* status);

//
// OpenCV Source Functions
//
void CS_PutSourceImage(CS_Source source, int channel, struct CvMat* image,
                       CS_Status* status);
void CS_NotifySourceFrame(CS_Source source, CS_Status* status);
void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status);
void CS_NotifySourceError(CS_Source source, const char* msg, CS_Status* status);
void CS_SetSourceConnected(CS_Source source, CS_Bool connected,
                           CS_Status* status);
CS_Property CS_CreateSourceProperty(CS_Source source, const char* name,
                                    enum CS_PropertyType type,
                                    CS_Status* status);
CS_Property CS_CreateSourcePropertyCallback(
    CS_Source source, const char* name, enum CS_PropertyType type, void* data,
    void (*onChange)(void* data, CS_Property property), CS_Status* status);
void CS_RemoveSourceProperty(CS_Source source, CS_Property property,
                             CS_Status* status);
void CS_RemoveSourcePropertyByName(CS_Source source, const char* name,
                                   CS_Status* status);

//
// Sink Creation Functions
//
CS_Sink CS_CreateHTTPSink(const char* name, const char* listenAddress, int port,
                          CS_Status* status);
CS_Sink CS_CreateCvSink(const char* name, CS_Status* status);
CS_Sink CS_CreateCvSinkCallback(const char* name, void* data,
                                void (*processFrame)(void* data, uint64_t time),
                                CS_Status* status);

//
// Sink Functions
//
char* CS_GetSinkName(CS_Sink sink, CS_Status* status);
char* CS_GetSinkDescription(CS_Sink sink, CS_Status* status);
void CS_SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status);
CS_Property CS_GetSinkSourceProperty(CS_Sink sink, const char* name,
                                     CS_Status* status);
CS_Source CS_GetSinkSource(CS_Sink sink, CS_Status* status);
CS_Sink CS_CopySink(CS_Sink sink, CS_Status* status);
void CS_ReleaseSink(CS_Sink sink, CS_Status* status);

//
// Server Sink (e.g. HTTP) Functions
//
void CS_SetSinkSourceChannel(CS_Sink sink, int channel, CS_Status* status);

//
// OpenCV Sink Functions
//
uint64_t CS_SinkWaitForFrame(CS_Sink sink, CS_Status* status);
CS_Bool CS_GetSinkImage(CS_Sink sink, int channel, struct CvMat* image,
                        CS_Status* status);
uint64_t CS_GrabSinkFrame(CS_Sink sink, struct CvMat* image, CS_Status* status);
char* CS_GetSinkError(CS_Sink sink, CS_Status* status);
void CS_SetSinkEnabled(CS_Sink sink, CS_Bool enabled, CS_Status* status);

//
// Listener Functions
//
enum CS_SourceEvent {
  CS_SOURCE_CREATED = 0x01,
  CS_SOURCE_DESTROYED = 0x02,
  CS_SOURCE_CONNECTED = 0x04,
  CS_SOURCE_DISCONNECTED = 0x08
};

CS_Listener CS_AddSourceListener(void* data,
                                 void (*callback)(void* data, const char* name,
                                                  CS_Source source, int event),
                                 int eventMask, CS_Status* status);

void CS_RemoveSourceListener(CS_Listener handle, CS_Status* status);

enum CS_SinkEvent {
  CS_SINK_CREATED = 0x01,
  CS_SINK_DESTROYED = 0x02,
  CS_SINK_ENABLED = 0x04,
  CS_SINK_DISABLED = 0x08
};

CS_Listener CS_AddSinkListener(void* data,
                               void (*callback)(void* data, const char* name,
                                                CS_Sink sink, int event),
                               int eventMask, CS_Status* status);

void CS_RemoveSinkListener(CS_Listener handle, CS_Status* status);

//
// Utility Functions
//
typedef struct CS_USBCameraInfo {
  int dev;
  char* path;
  char* name;
  int channels;
} CS_USBCameraInfo;

CS_USBCameraInfo* CS_EnumerateUSBCameras(int* count, CS_Status* status);
void CS_FreeEnumeratedUSBCameras(CS_USBCameraInfo* cameras, int count);

CS_Source* CS_EnumerateSources(int* count, CS_Status* status);
void CS_ReleaseEnumeratedSources(CS_Source* sources, int count);

CS_Sink* CS_EnumerateSinks(int* count, CS_Status* status);
void CS_ReleaseEnumeratedSinks(CS_Sink* sinks, int count);

void CS_FreeString(char* str);
void CS_FreeEnumPropertyChoices(char** choices, int count);

void CS_FreeEnumeratedProperties(CS_Property* properties, int count);

#ifdef __cplusplus
}
#endif

#endif /* CAMERASERVER_C_H_ */
