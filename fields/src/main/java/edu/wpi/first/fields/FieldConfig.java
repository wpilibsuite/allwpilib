// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.fields;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

public class FieldConfig {
  public static class Corners {
    @JsonProperty("top-left")
    public double[] m_topLeft;

    @JsonProperty("bottom-right")
    public double[] m_bottomRight;
  }

  @JsonProperty("game")
  public String m_game;

  @JsonProperty("field-image")
  public String m_fieldImage;

  @JsonProperty("field-corners")
  public Corners m_fieldCorners;

  @JsonProperty("field-size")
  public double[] m_fieldSize;

  @JsonProperty("field-unit")
  public String m_fieldUnit;

  public FieldConfig() {}

  public URL getImageUrl() {
    return getClass().getResource(Fields.kBaseResourceDir + m_fieldImage);
  }

  public InputStream getImageAsStream() {
    return getClass().getResourceAsStream(Fields.kBaseResourceDir + m_fieldImage);
  }

  /**
   * Loads a predefined field configuration from a resource file.
   *
   * @param field The predefined field
   * @return The field configuration
   * @throws IOException Throws if the file could not be loaded
   */
  public static FieldConfig loadField(Fields field) throws IOException {
    return loadFromResource(field.m_resourceFile);
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
      return new ObjectMapper().readerFor(FieldConfig.class).readValue(reader);
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
    try (InputStream stream = FieldConfig.class.getResourceAsStream(resourcePath);
        InputStreamReader reader = new InputStreamReader(stream, StandardCharsets.UTF_8)) {
      return new ObjectMapper().readerFor(FieldConfig.class).readValue(reader);
    }
  }
}
