// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/cscore_cpp.hpp"

#include <memory>
#include <string>
#include <vector>

#include "Handle.hpp"
#include "Instance.hpp"
#include "NetworkListener.hpp"
#include "Notifier.hpp"
#include "PropertyContainer.hpp"
#include "SinkImpl.hpp"
#include "SourceImpl.hpp"
#include "Telemetry.hpp"
#include "wpi/net/hostname.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::cs;

static std::shared_ptr<PropertyContainer> GetPropertyContainer(
    CS_Property propertyHandle, int* propertyIndex, CS_Status* status) {
  std::shared_ptr<PropertyContainer> container;
  Handle handle{propertyHandle};
  if (handle.IsType(Handle::kProperty)) {
    int i = handle.GetParentIndex();
    auto data = Instance::GetInstance().GetSource(Handle{i, Handle::kSource});
    if (!data) {
      *status = CS_INVALID_HANDLE;
      return nullptr;
    }
    container = data->source;
  } else if (handle.IsType(Handle::kSinkProperty)) {
    int i = handle.GetParentIndex();
    auto data = Instance::GetInstance().GetSink(Handle{i, Handle::kSink});
    if (!data) {
      *status = CS_INVALID_HANDLE;
      return nullptr;
    }
    container = data->sink;
  } else {
    *status = CS_INVALID_HANDLE;
    return nullptr;
  }
  *propertyIndex = handle.GetIndex();
  return container;
}

namespace wpi::cs {

//
// Property Functions
//

CS_PropertyKind GetPropertyKind(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return CS_PROP_NONE;
  }
  return container->GetPropertyKind(propertyIndex);
}

std::string GetPropertyName(CS_Property property, CS_Status* status) {
  wpi::util::SmallString<128> buf;
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return {};
  }
  return std::string{container->GetPropertyName(propertyIndex, buf, status)};
}

std::string_view GetPropertyName(CS_Property property,
                                 wpi::util::SmallVectorImpl<char>& buf,
                                 CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return {};
  }
  return container->GetPropertyName(propertyIndex, buf, status);
}

int GetProperty(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return false;
  }
  return container->GetProperty(propertyIndex, status);
}

void SetProperty(CS_Property property, int value, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return;
  }
  container->SetProperty(propertyIndex, value, status);
}

int GetPropertyMin(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return 0.0;
  }
  return container->GetPropertyMin(propertyIndex, status);
}

int GetPropertyMax(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return 0.0;
  }
  return container->GetPropertyMax(propertyIndex, status);
}

int GetPropertyStep(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return 0.0;
  }
  return container->GetPropertyStep(propertyIndex, status);
}

int GetPropertyDefault(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return 0.0;
  }
  return container->GetPropertyDefault(propertyIndex, status);
}

std::string GetStringProperty(CS_Property property, CS_Status* status) {
  wpi::util::SmallString<128> buf;
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return {};
  }
  return std::string{container->GetStringProperty(propertyIndex, buf, status)};
}

std::string_view GetStringProperty(CS_Property property,
                                   wpi::util::SmallVectorImpl<char>& buf,
                                   CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return {};
  }
  return container->GetStringProperty(propertyIndex, buf, status);
}

void SetStringProperty(CS_Property property, std::string_view value,
                       CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return;
  }
  container->SetStringProperty(propertyIndex, value, status);
}

std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) {
    return {};
  }
  return container->GetEnumPropertyChoices(propertyIndex, status);
}

//
// Source Functions
//

CS_SourceKind GetSourceKind(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return CS_SOURCE_UNKNOWN;
  }
  return data->kind;
}

std::string GetSourceName(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return std::string{data->source->GetName()};
}

std::string_view GetSourceName(CS_Source source,
                               wpi::util::SmallVectorImpl<char>& buf,
                               CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return data->source->GetName();
}

std::string GetSourceDescription(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  wpi::util::SmallString<128> buf;
  return std::string{data->source->GetDescription(buf)};
}

