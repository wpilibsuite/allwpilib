// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class LoadConfigTest {
  @ParameterizedTest
  @EnumSource(Fields.class)
  void testLoadFields(Fields fieldData) throws IOException {
    Field field = Assertions.assertDoesNotThrow(() -> Field.loadField(fieldData));

    Assertions.assertEquals(fieldData.name, field.getName());
    Assertions.assertEquals(fieldData.name, field.name);
    Assertions.assertEquals(fieldData.season, field.season);
    Assertions.assertEquals(fieldData.game, field.game);
    Assertions.assertEquals(fieldData.hasImage, field.hasImage());
    if (fieldData.hasImage) {
      Assertions.assertEquals(fieldData.fieldImage, field.fieldImage.path);
      Assertions.assertEquals(fieldData.top, field.fieldImage.top);
      Assertions.assertEquals(fieldData.left, field.fieldImage.left);
      Assertions.assertEquals(fieldData.bottom, field.fieldImage.bottom);
      Assertions.assertEquals(fieldData.right, field.fieldImage.right);
    } else {
      Assertions.assertNull(field.fieldImage);
    }
    Assertions.assertEquals(fieldData.length, field.getFieldLength());
    Assertions.assertEquals(fieldData.width, field.getFieldWidth());
    Assertions.assertEquals(fieldData.length, field.length);
    Assertions.assertEquals(fieldData.width, field.width);
    Assertions.assertEquals(fieldData.program, field.program);
    Assertions.assertEquals(fieldData.getImageResource(), field.getImageResource());
    Assertions.assertEquals(
        readResource(Fields.BASE_RESOURCE_DIR + fieldData.resourceFile), field.getJson());

    Field resourceField = Field.loadFromResource(Fields.BASE_RESOURCE_DIR + fieldData.resourceFile);
    Assertions.assertEquals(fieldData.name, resourceField.name);
    Assertions.assertEquals(fieldData.season, resourceField.season);
    Assertions.assertEquals(fieldData.game, resourceField.game);
    if (fieldData.hasImage) {
      Assertions.assertEquals(fieldData.getImageResource(), resourceField.getImageResource());
    }

    Assertions.assertEquals(fieldData.hasTags, field.hasTags());
    Assertions.assertEquals(fieldData.length, field.getFieldLength(), 1e-9);
    Assertions.assertEquals(fieldData.width, field.getFieldWidth(), 1e-9);
    Assertions.assertEquals(field.getTags(), field.tags == null ? java.util.List.of() : field.tags);
  }

  @Test
  void testLoadFieldWithoutDimensionsFails(@TempDir Path tempDir) throws IOException {
    String json =
        """
        {
          "name": "2027 FRC Dimensionless Field",
          "season": "2027",
          "game": "Dimensionless Field",
          "program": "frc"
        }
        """;
    Path file = tempDir.resolve("dimensionless.json");
    Files.writeString(file, json);

    Assertions.assertThrows(RuntimeException.class, () -> Field.loadFromFile(file));
  }

  @Test
  void testLoadFieldWithoutImage(@TempDir Path tempDir) throws IOException {
    String json =
        """
        {
          "name": "2027 FRC Imageless Field",
          "season": "2027",
          "game": "Imageless Field",
          "field-dimensions": {
            "length": 3.0,
            "width": 2.0
          },
          "program": "frc"
        }
        """;
    Path file = tempDir.resolve("imageless.json");
    Files.writeString(file, json);

    Field field = Field.loadFromFile(file);

    Assertions.assertEquals("2027 FRC Imageless Field", field.getName());
    Assertions.assertEquals("2027 FRC Imageless Field", field.name);
    Assertions.assertEquals("2027", field.season);
    Assertions.assertEquals("Imageless Field", field.game);
    Assertions.assertFalse(field.hasImage());
    Assertions.assertNull(field.fieldImage);
    Assertions.assertNull(field.getImageResource());
    Assertions.assertFalse(field.hasTags());
    Assertions.assertTrue(field.getTags().isEmpty());
    Assertions.assertEquals(3.0, field.length);
    Assertions.assertEquals(2.0, field.width);
    Assertions.assertEquals(json, field.getJson());
  }

  @Test
  void testResourceLoadedFieldImageIsRelativeToJsonResource() throws IOException {
    Field field = Field.loadFromResource("/custom/layout.json");

    Assertions.assertEquals("image.png", field.fieldImage.path);
    Assertions.assertEquals("/custom/image.png", field.getImageResource());
    Assertions.assertNotNull(field.getImageUrl());
    try (InputStream stream = field.getImageAsStream()) {
      Assertions.assertNotNull(stream);
      Assertions.assertEquals(
          "test image\n", new String(stream.readAllBytes(), StandardCharsets.UTF_8));
    }
  }

  private static String readResource(String resourceFile) throws IOException {
    try (InputStream stream = LoadConfigTest.class.getResourceAsStream(resourceFile)) {
      if (stream == null) {
        throw new IOException("Could not locate resource: " + resourceFile);
      }
      return new String(stream.readAllBytes(), StandardCharsets.UTF_8);
    }
  }
}
