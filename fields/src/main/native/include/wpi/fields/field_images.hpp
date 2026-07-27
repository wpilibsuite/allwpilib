// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>

namespace wpi::fields {

/** A built-in field image resource. */
struct FieldImageResource {
  /** Image resource path relative to BASE_RESOURCE_DIR. */
  const char* resourcePath;

  /** Gets the image resource contents. */
  std::string_view (*getImage)();
};

/**
 * Gets the built-in field images.
 *
 * Match FieldImage::GetPath() to FieldImageResource::resourcePath to correlate
 * a field JSON description with its image.
 *
 * @return Built-in field images
 */
std::span<const FieldImageResource> GetFieldImages();

/**
 * Gets a built-in field image by resource path.
 *
 * @param resourcePath Image resource path relative to BASE_RESOURCE_DIR
 * @return The image resource contents, or an empty view if it is not available
 */
std::string_view GetFieldImage(std::string_view resourcePath);

}  // namespace wpi::fields
