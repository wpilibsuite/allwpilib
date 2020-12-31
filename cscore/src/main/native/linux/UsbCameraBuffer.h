// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_USBCAMERABUFFER_H_
#define CSCORE_USBCAMERABUFFER_H_

#include <sys/mman.h>

#include <utility>

namespace cs {

class UsbCameraBuffer {
 public:
  UsbCameraBuffer() noexcept = default;
  UsbCameraBuffer(UsbCameraBuffer&& other) noexcept : UsbCameraBuffer() {
    swap(*this, other);
  }
  UsbCameraBuffer& operator=(UsbCameraBuffer&& other) noexcept {
    swap(*this, other);
    return *this;
  }
  UsbCameraBuffer(const UsbCameraBuffer&) = delete;
  UsbCameraBuffer& operator=(const UsbCameraBuffer&) = delete;

  UsbCameraBuffer(int fd, size_t length, off_t offset) noexcept
      : m_length{length} {
    m_data =
        mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (m_data == MAP_FAILED) {
      m_data = nullptr;
      m_length = 0;
    }
  }

  ~UsbCameraBuffer() {
    if (m_data) {
      munmap(m_data, m_length);
    }
  }

  friend void swap(UsbCameraBuffer& first, UsbCameraBuffer& second) noexcept {
    using std::swap;
    swap(first.m_data, second.m_data);
    swap(first.m_length, second.m_length);
  }

  void* m_data{nullptr};
  size_t m_length{0};
};

}  // namespace cs

#endif  // CSCORE_USBCAMERABUFFER_H_
