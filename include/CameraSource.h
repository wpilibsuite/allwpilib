/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASOURCE_H_
#define CAMERASOURCE_H_

#include <functional>
#include <memory>
#include <string>

#include <stdint.h>

#include "llvm/StringRef.h"

namespace cv {
class Mat;
}

namespace cs {

struct USBCameraInfo {
  int dev;
  std::string path;
  std::string name;
  int channels;
};

class CameraSource {
  class Impl;

 public:
  CameraSource(llvm::StringRef name, std::unique_ptr<Impl> impl);
  CameraSource(const CameraSource&) = delete;
  CameraSource& operator=(const CameraSource&) = delete;
  ~CameraSource();

  static std::shared_ptr<CameraSource> CreateUSB(llvm::StringRef name, int dev);
  static std::shared_ptr<CameraSource> CreateUSB(llvm::StringRef name,
                                                 llvm::StringRef path);
  static std::shared_ptr<CameraSource> CreateHTTP(llvm::StringRef name,
                                                  llvm::StringRef url);
  static std::shared_ptr<CameraSource> CreateUser(llvm::StringRef name,
                                                  int nChannels);

  static std::vector<std::shared_ptr<CameraSource>> EnumerateSources();
  static std::vector<USBCameraInfo> EnumerateUSB();

  static int AddCreateListener(
      std::function<void(std::shared_ptr<CameraSource>, int)> callback,
      int eventMask);
  static void RemoveCreateListener(int handle);

  void PutImage(int channel, cv::Mat* image);
  void NotifyFrame();

  uint64_t WaitForFrame() const;
  uint64_t GetImage(int channel, cv::Mat* image) const;

  llvm::StringRef GetName() const { return m_name; }
  std::string GetDescription() const;
  uint64_t GetLastFrameTime() const;
  int GetNumChannels() const;
  bool IsConnected() const;

 private:
  std::string m_name;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace cs

#endif  /* CAMERASOURCE_H_ */
