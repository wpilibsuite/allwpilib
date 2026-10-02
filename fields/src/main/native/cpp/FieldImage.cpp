// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/FieldImage.hpp"

#include "wpi/util/json.hpp"

using namespace wpi::fields;

FieldImage::FieldImage(std::string_view path, int top, int left, int bottom,
                       int right)
    : m_path{path},
      m_top{top},
      m_left{left},
      m_bottom{bottom},
      m_right{right} {}

std::string_view FieldImage::GetPath() const {
  return m_path;
}

int FieldImage::GetTop() const {
  return m_top;
}

int FieldImage::GetLeft() const {
  return m_left;
}

int FieldImage::GetBottom() const {
  return m_bottom;
}

int FieldImage::GetRight() const {
  return m_right;
}

void wpi::fields::to_json(wpi::util::json& json, const FieldImage& image) {
  json = wpi::util::json::object("path", image.m_path, "top", image.m_top,
                                 "left", image.m_left, "bottom", image.m_bottom,
                                 "right", image.m_right);
}

void wpi::fields::from_json(const wpi::util::json& json, FieldImage& image) {
  image.m_path = json.at("path").get_string();
  image.m_top = json.at("top").get_int();
  image.m_left = json.at("left").get_int();
  image.m_bottom = json.at("bottom").get_int();
  image.m_right = json.at("right").get_int();
}
