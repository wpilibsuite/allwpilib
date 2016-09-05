/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_cpp.h"

#include "llvm/SmallString.h"

#include "SinkImpl.h"
#include "SourceImpl.h"
#include "Handle.h"

using namespace cs;

static std::shared_ptr<SourceImpl> GetPropertySource(CS_Property propertyHandle,
                                                     int* propertyIndex,
                                                     CS_Status* status) {
  Handle handle{propertyHandle};
  int i = handle.GetTypedIndex(Handle::kProperty);
  if (i < 0) {
    *status = CS_INVALID_HANDLE;
    return nullptr;
  }
  auto data = Sources::GetInstance().Get(Handle{i, Handle::kSource});
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return nullptr;
  }
  *propertyIndex = handle.GetSubIndex();
  return data->source;
}

namespace cs {

//
// Property Functions
//

CS_PropertyType GetPropertyType(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return CS_PROP_NONE;
  return source->GetPropertyType(propertyIndex);
}

std::string GetPropertyName(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> name;
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return std::string{};
  source->GetPropertyName(propertyIndex, name);
  return name.str();
}

void GetPropertyName(CS_Property property, llvm::SmallVectorImpl<char>& name,
                     CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->GetPropertyName(propertyIndex, name);
}

bool GetBooleanProperty(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return false;
  return source->GetBooleanProperty(propertyIndex);
}

void SetBooleanProperty(CS_Property property, bool value, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->SetBooleanProperty(propertyIndex, value);
}

double GetDoubleProperty(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return false;
  return source->GetDoubleProperty(propertyIndex);
}

void SetDoubleProperty(CS_Property property, double value, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->SetDoubleProperty(propertyIndex, value);
}

double GetDoublePropertyMin(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0.0;
  return source->GetPropertyMin(propertyIndex);
}

double GetDoublePropertyMax(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0.0;
  return source->GetPropertyMax(propertyIndex);
}

std::string GetStringProperty(CS_Property property, CS_Status* status) {
  llvm::SmallString<128> value;
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return std::string{};
  source->GetStringProperty(propertyIndex, value);
  return value.str();
}

void GetStringProperty(CS_Property property, llvm::SmallVectorImpl<char>& value,
                       CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->GetStringProperty(propertyIndex, value);
}

void SetStringProperty(CS_Property property, llvm::StringRef value,
                       CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->SetStringProperty(propertyIndex, value);
}

int GetEnumProperty(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return 0;
  return source->GetEnumProperty(propertyIndex);
}

void SetEnumProperty(CS_Property property, int value, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return;
  source->SetEnumProperty(propertyIndex, value);
}

std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return std::vector<std::string>{};
  return source->GetEnumPropertyChoices(propertyIndex);
}

//
// Source Creation Functions
//

CS_Source CreateUSBSourceDev(llvm::StringRef name, int dev, CS_Status* status) {
  return 0;  // TODO
}

CS_Source CreateUSBSourcePath(llvm::StringRef name, llvm::StringRef path,
                              CS_Status* status) {
  return 0;  // TODO
}

CS_Source CreateHTTPSource(llvm::StringRef name, llvm::StringRef url,
                           CS_Status* status) {
  return 0;  // TODO
}

CS_Source CreateCvSource(llvm::StringRef name, int numChannels,
                         CS_Status* status) {
  return 0;  // TODO
}

//
// Source Functions
//

std::string GetSourceName(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return data->source->GetName();
}

void GetSourceName(CS_Source source, llvm::SmallVectorImpl<char>& name,
                   CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  auto str = data->source->GetName();
  name.append(str.begin(), str.end());
}

std::string GetSourceDescription(CS_Source source, CS_Status* status) {
  llvm::SmallString<128> desc;
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  data->source->GetDescription(desc);
  return desc.str();
}

void GetSourceDescription(CS_Source source, llvm::SmallVectorImpl<char>& desc,
                          CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->source->GetDescription(desc);
}

uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  return 0;  // TODO
}

