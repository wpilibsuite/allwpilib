/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
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

#include "llvm/SmallString.h"

#include "Handle.h"
#include "Log.h"
#include "NetworkListener.h"
#include "Notifier.h"
#include "SinkImpl.h"
#include "SourceImpl.h"

using namespace cs;

static std::shared_ptr<SourceImpl> GetPropertySource(CS_Property propertyHandle,
                                                     int* propertyIndex,
                                                     CS_Status* status) {
  Handle handle{propertyHandle};
  int i = handle.GetParentIndex();
  if (i < 0) {
    *status = CS_INVALID_HANDLE;
    return nullptr;
  }
  auto data = Sources::GetInstance().Get(Handle{i, Handle::kSource});
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return nullptr;
  }
  *propertyIndex = handle.GetIndex();
  return data->source;
}

namespace cs {

//
// Property Functions
//

CS_PropertyKind GetPropertyKind(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return CS_PROP_NONE;
  return source->GetPropertyKind(propertyIndex);
}

std::string GetPropertyName(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> buf;
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return std::string{};
  return source->GetPropertyName(propertyIndex, buf, status);
}

llvm::StringRef GetPropertyName(CS_Property property,
                                llvm::SmallVectorImpl<char>& buf,
                                CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return llvm::StringRef{};
  return source->GetPropertyName(propertyIndex, buf, status);
}

int GetProperty(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return false;
  return source->GetProperty(propertyIndex, status);
}

void SetProperty(CS_Property property, int value, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->SetProperty(propertyIndex, value, status);
}

int GetPropertyMin(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0.0;
  return source->GetPropertyMin(propertyIndex, status);
}

int GetPropertyMax(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0.0;
  return source->GetPropertyMax(propertyIndex, status);
}

int GetPropertyStep(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0.0;
  return source->GetPropertyStep(propertyIndex, status);
}

int GetPropertyDefault(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0.0;
  return source->GetPropertyDefault(propertyIndex, status);
}

std::string GetStringProperty(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> buf;
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return std::string{};
  return source->GetStringProperty(propertyIndex, buf, status);
}

llvm::StringRef GetStringProperty(CS_Property property,
                                  llvm::SmallVectorImpl<char>& buf,
                                  CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return llvm::StringRef{};
  return source->GetStringProperty(propertyIndex, buf, status);
}

void SetStringProperty(CS_Property property, llvm::StringRef value,
                       CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->SetStringProperty(propertyIndex, value, status);
}

std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return std::vector<std::string>{};
  return source->GetEnumPropertyChoices(propertyIndex, status);
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

llvm::StringRef GetSourceName(CS_Source source,
                              llvm::SmallVectorImpl<char>& buf,
                              CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return llvm::StringRef{};
  }
  return data->source->GetName();
}

std::string GetSourceDescription(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  llvm::SmallString<128> buf;
  return data->source->GetDescription(buf);
}

llvm::StringRef GetSourceDescription(CS_Source source,
                                     llvm::SmallVectorImpl<char>& buf,
                                     CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return llvm::StringRef{};
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

bool IsSourceConnected(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return false;
  }
  return data->source->IsConnected();
}

CS_Property GetSourceProperty(CS_Source source, llvm::StringRef name,
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

llvm::ArrayRef<CS_Property> EnumerateSourceProperties(
    CS_Source source, llvm::SmallVectorImpl<CS_Property>& vec,
    CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  llvm::SmallVector<int, 32> properties_buf;
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

llvm::ArrayRef<CS_Sink> EnumerateSourceSinks(
    CS_Source source, llvm::SmallVectorImpl<CS_Sink>& vec, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return llvm::ArrayRef<CS_Sink>{};
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

llvm::StringRef GetSinkName(CS_Sink sink, llvm::SmallVectorImpl<char>& buf,
                            CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return llvm::StringRef{};
  }
  return data->sink->GetName();
}

std::string GetSinkDescription(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  llvm::SmallString<128> buf;
  return data->sink->GetDescription(buf);
}

llvm::StringRef GetSinkDescription(CS_Sink sink,
                                   llvm::SmallVectorImpl<char>& buf,
                                   CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return llvm::StringRef{};
  }
  return data->sink->GetDescription(buf);
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

CS_Property GetSinkSourceProperty(CS_Sink sink, llvm::StringRef name,
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

llvm::ArrayRef<CS_Source> EnumerateSourceHandles(
    llvm::SmallVectorImpl<CS_Source>& vec, CS_Status* status) {
  return Sources::GetInstance().GetAll(vec);
}

llvm::ArrayRef<CS_Sink> EnumerateSinkHandles(
    llvm::SmallVectorImpl<CS_Sink>& vec, CS_Status* status) {
  return Sinks::GetInstance().GetAll(vec);
}

std::string GetHostname() {
#ifdef __linux__
  char name[256];
  if (::gethostname(name, sizeof(name)) != 0) return "";
  name[255] = '\0';  // Per POSIX, may not be null terminated if too long
  return name;
#else
  return "";  // TODO
#endif
}

std::vector<std::string> GetNetworkInterfaces() {
#ifdef __linux__
  struct ifaddrs* ifa;
  if (::getifaddrs(&ifa) != 0) return std::vector<std::string>{};

  std::vector<std::string> rv;
  char buf[256];
  for (struct ifaddrs* i = ifa; i; i = i->ifa_next) {
    if (!i->ifa_addr) continue;  // no address
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
