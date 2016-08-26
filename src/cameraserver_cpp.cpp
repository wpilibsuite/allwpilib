/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_cpp.h"

#include <limits>

using namespace cs;

namespace cs {

//
// Property Functions
//

CS_PropertyType GetPropertyType(CS_Property property, CS_Status* status) {
  return CS_PROP_NONE;  // TODO
}

bool GetBooleanProperty(CS_Property property, CS_Status* status) {
  return false;  // TODO
}

void SetBooleanProperty(CS_Property property, bool value, CS_Status* status) {
  // TODO
}

double GetDoubleProperty(CS_Property property, CS_Status* status) {
  return 0.0;  // TODO
}

void SetDoubleProperty(CS_Property property, double value, CS_Status* status) {
  // TODO
}

double GetDoublePropertyMin(CS_Property property, CS_Status* status) {
  return 0.0;  // TODO
}

double GetDoublePropertyMax(CS_Property property, CS_Status* status) {
  return 0.0;  // TODO
}

std::string GetStringProperty(CS_Property property, CS_Status* status) {
  return "";  // TODO
}

void GetStringProperty(CS_Property property, llvm::SmallVectorImpl<char>& value,
                       CS_Status* status) {
  // TODO
}

void SetStringProperty(CS_Property property, llvm::StringRef value,
                       CS_Status* status) {
  // TODO
}

int GetEnumProperty(CS_Property property, CS_Status* status) {
  return 0;  // TODO
}

void SetEnumProperty(CS_Property property, int value, CS_Status* status) {
  // TODO
}

std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status) {
  return std::vector<std::string>{};  // TODO
}

//
// Source/Sink Functions
//

CS_Property GetSourceProperty(CS_Source handle, llvm::StringRef name,
                              CS_Status* status) {
  return 0;  // TODO
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
  return "";  // TODO
}

void GetSourceName(CS_Source sink, llvm::SmallVectorImpl<char>& name,
                   CS_Status* status) {
  // TODO
}

std::string GetSourceDescription(CS_Source source, CS_Status* status) {
  return "";  // TODO
}

void GetSourceDescription(CS_Source source, llvm::SmallVectorImpl<char>& desc,
                          CS_Status* status) {
  // TODO
}

uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status) {
  return 0;  // TODO
}

int GetSourceNumChannels(CS_Source source, CS_Status* status) {
  return 0;  // TODO
}

bool IsSourceConnected(CS_Source source, CS_Status* status) {
  return false;  // TODO
}

CS_Source CopySource(CS_Source source, CS_Status* status) {
  return source;  // TODO
}

void ReleaseSource(CS_Source source, CS_Status* status) {
  // TODO
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
    std::function<void(llvm::StringRef name, CS_Property property)> onChange,
    CS_Status* status) {
  return 0;  // TODO
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
  return "";  // TODO
}

void GetSinkName(CS_Sink sink, llvm::SmallVectorImpl<char>& name,
                 CS_Status* status) {
  // TODO
}

std::string GetSinkDescription(CS_Sink sink, CS_Status* status) {
  return "";  // TODO
}

void GetSinkDescription(CS_Sink sink, llvm::SmallVectorImpl<char>& desc,
                        CS_Status* status) {
  // TODO
}

void SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status) {
  // TODO
}

CS_Source GetSinkSource(CS_Sink sink, CS_Status* status) {
  return 0;  // TODO
}

CS_Sink CopySink(CS_Sink sink, CS_Status* status) {
  return sink;  // TODO
}

void ReleaseSink(CS_Sink sink, CS_Status* status) {
  // TODO
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
  // TODO
}

void EnumerateSinkHandles(llvm::SmallVectorImpl<CS_Sink>& handles,
                          CS_Status* status) {
  // TODO
}

}  // namespace cs
