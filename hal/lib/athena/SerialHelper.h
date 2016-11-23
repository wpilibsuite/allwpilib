/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <string>

#include "HAL/SerialPort.h"
#include "HAL/cpp/priority_mutex.h"
#include "llvm/SmallString.h"
#include "llvm/SmallVector.h"

namespace hal {
class SerialHelper {
 public:
  explicit SerialHelper(int32_t resourceHandle);

  std::string GetSerialPortName(HAL_SerialPort port, int32_t* status);

 private:
  void SortHubPathVector();
  void CoiteratedSort(llvm::SmallVectorImpl<llvm::SmallString<16>>& vec);
  void QueryHubPaths(int32_t* status);

  // Vectors to hold data before sorting.
  // Note we will most likely have at max 2 instances, and the longest string
  // is around 12, so these should never touch the heap;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_visaResource;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_osResource;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_unsortedHubPath;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_sortedHubPath;

  int32_t m_resourceHandle;

  static priority_mutex m_nameMutex;
  static std::string m_usbNames[2];
};
}  // namespace hal
