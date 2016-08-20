/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_cpp.h"

#include <limits>

using namespace cs;

bool Property::GetBoolean() const {
  return false;  // TODO
}

void Property::SetBoolean(bool value) {
  // TODO
}

double Property::GetDouble() const {
  return 0;  // TODO
}

void Property::SetDouble(double value) {
  // TODO
}

double Property::GetMin() const {
  return std::numeric_limits<double>::min();  // TODO
}

double Property::GetMax() const {
  return std::numeric_limits<double>::max();  // TODO
}

std::string Property::GetString() const {
  return "";  // TODO
}

void Property::SetString(llvm::StringRef value) {
  // TODO
}

int Property::GetEnum() const {
  return 0;  // TODO
}

void Property::SetEnum(int value) {
  // TODO
}

std::vector<std::string> Property::GetChoices() const {
  return std::vector<std::string>();  // TODO
}

VideoSource::~VideoSource() {
  // TODO
}

std::string VideoSource::GetDescription() const {
  return "";  // TODO
}

uint64_t VideoSource::GetLastFrameTime() const {
  return 0;  // TODO
}

int VideoSource::GetNumChannels() const {
  return 0;  // TODO
}

bool VideoSource::IsConnected() const {
  return false;  // TODO
}

Property VideoSource::GetProperty(llvm::StringRef name) {
  return Property();  // TODO
}

std::vector<std::shared_ptr<VideoSource>> VideoSource::EnumerateSources() {
  return std::vector<std::shared_ptr<VideoSource>>();  // TODO
}

std::shared_ptr<CameraSource> CameraSource::CreateUSB(llvm::StringRef name,
                                                      int dev) {
  return nullptr;  // TODO
}

std::shared_ptr<CameraSource> CameraSource::CreateUSB(llvm::StringRef name,
                                                      llvm::StringRef path) {
  return nullptr;  // TODO
}

std::shared_ptr<CameraSource> CameraSource::CreateHTTP(llvm::StringRef name,
                                                       llvm::StringRef url) {
  return nullptr;  // TODO
}

std::vector<USBCameraInfo> CameraSource::EnumerateUSBCameras() {
  return std::vector<USBCameraInfo>();  // TODO
}

void CvSource::PutImage(int channel, cv::Mat* image) {
  // TODO
}

void CvSource::NotifyFrame() {
  // TODO
}

void CvSource::PutFrame(cv::Mat* image) {
  PutImage(0, image);
  NotifyFrame();
}

void CvSource::NotifyError(llvm::StringRef msg) {
  // TODO
}

void CvSource::SetConnected(bool connected) {
  // TODO
}

Property CvSource::CreateProperty(llvm::StringRef name, Property::Type type) {
  return Property();  // TODO
}

Property CvSource::CreateProperty(
    llvm::StringRef name, Property::Type type,
    std::function<void(llvm::StringRef)> onChange) {
  return Property();  // TODO
}

void CvSource::RemoveProperty(llvm::StringRef name) {
  // TODO
}

std::shared_ptr<CvSource> CvSource::Create(llvm::StringRef name,
                                           int nChannels) {
  return nullptr;  // TODO
}

VideoSink::~VideoSink() {
  // TODO
}

std::string VideoSink::GetDescription() const {
  return ""; // TODO
}

void VideoSink::SetSource(std::shared_ptr<VideoSource> source) {
  // TODO
}

Property VideoSink::GetSourceProperty(llvm::StringRef name) {
  return Property();  // TODO
}

std::vector<std::shared_ptr<VideoSink>> VideoSink::EnumerateSinks() {
  return std::vector<std::shared_ptr<VideoSink>>();  // TODO
}

void ServerSink::SetSourceChannel(int channel) {
  // TODO
}

std::shared_ptr<ServerSink> ServerSink::CreateHTTP(
    llvm::StringRef name, llvm::StringRef listenAddress, int port) {
  return nullptr;  // TODO
}

uint64_t CvSink::WaitForFrame() const {
  return 0;  // TODO
}

bool CvSink::GetImage(int channel, cv::Mat* image) const {
  return false;  // TODO
}

uint64_t CvSink::FrameGrab(cv::Mat* image) const {
  return 0;  // TODO
}

std::string CvSink::GetError() const {
  return "";  // TODO
}

void CvSink::SetEnabled(bool enabled) {
  // TODO
}

std::shared_ptr<CvSink> CvSink::Create(llvm::StringRef name) {
  return nullptr;  // TODO
}

std::shared_ptr<CvSink> CvSink::Create(
    llvm::StringRef name, std::function<void(uint64_t time)> processFrame) {
  return nullptr;  // TODO
}

int cs::AddSourceListener(
    std::function<void(llvm::StringRef name, std::shared_ptr<VideoSource>, int)>
        callback,
    int eventMask) {
  return 0;  // TODO
}

void cs::RemoveSourceListener(int handle) {
  // TODO
}

int cs::AddSinkListener(
    std::function<void(llvm::StringRef name, std::shared_ptr<VideoSink>, int)>
        callback,
    int eventMask) {
  return 0;  // TODO
}

void cs::RemoveSinkListener(int handle) {
  // TODO
}