int GetSourceNumChannels(CS_Source source, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return data->source->GetNumChannels();
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
  int property = data->source->GetProperty(name);
  if (property < 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return Handle{source, property, Handle::kProperty};
}

void EnumerateSourceProperties(CS_Source source,
                               llvm::SmallVectorImpl<CS_Property>& properties,
                               CS_Status* status) {
  // TODO
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
  if (--(data->refCount) == 0) inst.Free(source);
}

//
// OpenCV Source Functions
//

void PutSourceImage(CS_Source source, int channel, cv::Mat* image,
                    CS_Status* status) {
  // TODO
}

void NotifySourceFrame(CS_Source source, CS_Status* status) {
  // TODO
}

void PutSourceFrame(CS_Source source, cv::Mat* image, CS_Status* status) {
  // TODO
}

void NotifySourceError(CS_Source source, llvm::StringRef msg,
                       CS_Status* status) {
  // TODO
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  // TODO
}

CS_Property CreateSourceProperty(CS_Source source, llvm::StringRef name,
                                 CS_PropertyType type, CS_Status* status) {
  return 0;  // TODO
}

CS_Property CreateSourcePropertyCallback(
    CS_Source source, llvm::StringRef name, CS_PropertyType type,
    std::function<void(CS_Property property)> onChange, CS_Status* status) {
  return 0;  // TODO
}

void RemoveSourceProperty(CS_Source source, CS_Property property,
                          CS_Status* status) {
  // TODO
}

void RemoveSourceProperty(CS_Source source, llvm::StringRef name,
                          CS_Status* status) {
  // TODO
}

//
// Sink Creation Functions
//

CS_Sink CreateHTTPSink(llvm::StringRef name, llvm::StringRef listenAddress,
                       int port, CS_Status* status) {
  return 0;  // TODO
}

CS_Sink CreateCvSink(llvm::StringRef name, CS_Status* status) {
  return 0;  // TODO
}

CS_Sink CreateCvSinkCallback(llvm::StringRef name,
                             std::function<void(uint64_t time)> processFrame,
                             CS_Status* status) {
  return 0;  // TODO
}

//
// Sink Functions
//
std::string GetSinkName(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return data->sink->GetName();
}

void GetSinkName(CS_Sink sink, llvm::SmallVectorImpl<char>& name,
                 CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  auto str = data->sink->GetName();
  name.append(str.begin(), str.end());
}

std::string GetSinkDescription(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> desc;
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  data->sink->GetDescription(desc);
  return desc.str();
}

void GetSinkDescription(CS_Sink sink, llvm::SmallVectorImpl<char>& desc,
                        CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->sink->GetDescription(desc);
}

void SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  auto sourceData = Sources::GetInstance().Get(source);
  if (!sourceData) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  data->sink->SetSource(sourceData->source);
  data->sourceHandle.store(source);
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
  if (--(data->refCount) == 0) inst.Free(sink);
}

//
// Server Sink (e.g. HTTP) Functions
//

void SetSinkSourceChannel(CS_Sink sink, int channel, CS_Status* status) {
  // TODO
}

//
// OpenCV Sink Functions
//

uint64_t SinkWaitForFrame(CS_Sink sink, CS_Status* status) {
  return 0;  // TODO
}

bool GetSinkImage(CS_Sink sink, int channel, cv::Mat* image,
                  CS_Status* status) {
  return false;  // TODO
}

uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat* image, CS_Status* status) {
  return 0;  // TODO
}

std::string GetSinkError(CS_Sink sink, CS_Status* status) {
  return "";  // TODO
}

void GetSinkError(CS_Sink sink, llvm::SmallVectorImpl<char>& msg,
                  CS_Status* status) {
  // TODO
}

void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status) {
  // TODO
}

//
// Listener Functions
//

CS_Listener AddSourceListener(
    std::function<void(llvm::StringRef name, CS_Source source, int event)>
        callback,
    int eventMask, CS_Status* status) {
  return 0;  // TODO
}

void RemoveSourceListener(CS_Listener handle, CS_Status* status) {
  // TODO
}

CS_Listener AddSinkListener(
    std::function<void(llvm::StringRef name, CS_Sink sink, int event)> callback,
    int eventMask, CS_Status* status) {
  return 0;  // TODO
}

void RemoveSinkListener(CS_Listener handle, CS_Status* status) {
  // TODO
}

//
// Utility Functions
//

std::vector<USBCameraInfo> EnumerateUSBCameras(CS_Status* status) {
  return std::vector<USBCameraInfo>{};  // TODO
}

void EnumerateSourceHandles(llvm::SmallVectorImpl<CS_Source>& handles,
                            CS_Status* status) {
  Sources::GetInstance().GetAll(handles);
}

void EnumerateSinkHandles(llvm::SmallVectorImpl<CS_Sink>& handles,
                          CS_Status* status) {
  Sinks::GetInstance().GetAll(handles);
}

}  // namespace cs
