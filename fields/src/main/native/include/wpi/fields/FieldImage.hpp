// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/util/SymbolExports.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::fields {

class FieldImage;

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const FieldImage& image);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, FieldImage& image);

/** Field image metadata. */
class WPILIB_DLLEXPORT FieldImage {
 public:
  /** Constructs empty field image metadata. */
  FieldImage() = default;

  /**
   * Constructs field image metadata.
   *
   * @param path Field image path. JSON files store this relative to the JSON
   *             file's directory; built-in fields store the image resource path
   *             relative to BASE_RESOURCE_DIR.
   * @param top Top field boundary in image pixels.
   * @param left Left field boundary in image pixels.
   * @param bottom Bottom field boundary in image pixels.
   * @param right Right field boundary in image pixels.
   */
  FieldImage(std::string_view path, int top, int left, int bottom, int right);

  /**
   * Gets the field image path.
   *
   * JSON-loaded fields preserve the path from JSON, which is relative to the
   * JSON file's directory. Built-in fields return the image resource path
   * relative to BASE_RESOURCE_DIR, which can be passed to GetFieldImage().
   *
   * @return Field image path.
   */
  std::string_view GetPath() const;

  /**
   * Gets the top field boundary in image pixels.
   *
   * @return Top field boundary in image pixels.
   */
  int GetTop() const;

  /**
   * Gets the left field boundary in image pixels.
   *
   * @return Left field boundary in image pixels.
   */
  int GetLeft() const;

  /**
   * Gets the bottom field boundary in image pixels.
   *
   * @return Bottom field boundary in image pixels.
   */
  int GetBottom() const;

  /**
   * Gets the right field boundary in image pixels.
   *
   * @return Right field boundary in image pixels.
   */
  int GetRight() const;

  /**
   * Checks equality between this FieldImage and another object.
   */
  bool operator==(const FieldImage&) const = default;

 private:
  std::string m_path;
  int m_top = 0;
  int m_left = 0;
  int m_bottom = 0;
  int m_right = 0;

  friend void to_json(wpi::util::json& json, const FieldImage& image);
  friend void from_json(const wpi::util::json& json, FieldImage& image);
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const FieldImage& image);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, FieldImage& image);

}  // namespace wpi::fields
