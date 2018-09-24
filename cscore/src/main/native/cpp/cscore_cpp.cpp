/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cscore_cpp.h"

#if defined(__linux__)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <wpi/SmallString.h>

#include "Handle.h"
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
    auto data = Sources::GetInstance().Get(Handle{i, Handle::kSource});
    if (!data) {
      *status = CS_INVALID_HANDLE;
      return nullptr;
    }
    container = data->source;
  } else if (handle.IsType(Handle::kSinkProperty)) {
    int i = handle.GetParentIndex();
    auto data = Sinks::GetInstance().Get(Handle{i, Handle::kSink});
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
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return CS_SOURCE_UNKNOWN;
  }
  return data->kind;
}

std::string GetSourceName(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return data->source->GetName();
}

wpi::StringRef GetSourceName(CS_Source source, wpi::SmallVectorImpl<char>& buf,
                             CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->source->GetName();
}

std::string GetSourceDescription(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
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
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->source->GetDescription(buf);
}

uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->source->GetCurFrameTime();
}

void SetSourceConnectionStrategy(CS_Source source,
                                 CS_ConnectionStrategy strategy,
                                 CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetConnectionStrategy(strategy);
}

bool IsSourceConnected(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->IsConnected();
}

bool IsSourceEnabled(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->IsEnabled();
}

CS_Property GetSourceProperty(CS_Source source, const wpi::Twine& name,
                              CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
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
  auto data = Sources::GetInstance().Get(source);
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
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return VideoMode{};
  }
  return data->source->GetVideoMode(status);
}

bool SetSourceVideoMode(CS_Source source, const VideoMode& mode,
                        CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetVideoMode(mode, status);
}

bool SetSourcePixelFormat(CS_Source source, VideoMode::PixelFormat pixelFormat,
                          CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetPixelFormat(pixelFormat, status);
}

bool SetSourceResolution(CS_Source source, int width, int height,
                         CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetResolution(width, height, status);
}

bool SetSourceFPS(CS_Source source, int fps, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->SetFPS(fps, status);
}

std::vector<VideoMode> EnumerateSourceVideoModes(CS_Source source,
                                                 CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::vector<VideoMode>{};
  }
  return data->source->EnumerateVideoModes(status);
}

wpi::ArrayRef<CS_Sink> EnumerateSourceSinks(CS_Source source,
                                            wpi::SmallVectorImpl<CS_Sink>& vec,
                                            CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::ArrayRef<CS_Sink>{};
  }
  vec.clear();
  Sinks::GetInstance().ForEach([&](CS_Sink sinkHandle, const SinkData& data) {
    if (source == data.sourceHandle.load()) vec.push_back(sinkHandle);
  });
  return vec;
}

CS_Source CopySource(CS_Source source, CS_Status* status) {
  if (source == 0) return 0;
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  data->refCount++;
  return source;
}

void ReleaseSource(CS_Source source, CS_Status* status) {
  if (source == 0) return;
  auto& inst = Sources::GetInstance();
  auto data = inst.Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->refCount-- == 0) {
    Notifier::GetInstance().NotifySource(data->source->GetName(), source,
                                         CS_SOURCE_DESTROYED);
    inst.Free(source);
  }
}

//
// Camera Source Common Property Fuctions
//

void SetCameraBrightness(CS_Source source, int brightness, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetBrightness(brightness, status);
}

int GetCameraBrightness(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->source->GetBrightness(status);
}

void SetCameraWhiteBalanceAuto(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetWhiteBalanceAuto(status);
}

void SetCameraWhiteBalanceHoldCurrent(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetWhiteBalanceHoldCurrent(status);
}

void SetCameraWhiteBalanceManual(CS_Source source, int value,
                                 CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetWhiteBalanceManual(value, status);
}

void SetCameraExposureAuto(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetExposureAuto(status);
}

void SetCameraExposureHoldCurrent(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->SetExposureHoldCurrent(status);
}

void SetCameraExposureManual(CS_Source source, int value, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
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
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return CS_SINK_UNKNOWN;
  }
  return data->kind;
}

std::string GetSinkName(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return data->sink->GetName();
}

wpi::StringRef GetSinkName(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                           CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->sink->GetName();
}

std::string GetSinkDescription(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  wpi::SmallString<128> buf;
  return data->sink->GetDescription(buf);
}

wpi::StringRef GetSinkDescription(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                                  CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return data->sink->GetDescription(buf);
}

CS_Property GetSinkProperty(CS_Sink sink, const wpi::Twine& name,
                            CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
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
  auto data = Sinks::GetInstance().Get(sink);
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
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (source == 0) {
    data->sink->SetSource(nullptr);
  } else {
    auto sourceData = Sources::GetInstance().Get(source);
    if (!sourceData) {
      *status = CS_INVALID_HANDLE;
      return;
    }
    data->sink->SetSource(sourceData->source);
  }
  data->sourceHandle.store(source);
  Notifier::GetInstance().NotifySinkSourceChanged(data->sink->GetName(), sink,
                                                  source);
}

