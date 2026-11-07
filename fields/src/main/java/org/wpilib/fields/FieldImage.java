// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import io.avaje.jsonb.Json;
import java.util.Objects;

/** Field image metadata. */
@Json
public final class FieldImage {
  /**
   * Field image path. JSON files store this relative to the JSON file's directory; built-in fields
   * store the image resource path relative to {@link Fields#BASE_RESOURCE_DIR}.
   */
  @Json.Property("path")
  public String path;

  /** Top field boundary in image pixels. */
  @Json.Property("top")
  public int top;

  /** Left field boundary in image pixels. */
  @Json.Property("left")
  public int left;

  /** Bottom field boundary in image pixels. */
  @Json.Property("bottom")
  public int bottom;

  /** Right field boundary in image pixels. */
  @Json.Property("right")
  public int right;

  /** Constructs a field image metadata object. */
  public FieldImage() {}

  /**
   * Constructs a field image metadata object.
   *
   * @param path Field image path. JSON files store this relative to the JSON file's directory;
   *     built-in fields store the image resource path relative to {@link Fields#BASE_RESOURCE_DIR}.
   * @param top Top field boundary in image pixels.
   * @param left Left field boundary in image pixels.
   * @param bottom Bottom field boundary in image pixels.
   * @param right Right field boundary in image pixels.
   */
  public FieldImage(String path, int top, int left, int bottom, int right) {
    this.path = path;
    this.top = top;
    this.left = left;
    this.bottom = bottom;
    this.right = right;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof FieldImage image
        && Objects.equals(path, image.path)
        && top == image.top
        && left == image.left
        && bottom == image.bottom
        && right == image.right;
  }

  @Override
  public int hashCode() {
    return Objects.hash(path, top, left, bottom, right);
  }
}
