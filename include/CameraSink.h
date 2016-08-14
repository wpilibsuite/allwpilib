/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASINK_H_
#define CAMERASINK_H_

#include <functional>
#include <memory>
#include <string>

#include "llvm/StringRef.h"

namespace cs {

class CameraSource;

class CameraSink {
  class Impl;

 public:
  CameraSink(llvm::StringRef name, std::unique_ptr<Impl> impl);
  CameraSink(const CameraSink&) = delete;
  CameraSink& operator=(const CameraSink&) = delete;
  ~CameraSink();

  static std::shared_ptr<CameraSink> CreateHTTP(llvm::StringRef name,
                                                llvm::StringRef listenAddress,
                                                int port);
  static std::shared_ptr<CameraSink> CreateRTSP(llvm::StringRef name,
                                                llvm::StringRef listenAddress,
                                                int port);

  static std::vector<std::shared_ptr<CameraSink>> EnumerateSinks();

  static int AddCreateListener(
      std::function<void(std::shared_ptr<CameraSink>, int)> callback,
      int eventMask);
  static void RemoveCreateListener(int handle);

  void SetSource(std::shared_ptr<CameraSource> source, int channel);

  llvm::StringRef GetName() const { return m_name; }
  std::string GetDescription() const;

 private:
  std::string m_name;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace cs

#endif  /* CAMERASINK_H_ */
