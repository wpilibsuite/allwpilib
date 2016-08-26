/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_c.h"

#include <cstddef>
#include <cstdlib>

#include "llvm/SmallString.h"

#include "cameraserver_cpp.h"

//
// Conversion helpers
//

static char* ConvertToC(llvm::StringRef in) {
  char* out = static_cast<char*>(std::malloc(in.size() + 1));
  std::memmove(out, in.data(), in.size());
  out[in.size()] = '\0';
  return out;
}

extern "C" {

CS_PropertyType CS_GetPropertyType(CS_Property property, CS_Status* status) {
  return cs::GetPropertyType(property, status);
}

char* CS_GetPropertyName(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetPropertyName(property, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

CS_Bool CS_GetBooleanProperty(CS_Property property, CS_Status* status) {
  return cs::GetBooleanProperty(property, status);
}

void CS_SetBooleanProperty(CS_Property property, CS_Bool value,
                           CS_Status* status) {
  return cs::SetBooleanProperty(property, value, status);
}

double CS_GetDoubleProperty(CS_Property property, CS_Status* status) {
  return cs::GetDoubleProperty(property, status);
}

void CS_SetDoubleProperty(CS_Property property, double value,
                          CS_Status* status) {
  return cs::SetDoubleProperty(property, value, status);
}

double CS_GetDoublePropertyMin(CS_Property property, CS_Status* status) {
  return cs::GetDoublePropertyMin(property, status);
}

double CS_GetDoublePropertyMax(CS_Property property, CS_Status* status) {
  return cs::GetDoublePropertyMax(property, status);
}

char* CS_GetStringProperty(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetStringProperty(property, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

void CS_SetStringProperty(CS_Property property, const char* value,
                          CS_Status* status) {
  return cs::SetStringProperty(property, value, status);
}

int CS_GetEnumProperty(CS_Property property, CS_Status* status) {
  return cs::GetEnumProperty(property, status);
}

void CS_SetEnumProperty(CS_Property property, int value, CS_Status* status) {
  return cs::SetEnumProperty(property, value, status);
}

char** CS_GetEnumPropertyChoices(CS_Property property, int* count,
                                 CS_Status* status) {
  auto choices = cs::GetEnumPropertyChoices(property, status);
  char** out = static_cast<char**>(std::malloc(choices.size() * sizeof(char*)));
  *count = choices.size();
  for (std::size_t i = 0; i < choices.size(); ++i)
    out[i] = ConvertToC(choices[i]);
  return out;
}

CS_Property CS_GetSourceProperty(CS_Source handle, const char* name,
                                 CS_Status* status) {
  return cs::GetSourceProperty(handle, name, status);
}

CS_Source CS_CreateUSBSourceDev(const char* name, int dev, CS_Status* status) {
  return cs::CreateUSBSourceDev(name, dev, status);
}

CS_Source CS_CreateUSBSourcePath(const char* name, const char* path,
                                 CS_Status* status) {
  return cs::CreateUSBSourcePath(name, path, status);
}

CS_Source CS_CreateHTTPSource(const char* name, const char* url,
                              CS_Status* status) {
  return cs::CreateHTTPSource(name, url, status);
}

CS_Source CS_CreateCvSource(const char* name, int numChannels,
                            CS_Status* status) {
  return cs::CreateCvSource(name, numChannels, status);
}

char* CS_GetSourceName(CS_Source source, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetSourceName(source, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

char* CS_GetSourceDescription(CS_Source source, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetSourceDescription(source, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

uint64_t CS_GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  return cs::GetSourceLastFrameTime(source, status);
}

int CS_GetSourceNumChannels(CS_Source source, CS_Status* status) {
  return cs::GetSourceNumChannels(source, status);
}

CS_Bool CS_IsSourceConnected(CS_Source source, CS_Status* status) {
  return cs::IsSourceConnected(source, status);
}

CS_Property* CS_EnumerateSourceProperties(CS_Source source, int* count,
                                          CS_Status* status) {
  llvm::SmallVector<CS_Property, 32> vec;
  cs::EnumerateSourceProperties(source, vec, status);
  CS_Property* out =
      static_cast<CS_Property*>(std::malloc(vec.size() * sizeof(CS_Property)));
  *count = vec.size();
  std::copy(vec.begin(), vec.end(), out);
  return out;
}

CS_Source CS_CopySource(CS_Source source, CS_Status* status) {
  return cs::CopySource(source, status);
}

void CS_ReleaseSource(CS_Source source, CS_Status* status) {
  return cs::ReleaseSource(source, status);
}

void CS_PutSourceImage(CS_Source source, int channel, struct CvMat* image,
                       CS_Status* status) {
  //TODO: return cs::PutSourceImage(source, channel, image, status);
}

void CS_NotifySourceFrame(CS_Source source, CS_Status* status) {
  return cs::NotifySourceFrame(source, status);
}

void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status) {
  //TODO: return cs::PutSourceFrame(source, image, status);
}

void CS_NotifySourceError(CS_Source source, const char* msg,
                          CS_Status* status) {
  return cs::NotifySourceError(source, msg, status);
}

void CS_SetSourceConnected(CS_Source source, CS_Bool connected,
                           CS_Status* status) {
  return cs::SetSourceConnected(source, connected, status);
}

CS_Property CS_CreateSourceProperty(CS_Source source, const char* name,
                                    enum CS_PropertyType type,
                                    CS_Status* status) {
  return cs::CreateSourceProperty(source, name, type, status);
}

CS_Property CS_CreateSourcePropertyCallback(
    CS_Source source, const char* name, enum CS_PropertyType type, void* data,
    void (*onChange)(void* data, CS_Property property),
    CS_Status* status) {
  return cs::CreateSourcePropertyCallback(
      source, name, type,
      [=](CS_Property property) { onChange(data, property); }, status);
}

void CS_RemoveSourceProperty(CS_Source source, CS_Property property,
                             CS_Status* status) {
  return cs::RemoveSourceProperty(source, property, status);
}

void CS_RemoveSourcePropertyByName(CS_Source source, const char* name,
                                   CS_Status* status) {
  return cs::RemoveSourceProperty(source, name, status);
}

CS_Sink CS_CreateHTTPSink(const char* name, const char* listenAddress, int port,
                          CS_Status* status) {
  return cs::CreateHTTPSink(name, listenAddress, port, status);
}

CS_Sink CS_CreateCvSink(const char* name, CS_Status* status) {
  return cs::CreateCvSink(name, status);
}

CS_Sink CS_CreateCvSinkCallback(const char* name, void* data,
                                void (*processFrame)(void* data, uint64_t time),
                                CS_Status* status) {
  return cs::CreateCvSinkCallback(
      name, [=](uint64_t time) { processFrame(data, time); }, status);
}

char* CS_GetSinkName(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetSinkName(sink, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

char* CS_GetSinkDescription(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetSinkDescription(sink, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

void CS_SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status) {
  return cs::SetSinkSource(sink, source, status);
}

CS_Source CS_GetSinkSource(CS_Sink sink, CS_Status* status) {
  return cs::GetSinkSource(sink, status);
}

CS_Sink CS_CopySink(CS_Sink sink, CS_Status* status) {
  return cs::CopySink(sink, status);
}

void CS_ReleaseSink(CS_Sink sink, CS_Status* status) {
  return cs::ReleaseSink(sink, status);
}

void CS_SetSinkSourceChannel(CS_Sink sink, int channel, CS_Status* status) {
  return cs::SetSinkSourceChannel(sink, channel, status);
}

uint64_t CS_SinkWaitForFrame(CS_Sink sink, CS_Status* status) {
  return cs::SinkWaitForFrame(sink, status);
}

CS_Bool CS_GetSinkImage(CS_Sink sink, int channel, struct CvMat* image,
                        CS_Status* status) {
  return 0;  //TODO: cs::GetSinkImage(sink, channel, image, status);
}

uint64_t CS_GrabSinkFrame(CS_Sink sink, struct CvMat* image,
                          CS_Status* status) {
  return 0;  // TODO: cs::GrabSinkFrame(sink, image, status);
}

char* CS_GetSinkError(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> str;
  cs::GetSinkError(sink, str, status);
  if (*status != 0) return nullptr;
  return ConvertToC(str);
}

void CS_SetSinkEnabled(CS_Sink sink, CS_Bool enabled, CS_Status* status) {
  return cs::SetSinkEnabled(sink, enabled, status);
}

CS_Listener CS_AddSourceListener(void* data,
                                 void (*callback)(void* data, const char* name,
                                                  CS_Source source, int event),
                                 int eventMask, CS_Status* status) {
  return cs::AddSourceListener(
      [=](llvm::StringRef name, CS_Source source, int event) {
        // avoid the copy if possible
        if (name[name.size()] == '\0') {
          callback(data, name.data(), source, event);
        } else {
          llvm::SmallString<128> copy{name};
          callback(data, copy.c_str(), source, event);
        }
      },
      eventMask, status);
}

void CS_RemoveSourceListener(CS_Listener handle, CS_Status* status) {
  return cs::RemoveSourceListener(handle, status);
}

CS_Listener CS_AddSinkListener(void* data,
                               void (*callback)(void* data, const char* name,
                                                CS_Sink sink, int event),
                               int eventMask, CS_Status* status) {
  return cs::AddSinkListener(
      [=](llvm::StringRef name, CS_Sink sink, int event) {
        // avoid the copy if possible
        if (name[name.size()] == '\0') {
          callback(data, name.data(), sink, event);
        } else {
          llvm::SmallString<128> copy{name};
          callback(data, copy.c_str(), sink, event);
        }
      },
      eventMask, status);
}

void CS_RemoveSinkListener(CS_Listener handle, CS_Status* status) {
  return cs::RemoveSinkListener(handle, status);
}

CS_USBCameraInfo* CS_EnumerateUSBCameras(int* count, CS_Status* status) {
  auto cameras = cs::EnumerateUSBCameras(status);
  CS_USBCameraInfo* out = static_cast<CS_USBCameraInfo*>(
      std::malloc(cameras.size() * sizeof(CS_USBCameraInfo)));
  *count = cameras.size();
  for (std::size_t i = 0; i < cameras.size(); ++i) {
    out[i].dev = cameras[i].dev;
    out[i].path = ConvertToC(cameras[i].path);
    out[i].name = ConvertToC(cameras[i].name);
    out[i].channels = cameras[i].channels;
  }
  return out;
}

void CS_FreeEnumeratedUSBCameras(CS_USBCameraInfo* cameras, int count) {
  if (!cameras) return;
  for (int i = 0; i < count; ++i) {
    std::free(cameras[i].path);
    std::free(cameras[i].name);
  }
  std::free(cameras);
}

CS_Source* CS_EnumerateSources(int* count, CS_Status* status) {
  llvm::SmallVector<CS_Source, 32> handles;
  cs::EnumerateSources(handles, status);
  CS_Source* sources =
      static_cast<CS_Source*>(std::malloc(handles.size() * sizeof(CS_Source)));
  *count = handles.size();
  std::copy(handles.begin(), handles.end(), sources);
  return sources;
}

void CS_ReleaseEnumeratedSources(CS_Source* sources, int count) {
  if (!sources) return;
  for (int i = 0; i < count; ++i) {
    CS_Status status = 0;
    if (sources[i] != 0) cs::ReleaseSource(sources[i], &status);
  }
  std::free(sources);
}

CS_Sink* CS_EnumerateSinks(int* count, CS_Status* status) {
  llvm::SmallVector<CS_Sink, 32> handles;
  cs::EnumerateSinks(handles, status);
  CS_Sink* sinks =
      static_cast<CS_Sink*>(std::malloc(handles.size() * sizeof(CS_Sink)));
  *count = handles.size();
  std::copy(handles.begin(), handles.end(), sinks);
  return sinks;
}

void CS_ReleaseEnumeratedSinks(CS_Sink* sinks, int count) {
  if (!sinks) return;
  for (int i = 0; i < count; ++i) {
    CS_Status status = 0;
    if (sinks[i] != 0) cs::ReleaseSink(sinks[i], &status);
  }
  std::free(sinks);
}

void CS_FreeString(char* str) {
  std::free(str);
}

void CS_FreeEnumPropertyChoices(char** choices, int count) {
  if (!choices) return;
  for (int i = 0; i < count; ++i) std::free(choices[i]);
  std::free(choices);
}

void CS_FreeEnumeratedProperties(CS_Property* properties, int count) {
  std::free(properties);
}

}  // extern "C"
