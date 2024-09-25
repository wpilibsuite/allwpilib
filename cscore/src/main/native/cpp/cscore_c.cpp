// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_c.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <vector>

#include <wpi/MemAlloc.h>
#include <wpi/SmallString.h>

#include "c_util.h"
#include "cscore_cpp.h"

static CS_Event ConvertToC(const cs::RawEvent& rawEvent) {
  CS_Event event;
  event.kind = static_cast<CS_EventKind>(static_cast<int>(rawEvent.kind));
  event.source = rawEvent.sourceHandle;
  event.sink = rawEvent.sinkHandle;
  cs::ConvertToC(&event.name, rawEvent.name);
  event.mode = rawEvent.mode;
  event.property = rawEvent.propertyHandle;
  event.propertyKind = rawEvent.propertyKind;
  event.value = rawEvent.value;
  cs::ConvertToC(&event.name, rawEvent.valueStr);
  event.listener = rawEvent.listener;
  return event;
}

template <typename O, typename I>
static O* ConvertToC(std::vector<I>&& in, int* count) {
  using T = std::vector<I>;
  size_t size = in.size();
  O* out = static_cast<O*>(wpi::safe_malloc(size * sizeof(O) + sizeof(T)));
  *count = size;
  for (size_t i = 0; i < size; ++i) {
    out[i] = ConvertToC(in[i]);
  }

  // retain vector at end of returned array
  alignas(T) unsigned char buf[sizeof(T)];
  new (buf) T(std::move(in));
  std::memcpy(out + size * sizeof(O), buf, sizeof(T));

  return out;
}

