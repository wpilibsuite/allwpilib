/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_c.h"

#include <cstddef>
#include <cstdlib>

#include "opencv2/core/core.hpp"
#include "llvm/SmallString.h"

#include "cameraserver_cpp.h"
#include "c_util.h"

extern "C" {

CS_PropertyType CS_GetPropertyType(CS_Property property, CS_Status* status) {
  return cs::GetPropertyType(property, status);
}

char* CS_GetPropertyName(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetPropertyName(property, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

int CS_GetProperty(CS_Property property, CS_Status* status) {
  return cs::GetProperty(property, status);
}

void CS_SetProperty(CS_Property property, int value, CS_Status* status) {
  return cs::SetProperty(property, value, status);
}

int CS_GetPropertyMin(CS_Property property, CS_Status* status) {
  return cs::GetPropertyMin(property, status);
}

int CS_GetPropertyMax(CS_Property property, CS_Status* status) {
  return cs::GetPropertyMax(property, status);
}

int CS_GetPropertyStep(CS_Property property, CS_Status* status) {
  return cs::GetPropertyStep(property, status);
}

int CS_GetPropertyDefault(CS_Property property, CS_Status* status) {
  return cs::GetPropertyDefault(property, status);
}

char* CS_GetStringProperty(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetStringProperty(property, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

void CS_SetStringProperty(CS_Property property, const char* value,
                          CS_Status* status) {
  return cs::SetStringProperty(property, value, status);
}

char** CS_GetEnumPropertyChoices(CS_Property property, int* count,
                                 CS_Status* status) {
  auto choices = cs::GetEnumPropertyChoices(property, status);
  char** out = static_cast<char**>(std::malloc(choices.size() * sizeof(char*)));
  *count = choices.size();
  for (std::size_t i = 0; i < choices.size(); ++i)
    out[i] = cs::ConvertToC(choices[i]);
  return out;
}

CS_Source CS_CreateHTTPCamera(const char* name, const char* url,
                              CS_Status* status) {
  return cs::CreateHTTPCamera(name, url, status);
}

CS_Source CS_CreateCvSource(const char* name, const CS_VideoMode* mode,
                            CS_Status* status) {
  return cs::CreateCvSource(name, static_cast<const cs::VideoMode&>(*mode),
                            status);
}

char* CS_GetSourceName(CS_Source source, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetSourceName(source, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

char* CS_GetSourceDescription(CS_Source source, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetSourceDescription(source, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

uint64_t CS_GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  return cs::GetSourceLastFrameTime(source, status);
}

CS_Bool CS_IsSourceConnected(CS_Source source, CS_Status* status) {
  return cs::IsSourceConnected(source, status);
}

CS_Property CS_GetSourceProperty(CS_Source source, const char* name,
                                 CS_Status* status) {
  return cs::GetSourceProperty(source, name, status);
}

CS_Property* CS_EnumerateSourceProperties(CS_Source source, int* count,
                                          CS_Status* status) {
  llvm::SmallVector<CS_Property, 32> buf;
  auto vec = cs::EnumerateSourceProperties(source, buf, status);
  CS_Property* out =
      static_cast<CS_Property*>(std::malloc(vec.size() * sizeof(CS_Property)));
  *count = vec.size();
  std::copy(vec.begin(), vec.end(), out);
  return out;
}

void CS_GetSourceVideoMode(CS_Source source, CS_VideoMode* mode,
                           CS_Status* status) {
  *mode = cs::GetSourceVideoMode(source, status);
}

CS_Bool CS_SetSourceVideoMode(CS_Source source, const CS_VideoMode* mode,
                           CS_Status* status) {
  return cs::SetSourceVideoMode(
      source, static_cast<const cs::VideoMode&>(*mode), status);
}

CS_Bool CS_SetSourceVideoModeDiscrete(CS_Source source,
                                      enum CS_PixelFormat pixelFormat,
                                      int width, int height, int fps,
                                      CS_Status* status) {
  return cs::SetSourceVideoMode(
      source, cs::VideoMode{static_cast<cs::VideoMode::PixelFormat>(
                                static_cast<int>(pixelFormat)),
                            width, height, fps},
      status);
}

CS_Bool CS_SetSourcePixelFormat(CS_Source source,
                                enum CS_PixelFormat pixelFormat,
                                CS_Status* status) {
  return cs::SetSourcePixelFormat(
      source,
      static_cast<cs::VideoMode::PixelFormat>(static_cast<int>(pixelFormat)),
      status);
}

CS_Bool CS_SetSourceResolution(CS_Source source, int width, int height,
                               CS_Status* status) {
  return cs::SetSourceResolution(source, width, height, status);
}

CS_Bool CS_SetSourceFPS(CS_Source source, int fps, CS_Status* status) {
  return cs::SetSourceFPS(source, fps, status);
}

CS_VideoMode* CS_EnumerateSourceVideoModes(CS_Source source, int* count,
                                           CS_Status* status) {
  auto vec = cs::EnumerateSourceVideoModes(source, status);
  CS_VideoMode* out = static_cast<CS_VideoMode*>(
      std::malloc(vec.size() * sizeof(CS_VideoMode)));
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

void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status) {
  auto mat = cv::cvarrToMat(image);
  return cs::PutSourceFrame(source, mat, status);
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
  llvm::SmallString<128> buf;
  auto str = cs::GetSinkName(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

char* CS_GetSinkDescription(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetSinkDescription(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

void CS_SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status) {
  return cs::SetSinkSource(sink, source, status);
}

CS_Source CS_GetSinkSource(CS_Sink sink, CS_Status* status) {
  return cs::GetSinkSource(sink, status);
}

CS_Property CS_GetSinkSourceProperty(CS_Sink sink, const char* name,
                                     CS_Status* status) {
  return cs::GetSinkSourceProperty(sink, name, status);
}

CS_Sink CS_CopySink(CS_Sink sink, CS_Status* status) {
  return cs::CopySink(sink, status);
}

void CS_ReleaseSink(CS_Sink sink, CS_Status* status) {
  return cs::ReleaseSink(sink, status);
}

uint64_t CS_GrabSinkFrame(CS_Sink sink, struct CvMat* image,
                          CS_Status* status) {
  auto mat = cv::cvarrToMat(image);
  return cs::GrabSinkFrame(sink, mat, status);
}

char* CS_GetSinkError(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetSinkError(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
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

CS_Source* CS_EnumerateSources(int* count, CS_Status* status) {
  llvm::SmallVector<CS_Source, 32> buf;
  auto handles = cs::EnumerateSourceHandles(buf, status);
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
  llvm::SmallVector<CS_Sink, 32> buf;
  auto handles = cs::EnumerateSinkHandles(buf, status);
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

void CS_FreeEnumeratedVideoModes(CS_VideoMode* modes, int count) {
  std::free(modes);
}

}  // extern "C"
