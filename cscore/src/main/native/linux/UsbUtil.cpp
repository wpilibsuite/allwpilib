// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "UsbUtil.h"

#include <fcntl.h>
#include <libgen.h>
#include <sys/ioctl.h>

#include <string>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "Instance.h"

namespace cs {

static std::string GetUsbNameFromFile(int vendor, int product) {
  int fd = open("/var/lib/usbutils/usb.ids", O_RDONLY);
  if (fd < 0) {
    return {};
  }

  wpi::SmallString<128> buf;
  wpi::raw_fd_istream is{fd, true};

  // build vendor and product 4-char hex strings
  auto vendorStr = fmt::format("{:04x}", vendor);
  auto productStr = fmt::format("{:04x}", product);

  // scan file
  wpi::SmallString<128> lineBuf;
  bool foundVendor = false;
  for (;;) {
    auto line = is.getline(lineBuf, 4096);
    if (is.has_error()) {
      break;
    }

    if (line.empty()) {
      continue;
    }

    // look for vendor at start of line
    if (wpi::starts_with(line, vendorStr)) {
      foundVendor = true;
      buf += wpi::trim(wpi::substr(line, 5));
      buf += ' ';
      continue;
    }

    if (foundVendor) {
      // next vendor, but didn't match product?
      if (line[0] != '\t') {
        buf += "Unknown";
        return std::string{buf};
      }

      // look for product
      if (wpi::starts_with(wpi::substr(line, 1), productStr)) {
        buf += wpi::trim(wpi::substr(line, 6));
        return std::string{buf};
      }
    }
  }

  return {};
}

std::string GetUsbNameFromId(int vendor, int product) {
  // try reading usb.ids
  std::string rv = GetUsbNameFromFile(vendor, product);
  if (!rv.empty()) {
    return rv;
  }

  // Fall back to internal database
  switch (vendor) {
    case 0x046d: {
      std::string_view productStr;
      switch (product) {
        case 0x0802:
          productStr = "Webcam C200";
          break;
        case 0x0804:
          productStr = "Webcam C250";
          break;
        case 0x0805:
          productStr = "Webcam C300";
          break;
        case 0x0807:
          productStr = "Webcam B500";
          break;
        case 0x0808:
          productStr = "Webcam C600";
          break;
        case 0x0809:
          productStr = "Webcam Pro 9000";
          break;
        case 0x080a:
          productStr = "Portable Webcam C905";
          break;
        case 0x080f:
          productStr = "Webcam C120";
          break;
        case 0x0819:
          productStr = "Webcam C210";
          break;
        case 0x081b:
          productStr = "Webcam C310";
          break;
        case 0x081d:
          productStr = "HD Webcam C510";
          break;
        case 0x0821:
          productStr = "HD Webcam C910";
          break;
        case 0x0825:
          productStr = "Webcam C270";
          break;
        case 0x0826:
          productStr = "HD Webcam C525";
          break;
        case 0x0828:
          productStr = "HD Webcam B990";
          break;
        case 0x082b:
          productStr = "Webcam C170";
          break;
        case 0x082d:
          productStr = "HD Pro Webcam C920";
          break;
        case 0x0836:
          productStr = "B525 HD Webcam";
          break;
        case 0x0843:
          productStr = "Webcam C930e";
          break;
      }
      return fmt::format("Logitech, Inc. {}", productStr);
    }
  }

  return {};
}

int CheckedIoctl(int fd, unsigned long req, void* data,  // NOLINT(runtime/int)
                 const char* name, const char* file, int line, bool quiet) {
  int retval = ioctl(fd, req, data);
  if (!quiet && retval < 0) {
    WPI_ERROR(Instance::GetInstance().logger, "ioctl {} failed at {}:{}: {}",
              name, fs::path{file}.filename().string(), line,
              std::strerror(errno));
  }
  return retval;
}

}  // namespace cs
