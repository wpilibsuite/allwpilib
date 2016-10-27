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

CS_PropertyType GetPropertyType(CS_Property property, CS_Status* status) {
  int propertyIndex;
  auto source = GetPropertySource(property, &propertyIndex, status);
  if (!source) return CS_PROP_NONE;
  return source->GetPropertyType(propertyIndex);
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
// Source Creation Functions
//

CS_Source CreateHTTPCamera(llvm::StringRef name, llvm::StringRef url,
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
  if (data->refCount-- == 0) inst.Free(source);
}

//
// Sink Creation Functions
//

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
  if (data->refCount-- == 0) inst.Free(sink);
}

//
// OpenCV Sink Functions
//

uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat& image, CS_Status* status) {
  return 0;  // TODO
}

std::string GetSinkError(CS_Sink sink, CS_Status* status) {
  return std::string{};  // TODO
}

llvm::StringRef GetSinkError(CS_Sink sink, llvm::SmallVectorImpl<char>& buf,
                             CS_Status* status) {
  return llvm::StringRef{};  // TODO
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

llvm::ArrayRef<CS_Source> EnumerateSourceHandles(
    llvm::SmallVectorImpl<CS_Source>& vec, CS_Status* status) {
  return Sources::GetInstance().GetAll(vec);
}

llvm::ArrayRef<CS_Sink> EnumerateSinkHandles(
    llvm::SmallVectorImpl<CS_Sink>& vec, CS_Status* status) {
  return Sinks::GetInstance().GetAll(vec);
}

}  // namespace cs
