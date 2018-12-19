/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/cpp/SerialHelper.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include <wpi/FileSystem.h>
#include <wpi/StringRef.h>

#include "../visa/visa.h"
#include "hal/Errors.h"

constexpr const char* OnboardResourceVISA = "ASRL1::INSTR";
constexpr const char* MxpResourceVISA = "ASRL2::INSTR";

constexpr const char* OnboardResourceOS = "/dev/ttyS0";
constexpr const char* MxpResourceOS = "/dev/ttyS1";

namespace hal {

std::string SerialHelper::m_usbNames[2]{"", ""};

wpi::mutex SerialHelper::m_nameMutex;

SerialHelper::SerialHelper() {
  viOpenDefaultRM(reinterpret_cast<ViSession*>(&m_resourceHandle));
}

std::string SerialHelper::GetVISASerialPortName(HAL_SerialPort port,
                                                int32_t* status) {
  if (port == HAL_SerialPort::HAL_SerialPort_Onboard) {
    return OnboardResourceVISA;
  } else if (port == HAL_SerialPort::HAL_SerialPort_MXP) {
    return MxpResourceVISA;
  }

  QueryHubPaths(status);

  // If paths are empty or status error, return error
  if (*status != 0 || m_visaResource.empty() || m_osResource.empty() ||
      m_sortedHubPath.empty()) {
    *status = HAL_SERIAL_PORT_NOT_FOUND;
    return "";
  }

  int32_t visaIndex = GetIndexForPort(port, status);
  if (visaIndex == -1) {
    *status = HAL_SERIAL_PORT_NOT_FOUND;
    return "";
    // Error
  } else {
    return m_visaResource[visaIndex].str();
  }
}

std::string SerialHelper::GetOSSerialPortName(HAL_SerialPort port,
                                              int32_t* status) {
  if (port == HAL_SerialPort::HAL_SerialPort_Onboard) {
    return OnboardResourceOS;
  } else if (port == HAL_SerialPort::HAL_SerialPort_MXP) {
    return MxpResourceOS;
  }

  QueryHubPaths(status);

  // If paths are empty or status error, return error
  if (*status != 0 || m_visaResource.empty() || m_osResource.empty() ||
      m_sortedHubPath.empty()) {
    *status = HAL_SERIAL_PORT_NOT_FOUND;
    return "";
  }

  int32_t osIndex = GetIndexForPort(port, status);
  if (osIndex == -1) {
    *status = HAL_SERIAL_PORT_NOT_FOUND;
    return "";
    // Error
  } else {
    return m_osResource[osIndex].str();
  }
}

std::vector<std::string> SerialHelper::GetVISASerialPortList(int32_t* status) {
  std::vector<std::string> retVec;

  // Always add 2 onboard ports
  retVec.emplace_back(OnboardResourceVISA);
  retVec.emplace_back(MxpResourceVISA);

  QueryHubPaths(status);

  // If paths are empty or status error, return only onboard list
  if (*status != 0 || m_visaResource.empty() || m_osResource.empty() ||
      m_sortedHubPath.empty()) {
    *status = 0;
    return retVec;
  }

  for (auto& i : m_visaResource) {
    retVec.emplace_back(i.str());
  }

  return retVec;
}

std::vector<std::string> SerialHelper::GetOSSerialPortList(int32_t* status) {
  std::vector<std::string> retVec;

  // Always add 2 onboard ports
  retVec.emplace_back(OnboardResourceOS);
  retVec.emplace_back(MxpResourceOS);

  QueryHubPaths(status);

  // If paths are empty or status error, return only onboard list
  if (*status != 0 || m_visaResource.empty() || m_osResource.empty() ||
      m_sortedHubPath.empty()) {
    *status = 0;
    return retVec;
  }

  for (auto& i : m_osResource) {
    retVec.emplace_back(i.str());
  }

  return retVec;
}

void SerialHelper::SortHubPathVector() {
  m_sortedHubPath.clear();
  m_sortedHubPath = m_unsortedHubPath;
  std::sort(m_sortedHubPath.begin(), m_sortedHubPath.end(),
            [](const wpi::SmallVectorImpl<char>& lhs,
               const wpi::SmallVectorImpl<char>& rhs) -> int {
              wpi::StringRef lhsRef(lhs.begin(), lhs.size());
              wpi::StringRef rhsRef(rhs.begin(), rhs.size());
              return lhsRef.compare(rhsRef);
            });
}

void SerialHelper::CoiteratedSort(
    wpi::SmallVectorImpl<wpi::SmallString<16>>& vec) {
  wpi::SmallVector<wpi::SmallString<16>, 4> sortedVec;
  for (auto& str : m_sortedHubPath) {
    for (size_t i = 0; i < m_unsortedHubPath.size(); i++) {
      if (wpi::StringRef{m_unsortedHubPath[i].begin(),
                         m_unsortedHubPath[i].size()}
              .equals(wpi::StringRef{str.begin(), str.size()})) {
        sortedVec.push_back(vec[i]);
        break;
      }
    }
  }
  vec = sortedVec;
}

void SerialHelper::QueryHubPaths(int32_t* status) {
  // VISA resource matching string
  const char* str = "?*";
  // Items needed for VISA
  ViUInt32 retCnt = 0;
  ViFindList viList = 0;
  ViChar desc[VI_FIND_BUFLEN];
  *status = viFindRsrc(m_resourceHandle, const_cast<char*>(str), &viList,
                       &retCnt, desc);

  if (*status < 0) {
    // Handle the bad status elsewhere
    // Note let positive statii (warnings) continue
    goto done;
  }
  // Status might be positive, so reset it to 0
  *status = 0;

  // Storage buffer for Visa call
  char osName[256];

  // Loop through all returned VISA objects.
  // Increment the internal VISA ptr every loop
  for (size_t i = 0; i < retCnt; i++, viFindNext(viList, desc)) {
    // Ignore any matches to the 2 onboard ports
    if (std::strcmp(OnboardResourceVISA, desc) == 0 ||
        std::strcmp(MxpResourceVISA, desc) == 0) {
      continue;
    }

    // Open the resource, grab its interface name, and close it.
    ViSession vSession;
    *status = viOpen(m_resourceHandle, desc, VI_NULL, VI_NULL, &vSession);
    if (*status < 0) goto done;
    *status = 0;

    *status = viGetAttribute(vSession, VI_ATTR_INTF_INST_NAME, &osName);
    // Ignore an error here, as we want to close the session on an error
    // Use a seperate close variable so we can check
    ViStatus closeStatus = viClose(vSession);
    if (*status < 0) goto done;
    if (closeStatus < 0) goto done;
    *status = 0;

    // split until (/dev/
    wpi::StringRef devNameRef = wpi::StringRef{osName}.split("(/dev/").second;
    // String not found, continue
    if (devNameRef.equals("")) continue;

    // Split at )
    wpi::StringRef matchString = devNameRef.split(')').first;
    if (matchString.equals(devNameRef)) continue;

    // Search directories to get a list of system accessors
    // The directories we need are not symbolic, so we can safely
    // disable symbolic links.
    std::error_code ec;
    for (auto p = wpi::sys::fs::recursive_directory_iterator(
             "/sys/devices/soc0", ec, false);
         p != wpi::sys::fs::recursive_directory_iterator(); p.increment(ec)) {
      if (ec) break;
      wpi::StringRef path{p->path()};
      if (path.find("amba") == wpi::StringRef::npos) continue;
      if (path.find("usb") == wpi::StringRef::npos) continue;
      if (path.find(matchString) == wpi::StringRef::npos) continue;

      wpi::SmallVector<wpi::StringRef, 16> pathSplitVec;
      // Split path into individual directories
      path.split(pathSplitVec, '/', -1, false);

      // Find each individual item index
      int findusb = -1;
      int findtty = -1;
      int findregex = -1;
      for (size_t i = 0; i < pathSplitVec.size(); i++) {
        if (findusb == -1 && pathSplitVec[i].equals("usb1")) {
          findusb = i;
        }
        if (findtty == -1 && pathSplitVec[i].equals("tty")) {
          findtty = i;
        }
        if (findregex == -1 && pathSplitVec[i].equals(matchString)) {
          findregex = i;
        }
      }

      // Get the index for our device
      int hubIndex = findtty;
      if (findtty == -1) hubIndex = findregex;

      int devStart = findusb + 1;

      if (hubIndex < devStart) continue;

      // Add our devices to our list
      m_unsortedHubPath.emplace_back(
          wpi::StringRef{pathSplitVec[hubIndex - 2]});
      m_visaResource.emplace_back(desc);
      m_osResource.emplace_back(
          wpi::StringRef{osName}.split("(").second.split(")").first);
      break;
    }
  }

  SortHubPathVector();

  CoiteratedSort(m_visaResource);
  CoiteratedSort(m_osResource);
done:
  viClose(viList);
}

int32_t SerialHelper::GetIndexForPort(HAL_SerialPort port, int32_t* status) {
  // Hold lock whenever we're using the names array
  std::lock_guard<wpi::mutex> lock(m_nameMutex);

  std::string portString = m_usbNames[port - 2];

  wpi::SmallVector<int32_t, 4> indices;

  // If port has not been assigned, find the one to assign
  if (portString.empty()) {
    for (size_t i = 0; i < 2; i++) {
      // Remove all used ports
      auto idx = std::find(m_sortedHubPath.begin(), m_sortedHubPath.end(),
                           m_usbNames[i]);
      if (idx != m_sortedHubPath.end()) {
        // found
        m_sortedHubPath.erase(idx);
      }
      if (m_usbNames[i] == "") {
        indices.push_back(i);
      }
    }

    int32_t idx = -1;
    for (size_t i = 0; i < indices.size(); i++) {
      if (indices[i] == port - 2) {
        idx = i;
        break;
      }
    }

    if (idx == -1) {
      *status = HAL_SERIAL_PORT_NOT_FOUND;
      return -1;
    }

    if (idx >= static_cast<int32_t>(m_sortedHubPath.size())) {
      *status = HAL_SERIAL_PORT_NOT_FOUND;
      return -1;
    }

    portString = m_sortedHubPath[idx].str();
    m_usbNames[port - 2] = portString;
  }

  int retIndex = -1;

  for (size_t i = 0; i < m_sortedHubPath.size(); i++) {
    if (m_sortedHubPath[i].equals(portString)) {
      retIndex = i;
      break;
    }
  }

  return retIndex;
}

}  // namespace hal
