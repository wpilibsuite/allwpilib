/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_IMAGE_H_
#define CAMERASERVER_IMAGE_H_

#include <cstddef>

namespace cs {

class Image {
 public:
  enum Type {
    kUnknown = 0,
    kJpeg
  };

  Image() : m_data(nullptr), m_size(0), m_capacity(0) {}
  explicit Image(std::size_t capacity)
      : m_data(new char[capacity]), m_capacity(capacity) {}
  Image(Image&& image)
      : m_data(image.m_data),
        m_size(image.m_size),
        m_capacity(image.m_capacity) {
    image.m_data = nullptr;
    image.m_size = 0;
    image.m_capacity = 0;
  }
  ~Image() { delete[] m_data; }
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  char* data() { return m_data; }
  const char* data() const { return m_data; }
  std::size_t size() const { return m_size; }
  std::size_t capacity() const { return m_capacity; }
  Type type() const { return m_type; }

  void SetSize(std::size_t size) { m_size = size; }
  void SetType(Type type) { m_type = type; }

 private:
  char* m_data;
  std::size_t m_size;
  std::size_t m_capacity;
  Type m_type = kUnknown;
};

}  // namespace cs

#endif  // CAMERASERVER_IMAGE_H_
