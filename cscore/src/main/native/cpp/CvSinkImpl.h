/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CVSINKIMPL_H_
#define CSCORE_CVSINKIMPL_H_

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <llvm/SmallVector.h>
#include <llvm/StringRef.h>
#include <llvm/raw_ostream.h>
#include <support/raw_istream.h>
#include <support/raw_socket_ostream.h>
#include <tcpsockets/NetworkAcceptor.h>
#include <tcpsockets/NetworkStream.h>

#include "SinkImpl.h"

namespace cs {

class SourceImpl;

class CvSinkImpl : public SinkImpl {
 public:
  explicit CvSinkImpl(llvm::StringRef name);
  CvSinkImpl(llvm::StringRef name,
             std::function<void(uint64_t time)> processFrame);
  ~CvSinkImpl() override;

  void Stop();

  uint64_t GrabFrame(cv::Mat& image);
  uint64_t GrabFrame(cv::Mat& image, double timeout);

 private:
  void ThreadMain();

  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_thread;
  std::function<void(uint64_t time)> m_processFrame;
};

}  // namespace cs

#endif  // CSCORE_CVSINKIMPL_H_