std::string_view GetSourceDescription(CS_Source source,
                                      wpi::util::SmallVectorImpl<char>& buf,
                                      CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return data->source->GetDescription(buf);
}

uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->source->GetCurFrameTime();
}

void SetSourceConnectionStrategy(CS_Source source,
                                 CS_ConnectionStrategy strategy,
                                 CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetConnectionStrategy(strategy);
}

bool IsSourceConnected(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->IsConnected();
}

bool IsSourceEnabled(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->IsEnabled();
}

CS_Property GetSourceProperty(CS_Source source, std::string_view name,
                              CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  int property = data->source->GetPropertyIndex(name);
  if (property < 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return Handle{source, property, Handle::kProperty};
}

std::span<CS_Property> EnumerateSourceProperties(
    CS_Source source, wpi::util::SmallVectorImpl<CS_Property>& vec,
    CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  wpi::util::SmallVector<int, 32> properties_buf;
  for (auto property :
       data->source->EnumerateProperties(properties_buf, status)) {
    vec.push_back(Handle{source, property, Handle::kProperty});
  }
  return vec;
}

VideoMode GetSourceVideoMode(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return VideoMode{};
  }
  return data->source->GetVideoMode(status);
}

bool SetSourceVideoMode(CS_Source source, const VideoMode& mode,
                        CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetVideoMode(mode, status);
}

bool SetSourcePixelFormat(CS_Source source, wpi::util::PixelFormat pixelFormat,
                          CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetPixelFormat(pixelFormat, status);
}

bool SetSourceResolution(CS_Source source, int width, int height,
                         CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetResolution(width, height, status);
}

bool SetSourceFPS(CS_Source source, int fps, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetFPS(fps, status);
}

bool SetSourceConfigJson(CS_Source source, std::string_view config,
                         CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetConfigJson(config, status);
}

bool SetSourceConfigJson(CS_Source source, const wpi::util::json& config,
                         CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetConfigJson(config, status);
}

std::string GetSourceConfigJson(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return data->source->GetConfigJson(status);
}

wpi::util::json GetSourceConfigJsonObject(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::util::json{};
  }
  return data->source->GetConfigJsonObject(status);
}

std::vector<VideoMode> EnumerateSourceVideoModes(CS_Source source,
                                                 CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::vector<VideoMode>{};
  }
  return data->source->EnumerateVideoModes(status);
}

std::span<CS_Sink> EnumerateSourceSinks(
    CS_Source source, wpi::util::SmallVectorImpl<CS_Sink>& vec,
    CS_Status* status) {
  auto& inst = Instance::GetInstance();
  auto data = inst.GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return inst.EnumerateSourceSinks(source, vec);
}

CS_Source CopySource(CS_Source source, CS_Status* status) {
  if (source == 0) {
    return 0;
  }
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  data->refCount++;
  return source;
}

void ReleaseSource(CS_Source source, CS_Status* status) {
  if (source == 0) {
    return;
  }
  auto& inst = Instance::GetInstance();
  auto data = inst.GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->refCount-- == 0) {
    inst.DestroySource(source);
  }
}

//
// Camera Source Common Property Functions
//

void SetCameraBrightness(CS_Source source, int brightness, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetBrightness(brightness, status);
}

int GetCameraBrightness(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->source->GetBrightness(status);
}

void SetCameraWhiteBalanceAuto(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetWhiteBalanceAuto(status);
}

void SetCameraWhiteBalanceHoldCurrent(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetWhiteBalanceHoldCurrent(status);
}

void SetCameraWhiteBalanceManual(CS_Source source, int value,
                                 CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetWhiteBalanceManual(value, status);
}

void SetCameraExposureAuto(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetExposureAuto(status);
}

void SetCameraExposureHoldCurrent(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetExposureHoldCurrent(status);
}

void SetCameraExposureManual(CS_Source source, int value, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetExposureManual(value, status);
}

//
// Sink Functions
//