CS_Source GetSinkSource(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->sourceHandle.load();
}

CS_Property GetSinkSourceProperty(CS_Sink sink, const wpi::Twine& name,
                                  CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return GetSourceProperty(data->sourceHandle.load(), name, status);
}

CS_Sink CopySink(CS_Sink sink, CS_Status* status) {
  if (sink == 0) return 0;
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  data->refCount++;
  return sink;
}

void ReleaseSink(CS_Sink sink, CS_Status* status) {
  if (sink == 0) return;
  auto& inst = Sinks::GetInstance();
  auto data = inst.Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  if (data->refCount-- == 0) {
    Notifier::GetInstance().NotifySink(data->sink->GetName(), sink,
                                       CS_SINK_DESTROYED);
    inst.Free(sink);
  }
}

//
// Listener Functions
//

void SetListenerOnStart(std::function<void()> onStart) {
  Notifier::GetInstance().SetOnStart(onStart);
}

void SetListenerOnExit(std::function<void()> onExit) {
  Notifier::GetInstance().SetOnExit(onExit);
}

CS_Listener AddListener(std::function<void(const RawEvent& event)> callback,
                        int eventMask, bool immediateNotify,
                        CS_Status* status) {
  int uid = Notifier::GetInstance().AddListener(callback, eventMask);
  if ((eventMask & CS_NETWORK_INTERFACES_CHANGED) != 0) {
    // start network interface event listener
    NetworkListener::GetInstance().Start();
    if (immediateNotify)
      Notifier::GetInstance().NotifyNetworkInterfacesChanged();
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
  Notifier::GetInstance().RemoveListener(uid);
}

bool NotifierDestroyed() { return Notifier::destroyed(); }

//
// Telemetry Functions
//
void SetTelemetryPeriod(double seconds) {
  Telemetry::GetInstance().Start();
  Telemetry::GetInstance().SetPeriod(seconds);
}

double GetTelemetryElapsedTime() {
  return Telemetry::GetInstance().GetElapsedTime();
}

int64_t GetTelemetryValue(CS_Handle handle, CS_TelemetryKind kind,
                          CS_Status* status) {
  return Telemetry::GetInstance().GetValue(handle, kind, status);
}

double GetTelemetryAverageValue(CS_Handle handle, CS_TelemetryKind kind,
                                CS_Status* status) {
  return Telemetry::GetInstance().GetAverageValue(handle, kind, status);
}

//
// Logging Functions
//
void SetLogger(LogFunc func, unsigned int min_level) {
  Logger& logger = Logger::GetInstance();
  logger.SetLogger(func);
  logger.set_min_level(min_level);
}

void SetDefaultLogger(unsigned int min_level) {
  Logger& logger = Logger::GetInstance();
  logger.SetDefaultLogger();
  logger.set_min_level(min_level);
}

//
// Utility Functions
//

wpi::ArrayRef<CS_Source> EnumerateSourceHandles(
    wpi::SmallVectorImpl<CS_Source>& vec, CS_Status* status) {
  return Sources::GetInstance().GetAll(vec);
}

wpi::ArrayRef<CS_Sink> EnumerateSinkHandles(wpi::SmallVectorImpl<CS_Sink>& vec,
                                            CS_Status* status) {
  return Sinks::GetInstance().GetAll(vec);
}

std::string GetHostname() {
#ifdef __linux__
  char name[256];
  if (::gethostname(name, sizeof(name)) != 0) return "";
  name[255] = '\0';  // Per POSIX, may not be null terminated if too long
  return name;
#else
  return "";                          // TODO
#endif
}

std::vector<std::string> GetNetworkInterfaces() {
#ifdef __linux__
  struct ifaddrs* ifa;
  if (::getifaddrs(&ifa) != 0) return std::vector<std::string>{};

  std::vector<std::string> rv;
  char buf[256];
  for (struct ifaddrs* i = ifa; i; i = i->ifa_next) {
    if (!i->ifa_addr) continue;                       // no address
    if (i->ifa_addr->sa_family != AF_INET) continue;  // only return IPv4
    struct sockaddr_in* addr_in = reinterpret_cast<sockaddr_in*>(i->ifa_addr);
    const char* addr =
        ::inet_ntop(addr_in->sin_family, &addr_in->sin_addr, buf, sizeof(buf));
    if (!addr) continue;  // error converting address
    rv.emplace_back(addr);
  }

  ::freeifaddrs(ifa);
  return rv;
#else
  return std::vector<std::string>{};  // TODO
#endif
}

}  // namespace cs
