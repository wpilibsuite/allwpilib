/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_CPP_H_
#define CAMERASERVER_CPP_H_

#include <functional>
#include <string>
#include <vector>

#include <stdint.h>

#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"

#include "cameraserver_c.h"

namespace cv {
class Mat;
}

namespace cs {

//
// Handle-based interface for C++.  Users are encouraged to use the
// object oriented interface instead; this interface is intended for use
// in applications such as JNI which require handle-based access.
//

//
// Property Functions
//

CS_PropertyType GetPropertyType(CS_Property property, CS_Status* status);
bool GetBooleanProperty(CS_Property property, CS_Status* status);
void SetBooleanProperty(CS_Property property, bool value, CS_Status* status);
double GetDoubleProperty(CS_Property property, CS_Status* status);
void SetDoubleProperty(CS_Property property, double value, CS_Status* status);
double GetDoublePropertyMin(CS_Property property, CS_Status* status);
double GetDoublePropertyMax(CS_Property property, CS_Status* status);
std::string GetStringProperty(CS_Property property, CS_Status* status);
void GetStringProperty(CS_Property property, llvm::SmallVectorImpl<char>& value,
                       CS_Status* status);
void SetStringProperty(CS_Property property, llvm::StringRef value,
                       CS_Status* status);
int GetEnumProperty(CS_Property property, CS_Status* status);
void SetEnumProperty(CS_Property property, int value, CS_Status* status);
std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status);

//
// Source/Sink Functions
//
CS_Property GetSourceProperty(CS_Source handle, llvm::StringRef name,
                              CS_Status* status);

//
// Source Creation Functions
//
CS_Source CreateUSBSourceDev(llvm::StringRef name, int dev, CS_Status* status);
CS_Source CreateUSBSourcePath(llvm::StringRef name, llvm::StringRef path,
                              CS_Status* status);
CS_Source CreateHTTPSource(llvm::StringRef name, llvm::StringRef url,
                           CS_Status* status);
CS_Source CreateCvSource(llvm::StringRef name, int numChannels,
                         CS_Status* status);

//
// Source Functions
//
std::string GetSourceName(CS_Source source, CS_Status* status);
void GetSourceName(CS_Source source, llvm::SmallVectorImpl<char>& name,
                   CS_Status* status);
std::string GetSourceDescription(CS_Source source, CS_Status* status);
void GetSourceDescription(CS_Source source, llvm::SmallVectorImpl<char>& desc,
                          CS_Status* status);
uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status);
int GetSourceNumChannels(CS_Source source, CS_Status* status);
bool IsSourceConnected(CS_Source source, CS_Status* status);
CS_Source CopySource(CS_Source source, CS_Status* status);
void ReleaseSource(CS_Source source, CS_Status* status);

//
// OpenCV Source Functions
//
void PutSourceImage(CS_Source source, int channel, cv::Mat* image,
                    CS_Status* status);
void NotifySourceFrame(CS_Source source, CS_Status* status);
void PutSourceFrame(CS_Source source, cv::Mat* image, CS_Status* status);
void NotifySourceError(CS_Source source, llvm::StringRef msg,
                       CS_Status* status);
void SetSourceConnected(CS_Source source, bool connected, CS_Status* status);
CS_Property CreateSourceProperty(CS_Source source, llvm::StringRef name,
                                 CS_PropertyType type, CS_Status* status);
CS_Property CreateSourcePropertyCallback(
    CS_Source source, llvm::StringRef name, CS_PropertyType type,
    std::function<void(llvm::StringRef name, CS_Property property)> onChange,
    CS_Status* status);
void RemoveSourceProperty(CS_Source source, llvm::StringRef name,
                          CS_Status* status);

//
// Sink Creation Functions
//
CS_Sink CreateHTTPSink(llvm::StringRef name, llvm::StringRef listenAddress,
                       int port, CS_Status* status);
CS_Sink CreateCvSink(llvm::StringRef name, CS_Status* status);
CS_Sink CreateCvSinkCallback(llvm::StringRef name,
                             std::function<void(uint64_t time)> processFrame,
                             CS_Status* status);

//
// Sink Functions
//
std::string GetSinkName(CS_Sink sink, CS_Status* status);
void GetSinkName(CS_Sink sink, llvm::SmallVectorImpl<char>& name,
                 CS_Status* status);
std::string GetSinkDescription(CS_Sink sink, CS_Status* status);
void GetSinkDescription(CS_Sink sink, llvm::SmallVectorImpl<char>& desc,
                        CS_Status* status);
void SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status);
CS_Source GetSinkSource(CS_Sink sink, CS_Status* status);
CS_Sink CopySink(CS_Sink sink, CS_Status* status);
void ReleaseSink(CS_Sink sink, CS_Status* status);

//
// Server Sink (e.g. HTTP) Functions
//
void SetSinkSourceChannel(CS_Sink sink, int channel, CS_Status* status);

//
// OpenCV Sink Functions
//
uint64_t SinkWaitForFrame(CS_Sink sink, CS_Status* status);
bool GetSinkImage(CS_Sink sink, int channel, cv::Mat* image, CS_Status* status);
uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat* image, CS_Status* status);
std::string GetSinkError(CS_Sink sink, CS_Status* status);
void GetSinkError(CS_Sink sink, llvm::SmallVectorImpl<char> msg,
                  CS_Status* status);
void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status);

//
// Listener Functions
//
CS_Listener AddSourceListener(
    std::function<void(llvm::StringRef name, CS_Source source, int event)>
        callback,
    int eventMask, CS_Status* status);

void RemoveSourceListener(CS_Listener handle, CS_Status* status);

CS_Listener AddSinkListener(
    std::function<void(llvm::StringRef name, CS_Sink sink, int event)> callback,
    int eventMask, CS_Status* status);

void RemoveSinkListener(CS_Listener handle, CS_Status* status);

//
// Utility Functions
//
void EnumerateSourceHandles(llvm::SmallVectorImpl<CS_Source>& handles,
                            CS_Status* status);
void EnumerateSinkHandles(llvm::SmallVectorImpl<CS_Sink>& handles,
                          CS_Status* status);

}  // namespace cs

#endif  /* CAMERASERVER_CPP_H_ */
