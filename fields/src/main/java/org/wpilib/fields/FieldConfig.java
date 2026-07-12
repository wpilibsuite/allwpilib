// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import io.avaje.jsonb.Json;
import io.avaje.jsonb.Jsonb;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;

@Json
public class FieldConfig {
  public static class Corners {
    @Json.Property("top-left")
    public double[] topLeft;

    @Json.Property("bottom-right")
    public double[] bottomRight;
  }

  @Json.Property("game")
  public String game;

  @Json.Property("field-image")
  public String fieldImage;

  @Json.Property("field-corners")
  public Corners fieldCorners;

  @Json.Property("field-size")
  public double[] fieldSize;

  @Json.Property("field-unit")
  public String fieldUnit;

  @Json.Property("program")
  public String program;

  public FieldConfig() {}

  public URL getImageUrl() {
    return getClass().getResource(Fields.BASE_RESOURCE_DIR + program + "/" + fieldImage);
  }

  public InputStream getImageAsStream() {
    return getClass().getResourceAsStream(Fields.BASE_RESOURCE_DIR + program + "/" + fieldImage);
  }

  /**
   * Loads a predefined field configuration from a resource file.
   *
   * @param field The predefined field
   * @return The field configuration
   * @throws IOException Throws if the file could not be loaded
   */
  public static FieldConfig loadField(Fields field) throws IOException {
    return loadFromResource(field.resourceFile);
  }

  /**
   * Loads a field configuration from a file on disk.
   *
   * @param file The json file to load
   * @return The field configuration
   * @throws IOException Throws if the file could not be loaded
   */
  public static FieldConfig loadFromFile(Path file) throws IOException {
    try (BufferedReader reader = Files.newBufferedReader(file)) {
      return Jsonb.instance().type(FieldConfig.class).fromJson(reader);
    }
  }

  /**
   * Loads a field configuration from a resource file located inside the programs jar file.
   *
   * @param resourcePath The path to the resource file
   * @return The field configuration
   * @throws IOException Throws if the resource could not be loaded
   */
  public static FieldConfig loadFromResource(String resourcePath) throws IOException {
    try (InputStream stream = FieldConfig.class.getResourceAsStream(resourcePath)) {
      return Jsonb.instance().type(FieldConfig.class).fromJson(stream);
    }
  }
}