CS_SinkKind GetSinkKind(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return CS_SINK_UNKNOWN;
  }
  return data->kind;
}

std::string GetSinkName(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return std::string{data->sink->GetName()};
}

std::string_view GetSinkName(CS_Sink sink,
                             wpi::util::SmallVectorImpl<char>& buf,
                             CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return data->sink->GetName();
}

std::string GetSinkDescription(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  wpi::util::SmallString<128> buf;
  return std::string{data->sink->GetDescription(buf)};
}

std::string_view GetSinkDescription(CS_Sink sink,
                                    wpi::util::SmallVectorImpl<char>& buf,
                                    CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  return data->sink->GetDescription(buf);
}

CS_Property GetSinkProperty(CS_Sink sink, std::string_view name,
                            CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  int property = data->sink->GetPropertyIndex(name);
  if (property < 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return Handle{sink, property, Handle::kSinkProperty};
}

std::span<CS_Property> EnumerateSinkProperties(
    CS_Sink sink, wpi::util::SmallVectorImpl<CS_Property>& vec,
    CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return {};
  }
  wpi::util::SmallVector<int, 32> properties_buf;
  for (auto property :
       data->sink->EnumerateProperties(properties_buf, status)) {
    vec.push_back(Handle{sink, property, Handle::kSinkProperty});
  }
  return vec;
}

bool SetSinkConfigJson(CS_Sink sink, std::string_view config,
                       CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->sink->SetConfigJson(config, status);
}

bool SetSinkConfigJson(CS_Sink sink, const wpi::util::json& config,
                       CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->sink->SetConfigJson(config, status);
}

std::string GetSinkConfigJson(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return data->sink->GetConfigJson(status);
}

wpi::util::json GetSinkConfigJsonObject(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::util::json{};
  }
  return data->sink->GetConfigJsonObject(status);
}

void SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (source == 0) {
    data->sink->SetSource(nullptr);
  } else {
    auto sourceData = Instance::GetInstance().GetSource(source);
    if (!sourceData) {
      *status = CS_INVALID_HANDLE;
      return;
    }
    data->sink->SetSource(sourceData->source);
  }
  data->sourceHandle.store(source);
  Instance::GetInstance().notifier.NotifySinkSourceChanged(
      data->sink->GetName(), sink, source);
}

CS_Source GetSinkSource(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->sourceHandle.load();
}

CS_Property GetSinkSourceProperty(CS_Sink sink, std::string_view name,
                                  CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return GetSourceProperty(data->sourceHandle.load(), name, status);
}

CS_Sink CopySink(CS_Sink sink, CS_Status* status) {
  if (sink == 0) {
    return 0;
  }
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  data->refCount++;
  return sink;
}

void ReleaseSink(CS_Sink sink, CS_Status* status) {
  if (sink == 0) {
    return;
  }
  auto& inst = Instance::GetInstance();
  auto data = inst.GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->refCount-- == 0) {
    inst.DestroySink(sink);
  }
}

//
// Listener Functions
//

void SetListenerOnStart(std::function<void()> onStart) {
  Instance::GetInstance().notifier.SetOnStart(onStart);
}

void SetListenerOnExit(std::function<void()> onExit) {
  Instance::GetInstance().notifier.SetOnExit(onExit);
}

static void StartBackground(int eventMask, bool immediateNotify) {
  auto& inst = Instance::GetInstance();
  if ((eventMask & CS_NETWORK_INTERFACES_CHANGED) != 0) {
    // start network interface event listener
    inst.networkListener.Start();
    if (immediateNotify) {
      inst.notifier.NotifyNetworkInterfacesChanged();
    }
  }
  if ((eventMask & CS_USB_CAMERAS_CHANGED) != 0) {
    // start network interface event listener
    inst.usbCameraListener.Start();
    if (immediateNotify) {
      inst.notifier.NotifyUsbCamerasChanged();
    }
  }
}