extern "C" {

CS_PropertyKind CS_GetPropertyKind(CS_Property property, CS_Status* status) {
  return cs::GetPropertyKind(property, status);
}

void CS_GetPropertyName(CS_Property property, WPI_String* name,
                        CS_Status* status) {
  wpi::SmallString<128> buf;
  cs::ConvertToC(name, cs::GetPropertyName(property, buf, status));
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

void CS_GetStringProperty(CS_Property property, WPI_String* value,
                          CS_Status* status) {
  wpi::SmallString<128> buf;
  cs::ConvertToC(value, cs::GetStringProperty(property, buf, status));
}

void CS_SetStringProperty(CS_Property property, const struct WPI_String* value,
                          CS_Status* status) {
  return cs::SetStringProperty(property, wpi::to_string_view(value), status);
}

WPI_String* CS_GetEnumPropertyChoices(CS_Property property, int* count,
                                      CS_Status* status) {
  auto choices = cs::GetEnumPropertyChoices(property, status);
  WPI_String* out = WPI_AllocateStringArray(choices.size());
  *count = choices.size();
  for (size_t i = 0; i < choices.size(); ++i) {
    cs::ConvertToC(&out[i], choices[i]);
  }
  return out;
}

CS_SourceKind CS_GetSourceKind(CS_Source source, CS_Status* status) {
  return cs::GetSourceKind(source, status);
}

void CS_GetSourceName(CS_Source source, WPI_String* name, CS_Status* status) {
  wpi::SmallString<128> buf;
  cs::ConvertToC(name, cs::GetSourceName(source, buf, status));
}

void CS_GetSourceDescription(CS_Source source, WPI_String* description,
                             CS_Status* status) {
  wpi::SmallString<128> buf;
  cs::ConvertToC(description, cs::GetSourceDescription(source, buf, status));
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

CS_Property CS_GetSourceProperty(CS_Source source,
                                 const struct WPI_String* name,
                                 CS_Status* status) {
  return cs::GetSourceProperty(source, wpi::to_string_view(name), status);
}

CS_Property* CS_EnumerateSourceProperties(CS_Source source, int* count,
                                          CS_Status* status) {
  wpi::SmallVector<CS_Property, 32> buf;
  auto vec = cs::EnumerateSourceProperties(source, buf, status);
  CS_Property* out = static_cast<CS_Property*>(
      wpi::safe_malloc(vec.size() * sizeof(CS_Property)));
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
                                      enum WPI_PixelFormat pixelFormat,
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
                                enum WPI_PixelFormat pixelFormat,
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

CS_Bool CS_SetSourceConfigJson(CS_Source source,
                               const struct WPI_String* config,
                               CS_Status* status) {
  return cs::SetSourceConfigJson(source, wpi::to_string_view(config), status);
}

void CS_GetSourceConfigJson(CS_Source source, WPI_String* config,
                            CS_Status* status) {
  cs::ConvertToC(config, cs::GetSourceConfigJson(source, status));
}

CS_VideoMode* CS_EnumerateSourceVideoModes(CS_Source source, int* count,
                                           CS_Status* status) {
  auto vec = cs::EnumerateSourceVideoModes(source, status);
  CS_VideoMode* out = static_cast<CS_VideoMode*>(
      wpi::safe_malloc(vec.size() * sizeof(CS_VideoMode)));
  *count = vec.size();
  std::copy(vec.begin(), vec.end(), out);
  return out;
}

CS_Sink* CS_EnumerateSourceSinks(CS_Source source, int* count,
                                 CS_Status* status) {
  wpi::SmallVector<CS_Sink, 32> buf;
  auto handles = cs::EnumerateSourceSinks(source, buf, status);
  CS_Sink* sinks =
      static_cast<CS_Sink*>(wpi::safe_malloc(handles.size() * sizeof(CS_Sink)));
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

void CS_GetSinkName(CS_Sink sink, WPI_String* name, CS_Status* status) {
  wpi::SmallString<128> buf;
  cs::ConvertToC(name, cs::GetSinkName(sink, buf, status));
}

void CS_GetSinkDescription(CS_Sink sink, WPI_String* description,
                           CS_Status* status) {
  wpi::SmallString<128> buf;
  cs::ConvertToC(description, cs::GetSinkDescription(sink, buf, status));
}

CS_Property CS_GetSinkProperty(CS_Sink sink, const struct WPI_String* name,
                               CS_Status* status) {
  return cs::GetSinkProperty(sink, wpi::to_string_view(name), status);
}

CS_Property* CS_EnumerateSinkProperties(CS_Sink sink, int* count,
                                        CS_Status* status) {
  wpi::SmallVector<CS_Property, 32> buf;
  auto vec = cs::EnumerateSinkProperties(sink, buf, status);
  CS_Property* out = static_cast<CS_Property*>(
      wpi::safe_malloc(vec.size() * sizeof(CS_Property)));
  *count = vec.size();
  std::copy(vec.begin(), vec.end(), out);
  return out;
}

CS_Bool CS_SetSinkConfigJson(CS_Sink sink, const struct WPI_String* config,
                             CS_Status* status) {
  return cs::SetSinkConfigJson(sink, wpi::to_string_view(config), status);
}

void CS_GetSinkConfigJson(CS_Sink sink, WPI_String* config, CS_Status* status) {
  cs::ConvertToC(config, cs::GetSinkConfigJson(sink, status));
}

void CS_SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status) {
  return cs::SetSinkSource(sink, source, status);
}

CS_Source CS_GetSinkSource(CS_Sink sink, CS_Status* status) {
  return cs::GetSinkSource(sink, status);
}

CS_Property CS_GetSinkSourceProperty(CS_Sink sink,
                                     const struct WPI_String* name,
                                     CS_Status* status) {
  return cs::GetSinkSourceProperty(sink, wpi::to_string_view(name), status);
}

CS_Sink CS_CopySink(CS_Sink sink, CS_Status* status) {
  return cs::CopySink(sink, status);
}

void CS_ReleaseSink(CS_Sink sink, CS_Status* status) {
  return cs::ReleaseSink(sink, status);
}

void CS_SetListenerOnStart(void (*onStart)(void* data), void* data) {
  cs::SetListenerOnStart([=] { onStart(data); });
}

void CS_SetListenerOnExit(void (*onExit)(void* data), void* data) {
  cs::SetListenerOnExit([=] { onExit(data); });
}

CS_Listener CS_AddListener(void* data,
                           void (*callback)(void* data, const CS_Event* event),
                           int eventMask, CS_Bool immediateNotify,
                           CS_Status* status) {
  return cs::AddListener(
      [=](const cs::RawEvent& rawEvent) {
        CS_Event event = ConvertToC(rawEvent);
        callback(data, &event);
      },
      eventMask, immediateNotify, status);
}

void CS_RemoveListener(CS_Listener handle, CS_Status* status) {
  return cs::RemoveListener(handle, status);
}

CS_ListenerPoller CS_CreateListenerPoller(void) {
  return cs::CreateListenerPoller();
}

void CS_DestroyListenerPoller(CS_ListenerPoller poller) {
  cs::DestroyListenerPoller(poller);
}

CS_Listener CS_AddPolledListener(CS_ListenerPoller poller, int eventMask,
                                 CS_Bool immediateNotify, CS_Status* status) {
  return cs::AddPolledListener(poller, eventMask, immediateNotify, status);
}

struct CS_Event* CS_PollListener(CS_ListenerPoller poller, int* count) {
  return ConvertToC<CS_Event>(cs::PollListener(poller), count);
}

struct CS_Event* CS_PollListenerTimeout(CS_ListenerPoller poller, int* count,
                                        double timeout, CS_Bool* timedOut) {
  bool cppTimedOut = false;
  auto arrCpp = cs::PollListener(poller, timeout, &cppTimedOut);
  *timedOut = cppTimedOut;
  return ConvertToC<CS_Event>(std::move(arrCpp), count);
}

void CS_CancelPollListener(CS_ListenerPoller poller) {
  cs::CancelPollListener(poller);
}

void CS_FreeEvents(CS_Event* arr, int count) {
  // destroy vector saved at end of array
  using T = std::vector<cs::RawEvent>;
  alignas(T) unsigned char buf[sizeof(T)];
  std::memcpy(buf, arr + count * sizeof(CS_Event), sizeof(T));
  reinterpret_cast<T*>(buf)->~T();

  std::free(arr);
}

int CS_NotifierDestroyed(void) {
  return cs::NotifierDestroyed();
}

void CS_SetTelemetryPeriod(double seconds) {
  cs::SetTelemetryPeriod(seconds);
}

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

void CS_SetLogger(CS_LogFunc func, void* data, unsigned int min_level) {
  cs::SetLogger(
      [=](unsigned int level, const char* file, unsigned int line,
          const char* msg) {
        auto fileStr = wpi::make_string(file);
        auto msgStr = wpi::make_string(msg);
        func(data, level, &fileStr, line, &msgStr);
      },
      min_level);
}

void CS_SetDefaultLogger(unsigned int min_level) {
  cs::SetDefaultLogger(min_level);
}

void CS_Shutdown(void) {
  cs::Shutdown();
}

CS_Source* CS_EnumerateSources(int* count, CS_Status* status) {
  wpi::SmallVector<CS_Source, 32> buf;
  auto handles = cs::EnumerateSourceHandles(buf, status);
  CS_Source* sources = static_cast<CS_Source*>(
      wpi::safe_malloc(handles.size() * sizeof(CS_Source)));
  *count = handles.size();
  std::copy(handles.begin(), handles.end(), sources);
  return sources;
}

void CS_ReleaseEnumeratedSources(CS_Source* sources, int count) {
  if (!sources) {
    return;
  }
  for (int i = 0; i < count; ++i) {
    CS_Status status = 0;
    if (sources[i] != 0) {
      cs::ReleaseSource(sources[i], &status);
    }
  }
  std::free(sources);
}

CS_Sink* CS_EnumerateSinks(int* count, CS_Status* status) {
  wpi::SmallVector<CS_Sink, 32> buf;
  auto handles = cs::EnumerateSinkHandles(buf, status);
  CS_Sink* sinks =
      static_cast<CS_Sink*>(wpi::safe_malloc(handles.size() * sizeof(CS_Sink)));
  *count = handles.size();
  std::copy(handles.begin(), handles.end(), sinks);
  return sinks;
}

void CS_ReleaseEnumeratedSinks(CS_Sink* sinks, int count) {
  if (!sinks) {
    return;
  }
  for (int i = 0; i < count; ++i) {
    CS_Status status = 0;
    if (sinks[i] != 0) {
      cs::ReleaseSink(sinks[i], &status);
    }
  }
  std::free(sinks);
}

void CS_FreeEnumeratedProperties(CS_Property* properties, int count) {
  std::free(properties);
}

void CS_FreeEnumeratedVideoModes(CS_VideoMode* modes, int count) {
  std::free(modes);
}

void CS_GetHostname(struct WPI_String* hostname) {
  cs::ConvertToC(hostname, cs::GetHostname());
}

WPI_String* CS_GetNetworkInterfaces(int* count) {
  auto interfaces = cs::GetNetworkInterfaces();
  WPI_String* out = WPI_AllocateStringArray(interfaces.size());
  *count = interfaces.size();
  for (size_t i = 0; i < interfaces.size(); ++i) {
    cs::ConvertToC(&out[i], interfaces[i]);
  }
  return out;
}

}  // extern "C"
