/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_USBCAMERABUFFER_H_
#define CAMERASERVER_USBCAMERABUFFER_H_

#ifdef __linux__
#include <sys/mman.h>
#endif

namespace cs {

class USBCameraBuffer {
 public:
  USBCameraBuffer() noexcept : m_data{nullptr}, m_length{0} {}
  USBCameraBuffer(USBCameraBuffer&& other) noexcept : USBCameraBuffer() {
    swap(*this, other);
  }
  USBCameraBuffer& operator=(USBCameraBuffer&& other) noexcept {
    swap(*this, other);
    return *this;
  }
  USBCameraBuffer(const USBCameraBuffer&) = delete;
  USBCameraBuffer& operator=(const USBCameraBuffer&) = delete;

#ifdef __linux__
  USBCameraBuffer(int fd, size_t length, off_t offset) noexcept
      : m_length{length} {
    m_data =
        mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (m_data == MAP_FAILED) {
      m_data = nullptr;
      m_length = 0;
    }
  }

  ~USBCameraBuffer() {
    if (m_data) munmap(m_data, m_length);
  }
#endif

  friend void swap(USBCameraBuffer& first, USBCameraBuffer& second) noexcept {
    using std::swap;
    swap(first.m_data, second.m_data);
    swap(first.m_length, second.m_length);
  }

  void* m_data;
  size_t m_length;
};

}  // namespace cs

#endif  // CAMERASERVER_USBCAMERABUFFER_H_
