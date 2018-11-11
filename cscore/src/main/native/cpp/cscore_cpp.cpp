/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cscore_cpp.h"

#include <wpi/SmallString.h>
#include <wpi/hostname.h>
#include <wpi/json.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "NetworkListener.h"
#include "Notifier.h"
#include "PropertyContainer.h"
#include "SinkImpl.h"
#include "SourceImpl.h"
#include "Telemetry.h"

using namespace cs;

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

namespace cs {

//
// Property Functions
//

CS_PropertyKind GetPropertyKind(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return CS_PROP_NONE;
  return container->GetPropertyKind(propertyIndex);
}

std::string GetPropertyName(CS_Property property, CS_Status* status) {
  wpi::SmallString<128> buf;
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return std::string{};
  return container->GetPropertyName(propertyIndex, buf, status);
}

wpi::StringRef GetPropertyName(CS_Property property,
                               wpi::SmallVectorImpl<char>& buf,
                               CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return wpi::StringRef{};
  return container->GetPropertyName(propertyIndex, buf, status);
}

int GetProperty(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return false;
  return container->GetProperty(propertyIndex, status);
}

void SetProperty(CS_Property property, int value, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return;
  container->SetProperty(propertyIndex, value, status);
}

int GetPropertyMin(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return 0.0;
  return container->GetPropertyMin(propertyIndex, status);
}

int GetPropertyMax(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return 0.0;
  return container->GetPropertyMax(propertyIndex, status);
}

int GetPropertyStep(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return 0.0;
  return container->GetPropertyStep(propertyIndex, status);
}

int GetPropertyDefault(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return 0.0;
  return container->GetPropertyDefault(propertyIndex, status);
}

std::string GetStringProperty(CS_Property property, CS_Status* status) {
  wpi::SmallString<128> buf;
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return std::string{};
  return container->GetStringProperty(propertyIndex, buf, status);
}

wpi::StringRef GetStringProperty(CS_Property property,
                                 wpi::SmallVectorImpl<char>& buf,
                                 CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return wpi::StringRef{};
  return container->GetStringProperty(propertyIndex, buf, status);
}

void SetStringProperty(CS_Property property, const wpi::Twine& value,
                       CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return;
  container->SetStringProperty(propertyIndex, value, status);
}

std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status) {
  int propertyIndex;
  auto container = GetPropertyContainer(property, &propertyIndex, status);
  if (!container) return std::vector<std::string>{};
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
    return std::string{};
  }
  return data->source->GetName();
}

wpi::StringRef GetSourceName(CS_Source source, wpi::SmallVectorImpl<char>& buf,
                             CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->source->GetName();
}

std::string GetSourceDescription(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  wpi::SmallString<128> buf;
  return data->source->GetDescription(buf);
}

wpi::StringRef GetSourceDescription(CS_Source source,
                                    wpi::SmallVectorImpl<char>& buf,
                                    CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
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

CS_Property GetSourceProperty(CS_Source source, const wpi::Twine& name,
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

wpi::ArrayRef<CS_Property> EnumerateSourceProperties(
    CS_Source source, wpi::SmallVectorImpl<CS_Property>& vec,
    CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  wpi::SmallVector<int, 32> properties_buf;
  for (auto property :
       data->source->EnumerateProperties(properties_buf, status))
    vec.push_back(Handle{source, property, Handle::kProperty});
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

bool SetSourcePixelFormat(CS_Source source, VideoMode::PixelFormat pixelFormat,
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

bool SetSourceConfigJson(CS_Source source, wpi::StringRef config,
                         CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetConfigJson(config, status);
}

bool SetSourceConfigJson(CS_Source source, const wpi::json& config,
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

wpi::json GetSourceConfigJsonObject(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::json{};
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

wpi::ArrayRef<CS_Sink> EnumerateSourceSinks(CS_Source source,
                                            wpi::SmallVectorImpl<CS_Sink>& vec,
                                            CS_Status* status) {
  auto& inst = Instance::GetInstance();
  auto data = inst.GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::ArrayRef<CS_Sink>{};
  }
  return inst.EnumerateSourceSinks(source, vec);
}

CS_Source CopySource(CS_Source source, CS_Status* status) {
  if (source == 0) return 0;
  auto data = Instance::GetInstance().GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  data->refCount++;
  return source;
}

void ReleaseSource(CS_Source source, CS_Status* status) {
  if (source == 0) return;
  auto& inst = Instance::GetInstance();
  auto data = inst.GetSource(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->refCount-- == 0) inst.DestroySource(source);
}

//
// Camera Source Common Property Fuctions
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
    return std::string{};
  }
  return data->sink->GetName();
}

wpi::StringRef GetSinkName(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                           CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->sink->GetName();
}

std::string GetSinkDescription(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  wpi::SmallString<128> buf;
  return data->sink->GetDescription(buf);
}

wpi::StringRef GetSinkDescription(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                                  CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->sink->GetDescription(buf);
}

CS_Property GetSinkProperty(CS_Sink sink, const wpi::Twine& name,
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

wpi::ArrayRef<CS_Property> EnumerateSinkProperties(
    CS_Sink sink, wpi::SmallVectorImpl<CS_Property>& vec, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  wpi::SmallVector<int, 32> properties_buf;
  for (auto property : data->sink->EnumerateProperties(properties_buf, status))
    vec.push_back(Handle{sink, property, Handle::kSinkProperty});
  return vec;
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

CS_Property GetSinkSourceProperty(CS_Sink sink, const wpi::Twine& name,
                                  CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return GetSourceProperty(data->sourceHandle.load(), name, status);
}

CS_Sink CopySink(CS_Sink sink, CS_Status* status) {
  if (sink == 0) return 0;
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  data->refCount++;
  return sink;
}

void ReleaseSink(CS_Sink sink, CS_Status* status) {
  if (sink == 0) return;
  auto& inst = Instance::GetInstance();
  auto data = inst.GetSink(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->refCount-- == 0) inst.DestroySink(sink);
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

CS_Listener AddListener(std::function<void(const RawEvent& event)> callback,
                        int eventMask, bool immediateNotify,
                        CS_Status* status) {
  auto& inst = Instance::GetInstance();
  int uid = inst.notifier.AddListener(callback, eventMask);
  if ((eventMask & CS_NETWORK_INTERFACES_CHANGED) != 0) {
    // start network interface event listener
    inst.networkListener.Start();
    if (immediateNotify) inst.notifier.NotifyNetworkInterfacesChanged();
  }
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
  Instance::GetInstance().notifier.RemoveListener(uid);
}

bool NotifierDestroyed() { return Notifier::destroyed(); }

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
void Shutdown() { Instance::GetInstance().Shutdown(); }

//
// Utility Functions
//

wpi::ArrayRef<CS_Source> EnumerateSourceHandles(
    wpi::SmallVectorImpl<CS_Source>& vec, CS_Status* status) {
  return Instance::GetInstance().EnumerateSourceHandles(vec);
}

wpi::ArrayRef<CS_Sink> EnumerateSinkHandles(wpi::SmallVectorImpl<CS_Sink>& vec,
                                            CS_Status* status) {
  return Instance::GetInstance().EnumerateSinkHandles(vec);
}

std::string GetHostname() { return wpi::GetHostname(); }

}  // namespace cs