CS_Listener AddListener(std::function<void(const RawEvent& event)> callback,
                        int eventMask, bool immediateNotify,
                        CS_Status* status) {
  auto& inst = Instance::GetInstance();
  int uid = inst.notifier.Add(callback, eventMask);
  StartBackground(eventMask, immediateNotify);
  if (immediateNotify) {
    // TODO
  }
  return Handle{uid, Handle::kListener};
}

void RemoveListener(CS_Listener handle, CS_Status* status) {
  int uid = Handle{handle}.GetTypedIndex(Handle::kListener);
  if (uid < 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  Instance::GetInstance().notifier.Remove(uid);
}

CS_ListenerPoller CreateListenerPoller() {
  auto& inst = Instance::GetInstance();
  return Handle(inst.notifier.CreatePoller(), Handle::kListenerPoller);
}

void DestroyListenerPoller(CS_ListenerPoller poller) {
  int uid = Handle{poller}.GetTypedIndex(Handle::kListenerPoller);
  if (uid < 0) {
    return;
  }
  Instance::GetInstance().notifier.RemovePoller(uid);
}

CS_Listener AddPolledListener(CS_ListenerPoller poller, int eventMask,
                              bool immediateNotify, CS_Status* status) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kListenerPoller);
  if (id < 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }

  auto& inst = Instance::GetInstance();
  int uid = inst.notifier.AddPolled(id, eventMask);
  StartBackground(eventMask, immediateNotify);
  return Handle{uid, Handle::kListener};
}

std::vector<RawEvent> PollListener(CS_ListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kListenerPoller);
  if (id < 0) {
    return {};
  }
  return Instance::GetInstance().notifier.Poll(id);
}

std::vector<RawEvent> PollListener(CS_ListenerPoller poller, double timeout,
                                   bool* timedOut) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kListenerPoller);
  if (id < 0) {
    return {};
  }
  return Instance::GetInstance().notifier.Poll(id, timeout, timedOut);
}

void CancelPollListener(CS_ListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kListenerPoller);
  if (id < 0) {
    return;
  }
  return Instance::GetInstance().notifier.CancelPoll(id);
}

bool NotifierDestroyed() {
  return false;
}

//
// Telemetry Functions
//
void SetTelemetryPeriod(double seconds) {
  auto& inst = Instance::GetInstance();
  inst.telemetry.Start();
  inst.telemetry.SetPeriod(seconds);
}

double GetTelemetryElapsedTime() {
  return Instance::GetInstance().telemetry.GetElapsedTime();
}

int64_t GetTelemetryValue(CS_Handle handle, CS_TelemetryKind kind,
                          CS_Status* status) {
  return Instance::GetInstance().telemetry.GetValue(handle, kind, status);
}

double GetTelemetryAverageValue(CS_Handle handle, CS_TelemetryKind kind,
                                CS_Status* status) {
  return Instance::GetInstance().telemetry.GetAverageValue(handle, kind,
                                                           status);
}

//
// Logging Functions
//
void SetLogger(LogFunc func, unsigned int min_level) {
  auto& logger = Instance::GetInstance().logger;
  logger.SetLogger(func);
  logger.set_min_level(min_level);
}

void SetDefaultLogger(unsigned int min_level) {
  auto& inst = Instance::GetInstance();
  inst.SetDefaultLogger();
  inst.logger.set_min_level(min_level);
}

//
// Shutdown Function
//
void Shutdown() {
  Instance::GetInstance().Shutdown();
}

//
// Utility Functions
//

std::span<CS_Source> EnumerateSourceHandles(
    wpi::util::SmallVectorImpl<CS_Source>& vec, CS_Status* status) {
  return Instance::GetInstance().EnumerateSourceHandles(vec);
}

std::span<CS_Sink> EnumerateSinkHandles(
    wpi::util::SmallVectorImpl<CS_Sink>& vec, CS_Status* status) {
  return Instance::GetInstance().EnumerateSinkHandles(vec);
}

std::string GetHostname() {
  return wpi::net::GetHostname();
}

}  // namespace wpi::cs
