/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CameraSource.h"

using namespace cs;

class CameraSource::Impl {
 public:
  int id;
  std::string description;
  uint64_t lastFrameTime = 0;
  int nChannels = 0;
  bool connected = false;
};

CameraSource::CameraSource(llvm::StringRef name, std::unique_ptr<Impl> impl)
    : m_name(name), m_impl(std::move(impl)) {}

CameraSource::~CameraSource() {}

std::shared_ptr<CameraSource> CameraSource::CreateUSB(llvm::StringRef name,
                                                      int dev) {
  std::unique_ptr<Impl> impl{new Impl};
  // TODO
  return std::make_shared<CameraSource>(name, std::move(impl));
}

std::shared_ptr<CameraSource> CameraSource::CreateUSB(llvm::StringRef name,
                                                      llvm::StringRef path) {
  std::unique_ptr<Impl> impl{new Impl};
  // TODO
  return std::make_shared<CameraSource>(name, std::move(impl));
}

std::shared_ptr<CameraSource> CameraSource::CreateHTTP(llvm::StringRef name,
                                                       llvm::StringRef url) {
  std::unique_ptr<Impl> impl{new Impl};
  // TODO
  return std::make_shared<CameraSource>(name, std::move(impl));
}

std::shared_ptr<CameraSource> CameraSource::CreateUser(llvm::StringRef name,
                                                       int nChannels) {
  std::unique_ptr<Impl> impl{new Impl};
  // TODO
  impl->nChannels = nChannels;
  return std::make_shared<CameraSource>(name, std::move(impl));
}

std::vector<std::shared_ptr<CameraSource>> CameraSource::EnumerateSources() {
  // TODO
  return std::vector<std::shared_ptr<CameraSource>>();
}

std::vector<USBCameraInfo> CameraSource::EnumerateUSB() {
  return std::vector<USBCameraInfo>();
}

int CameraSource::AddCreateListener(
    std::function<void(std::shared_ptr<CameraSource>, int)> callback,
    int eventMask) {
  // TODO
  return 0;
}

void CameraSource::RemoveCreateListener(int handle) {
  // TODO
}

void CameraSource::PutImage(int channel, cv::Mat* image) {
  // TODO
}

void CameraSource::NotifyFrame() {
  // TODO
}

uint64_t CameraSource::WaitForFrame() const {
  // TODO
  return 0;
}

uint64_t CameraSource::GetImage(int channel, cv::Mat* image) const {
  // TODO
  return 0;
}

std::string CameraSource::GetDescription() const {
  return m_impl->description;
}

uint64_t CameraSource::GetLastFrameTime() const {
  return m_impl->lastFrameTime;
}

int CameraSource::GetNumChannels() const {
  return m_impl->nChannels;
}

bool CameraSource::IsConnected() const {
  return m_impl->connected;
}
