/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cscore_c.h"

#include <cstddef>
#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <wpi/SmallString.h>
#include <wpi/memory.h>

#include "c_util.h"
#include "cscore_cpp.h"

extern "C" {

CS_PropertyKind CS_GetPropertyKind(CS_Property property, CS_Status* status) {
  return cs::GetPropertyKind(property, status);
}

char* CS_GetPropertyName(CS_Property property, CS_Status* status) {
  wpi::SmallString<128> buf;
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
  wpi::SmallString<128> buf;
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
  char** out =
      static_cast<char**>(wpi::CheckedMalloc(choices.size() * sizeof(char*)));
  *count = choices.size();
  for (size_t i = 0; i < choices.size(); ++i)
    out[i] = cs::ConvertToC(choices[i]);
  return out;
}

CS_SourceKind CS_GetSourceKind(CS_Source source, CS_Status* status) {
  return cs::GetSourceKind(source, status);
}

char* CS_GetSourceName(CS_Source source, CS_Status* status) {
  wpi::SmallString<128> buf;
  auto str = cs::GetSourceName(source, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

char* CS_GetSourceDescription(CS_Source source, CS_Status* status) {
  wpi::SmallString<128> buf;
  auto str = cs::GetSourceDescription(source, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

uint64_t CS_GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  return cs::GetSourceLastFrameTime(source, status);
}

void CS_SetSourceConnectionStrategy(CS_Source source,
                                    CS_ConnectionStrategy strategy,
                                    CS_Status* status) {
  cs::SetSourceConnectionStrategy(source, strategy, status);
}

CS_Bool CS_IsSourceConnected(CS_Source source, CS_Status* status) {
  return cs::IsSourceConnected(source, status);
}

CS_Bool CS_IsSourceEnabled(CS_Source source, CS_Status* status) {
  return cs::IsSourceEnabled(source, status);
}

CS_Property CS_GetSourceProperty(CS_Source source, const char* name,
                                 CS_Status* status) {
  return cs::GetSourceProperty(source, name, status);
}

CS_Property* CS_EnumerateSourceProperties(CS_Source source, int* count,
                                          CS_Status* status) {
  wpi::SmallVector<CS_Property, 32> buf;
  auto vec = cs::EnumerateSourceProperties(source, buf, status);
  CS_Property* out = static_cast<CS_Property*>(
      wpi::CheckedMalloc(vec.size() * sizeof(CS_Property)));
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
      source,
      cs::VideoMode{static_cast<cs::VideoMode::PixelFormat>(
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

CS_Bool CS_SetSourceConfigJson(CS_Source source, const char* config,
                               CS_Status* status) {
  return cs::SetSourceConfigJson(source, config, status);
}

char* CS_GetSourceConfigJson(CS_Source source, CS_Status* status) {
  return cs::ConvertToC(cs::GetSourceConfigJson(source, status));
}

CS_VideoMode* CS_EnumerateSourceVideoModes(CS_Source source, int* count,
                                           CS_Status* status) {
  auto vec = cs::EnumerateSourceVideoModes(source, status);
  CS_VideoMode* out = static_cast<CS_VideoMode*>(
      wpi::CheckedMalloc(vec.size() * sizeof(CS_VideoMode)));
  *count = vec.size();
  std::copy(vec.begin(), vec.end(), out);
  return out;
}

CS_Sink* CS_EnumerateSourceSinks(CS_Source source, int* count,
                                 CS_Status* status) {
  wpi::SmallVector<CS_Sink, 32> buf;
  auto handles = cs::EnumerateSourceSinks(source, buf, status);
  CS_Sink* sinks = static_cast<CS_Sink*>(
      wpi::CheckedMalloc(handles.size() * sizeof(CS_Sink)));
  *count = handles.size();
  std::copy(handles.begin(), handles.end(), sinks);
  return sinks;
}

CS_Source CS_CopySource(CS_Source source, CS_Status* status) {
  return cs::CopySource(source, status);
}

void CS_ReleaseSource(CS_Source source, CS_Status* status) {
  return cs::ReleaseSource(source, status);
}

void CS_SetCameraBrightness(CS_Source source, int brightness,
                            CS_Status* status) {
  return cs::SetCameraBrightness(source, brightness, status);
}

int CS_GetCameraBrightness(CS_Source source, CS_Status* status) {
  return cs::GetCameraBrightness(source, status);
}

void CS_SetCameraWhiteBalanceAuto(CS_Source source, CS_Status* status) {
  return cs::SetCameraWhiteBalanceAuto(source, status);
}

void CS_SetCameraWhiteBalanceHoldCurrent(CS_Source source, CS_Status* status) {
  return cs::SetCameraWhiteBalanceHoldCurrent(source, status);
}

void CS_SetCameraWhiteBalanceManual(CS_Source source, int value,
                                    CS_Status* status) {
  return cs::SetCameraWhiteBalanceManual(source, value, status);
}

void CS_SetCameraExposureAuto(CS_Source source, CS_Status* status) {
  return cs::SetCameraExposureAuto(source, status);
}

void CS_SetCameraExposureHoldCurrent(CS_Source source, CS_Status* status) {
  return cs::SetCameraExposureHoldCurrent(source, status);
}

void CS_SetCameraExposureManual(CS_Source source, int value,
                                CS_Status* status) {
  return cs::SetCameraExposureManual(source, value, status);
}

CS_SinkKind CS_GetSinkKind(CS_Sink sink, CS_Status* status) {
  return cs::GetSinkKind(sink, status);
}

char* CS_GetSinkName(CS_Sink sink, CS_Status* status) {
  wpi::SmallString<128> buf;
  auto str = cs::GetSinkName(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

char* CS_GetSinkDescription(CS_Sink sink, CS_Status* status) {
  wpi::SmallString<128> buf;
  auto str = cs::GetSinkDescription(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

CS_Property CS_GetSinkProperty(CS_Sink sink, const char* name,
                               CS_Status* status) {
  return cs::GetSinkProperty(sink, name, status);
}

CS_Property* CS_EnumerateSinkProperties(CS_Sink sink, int* count,
                                        CS_Status* status) {
  wpi::SmallVector<CS_Property, 32> buf;
  auto vec = cs::EnumerateSinkProperties(sink, buf, status);
  CS_Property* out = static_cast<CS_Property*>(
      wpi::CheckedMalloc(vec.size() * sizeof(CS_Property)));
  *count = vec.size();
  std::copy(vec.begin(), vec.end(), out);
  return out;
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

void CS_SetListenerOnStart(void (*onStart)(void* data), void* data) {
  cs::SetListenerOnStart([=]() { onStart(data); });
}

void CS_SetListenerOnExit(void (*onExit)(void* data), void* data) {
  cs::SetListenerOnExit([=]() { onExit(data); });
}

CS_Listener CS_AddListener(void* data,
                           void (*callback)(void* data, const CS_Event* event),
                           int eventMask, int immediateNotify,
                           CS_Status* status) {
  return cs::AddListener(
      [=](const cs::RawEvent& rawEvent) {
        CS_Event event;
        event.kind = static_cast<CS_EventKind>(static_cast<int>(rawEvent.kind));
        event.source = rawEvent.sourceHandle;
        event.sink = rawEvent.sinkHandle;
        event.name = rawEvent.name.c_str();
        event.mode = rawEvent.mode;
        event.property = rawEvent.propertyHandle;
        event.propertyKind = rawEvent.propertyKind;
        event.value = rawEvent.value;
        event.valueStr = rawEvent.valueStr.c_str();
        callback(data, &event);
      },
      eventMask, immediateNotify, status);
}

void CS_RemoveListener(CS_Listener handle, CS_Status* status) {
  return cs::RemoveListener(handle, status);
}

int CS_NotifierDestroyed(void) { return cs::NotifierDestroyed(); }

void CS_SetTelemetryPeriod(double seconds) { cs::SetTelemetryPeriod(seconds); }

double CS_GetTelemetryElapsedTime(void) {
  return cs::GetTelemetryElapsedTime();
}

int64_t CS_GetTelemetryValue(CS_Handle handle, CS_TelemetryKind kind,
                             CS_Status* status) {
  return cs::GetTelemetryValue(handle, kind, status);
}

double CS_GetTelemetryAverageValue(CS_Handle handle, CS_TelemetryKind kind,
                                   CS_Status* status) {
  return cs::GetTelemetryAverageValue(handle, kind, status);
}

void CS_SetLogger(CS_LogFunc func, unsigned int min_level) {
  cs::SetLogger(func, min_level);
}

void CS_SetDefaultLogger(unsigned int min_level) {
  cs::SetDefaultLogger(min_level);
}

void CS_Shutdown(void) { cs::Shutdown(); }

CS_Source* CS_EnumerateSources(int* count, CS_Status* status) {
  wpi::SmallVector<CS_Source, 32> buf;
  auto handles = cs::EnumerateSourceHandles(buf, status);
  CS_Source* sources = static_cast<CS_Source*>(
      wpi::CheckedMalloc(handles.size() * sizeof(CS_Source)));
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
  wpi::SmallVector<CS_Sink, 32> buf;
  auto handles = cs::EnumerateSinkHandles(buf, status);
  CS_Sink* sinks = static_cast<CS_Sink*>(
      wpi::CheckedMalloc(handles.size() * sizeof(CS_Sink)));
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

void CS_FreeString(char* str) { std::free(str); }

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

char* CS_GetHostname() { return cs::ConvertToC(cs::GetHostname()); }

char** CS_GetNetworkInterfaces(int* count) {
  auto interfaces = cs::GetNetworkInterfaces();
  char** out = static_cast<char**>(
      wpi::CheckedMalloc(interfaces.size() * sizeof(char*)));
  *count = interfaces.size();
  for (size_t i = 0; i < interfaces.size(); ++i)
    out[i] = cs::ConvertToC(interfaces[i]);
  return out;
}

void CS_FreeNetworkInterfaces(char** interfaces, int count) {
  if (!interfaces) return;
  for (int i = 0; i < count; ++i) std::free(interfaces[i]);
  std::free(interfaces);
}

}  // extern "C"
