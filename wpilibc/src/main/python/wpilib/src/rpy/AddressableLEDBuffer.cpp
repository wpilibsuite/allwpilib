// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "rpy/AddressableLEDBuffer.h"

#include <span>
#include <stdexcept>

namespace wpi {

void AddressableLEDBuffer::SetRGB(size_t index, int r, int g, int b) {
  m_buffer.at(index).SetRGB(r, g, b);
}

void AddressableLEDBuffer::SetHSV(size_t index, int h, int s, int v) {
  m_buffer.at(index).SetHSV(h, s, v);
}

void AddressableLEDBuffer::SetLED(size_t index, const wpi::util::Color& color) {
  m_buffer.at(index).SetLED(color);
}

void AddressableLEDBuffer::SetLED(size_t index,
                                  const wpi::util::Color8Bit& color) {
  m_buffer.at(index).SetLED(color);
}

int AddressableLEDBuffer::GetRed(size_t index) const {
  return m_buffer.at(index).r;
}

int AddressableLEDBuffer::GetGreen(size_t index) const {
  return m_buffer.at(index).g;
}

int AddressableLEDBuffer::GetBlue(size_t index) const {
  return m_buffer.at(index).b;
}

wpi::util::Color AddressableLEDBuffer::GetLED(size_t index) const {
  const auto& led = m_buffer.at(index);
  return wpi::util::Color{led.r / 255.0, led.g / 255.0, led.b / 255.0};
}

wpi::util::Color8Bit AddressableLEDBuffer::GetLED8Bit(size_t index) const {
  const auto& led = m_buffer.at(index);
  return wpi::util::Color8Bit{led.r, led.g, led.b};
}

AddressableLED::LEDData& AddressableLEDBuffer::at(size_t index) {
  return m_buffer.at(index);
}

AddressableLED::LEDData& AddressableLEDBuffer::operator[](size_t index) {
  return m_buffer.at(index);
}

const AddressableLED::LEDData& AddressableLEDBuffer::operator[](
    size_t index) const {
  return m_buffer.at(index);
}

void AddressableLEDBuffer::View::SetRGB(size_t index, int r, int g, int b) {
  at(index).SetRGB(r, g, b);
}

void AddressableLEDBuffer::View::SetHSV(size_t index, int h, int s, int v) {
  at(index).SetHSV(h, s, v);
}

void AddressableLEDBuffer::View::SetLED(size_t index,
                                        const wpi::util::Color& color) {
  at(index).SetLED(color);
}

void AddressableLEDBuffer::View::SetLED(size_t index,
                                        const wpi::util::Color8Bit& color) {
  at(index).SetLED(color);
}

AddressableLED::LEDData& AddressableLEDBuffer::View::at(size_t index) {
  // std::span::at doesn't exist until C++26
  if (index >= m_data.size()) {
    throw std::out_of_range("Index out of range");
  }
  return m_data[index];
}

AddressableLED::LEDData& AddressableLEDBuffer::View::operator[](size_t index) {
  return at(index);
}

const AddressableLED::LEDData& AddressableLEDBuffer::View::at(
    size_t index) const {
  // std::span::at doesn't exist until C++26
  if (index >= m_data.size()) {
    throw std::out_of_range("Index out of range");
  }
  return m_data[index];
}

const AddressableLED::LEDData& AddressableLEDBuffer::View::operator[](
    size_t index) const {
  return at(index);
}

wpi::util::Color AddressableLEDBuffer::View::GetLED(size_t index) const {
  const auto& led = at(index);
  return wpi::util::Color{led.r / 255.0, led.g / 255.0, led.b / 255.0};
}

wpi::util::Color8Bit AddressableLEDBuffer::View::GetLED8Bit(
    size_t index) const {
  const auto& led = at(index);
  return wpi::util::Color8Bit{led.r, led.g, led.b};
}

AddressableLEDBuffer::View::View(std::span<AddressableLED::LEDData> data)
    : m_data(data) {}

AddressableLEDBuffer::View AddressableLEDBuffer::CreateView(
    pybind11::slice slice) {
  size_t start = 0, stop = 0, step = 0, slicelength = 0;
  slice.compute(m_buffer.size(), &start, &stop, &step, &slicelength);
  if (step != 1) {
    throw std::out_of_range("step != 1");
  }
  if (!slicelength) {
    throw std::out_of_range("zero length view");
  }
  return View(std::span(m_buffer).subspan(start, slicelength));
}

}  // namespace wpi
