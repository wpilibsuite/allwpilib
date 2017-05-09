/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "HAL/SerialPort.h"
#include "HAL/cpp/priority_mutex.h"
#include "llvm/SmallString.h"
#include "llvm/SmallVector.h"

namespace hal {
class SerialHelper {
 public:
  SerialHelper();

  std::string GetVISASerialPortName(HAL_SerialPort port, int32_t* status);
  std::string GetOSSerialPortName(HAL_SerialPort port, int32_t* status);

  std::vector<std::string> GetVISASerialPortList(int32_t* status);
  std::vector<std::string> GetOSSerialPortList(int32_t* status);

 private:
  void SortHubPathVector();
  void CoiteratedSort(llvm::SmallVectorImpl<llvm::SmallString<16>>& vec);
  void QueryHubPaths(int32_t* status);

  int32_t GetIndexForPort(HAL_SerialPort port, int32_t* status);

  // Vectors to hold data before sorting.
  // Note we will most likely have at max 2 instances, and the longest string
  // is around 12, so these should never touch the heap;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_visaResource;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_osResource;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_unsortedHubPath;
  llvm::SmallVector<llvm::SmallString<16>, 4> m_sortedHubPath;

  int32_t m_resourceHandle;

  static hal::priority_mutex m_nameMutex;
  static std::string m_usbNames[2];
};
}  // namespace hal
