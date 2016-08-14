/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CameraSink.h"

using namespace cs;

class CameraSink::Impl {
 public:
  int id;
  std::string description;
};

CameraSink::CameraSink(llvm::StringRef name, std::unique_ptr<Impl> impl)
    : m_name(name), m_impl(std::move(impl)) {}

CameraSink::~CameraSink() {}

std::shared_ptr<CameraSink> CameraSink::CreateHTTP(
    llvm::StringRef name, llvm::StringRef listenAddress, int port) {
  std::unique_ptr<Impl> impl{new Impl};
  // TODO
  return std::make_shared<CameraSink>(name, std::move(impl));
}

std::shared_ptr<CameraSink> CameraSink::CreateRTSP(
    llvm::StringRef name, llvm::StringRef listenAddress, int port) {
  std::unique_ptr<Impl> impl{new Impl};
  // TODO
  return std::make_shared<CameraSink>(name, std::move(impl));
}

std::vector<std::shared_ptr<CameraSink>> CameraSink::EnumerateSinks() {
  // TODO
  return std::vector<std::shared_ptr<CameraSink>>();
}

int CameraSink::AddCreateListener(
    std::function<void(std::shared_ptr<CameraSink>, int)> callback,
    int eventMask) {
  // TODO
  return 0;
}

void CameraSink::RemoveCreateListener(int handle) {
  // TODO
}

void CameraSink::SetSource(std::shared_ptr<CameraSource> source, int channel) {
  // TODO
}

std::string CameraSink::GetDescription() const {
  return m_impl->description;
}
