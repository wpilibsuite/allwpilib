// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import io.avaje.jsonb.Json;
import io.avaje.jsonb.Jsonb;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Supplier;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation3d;

/**
 * Class for representing a FIRST field, optional field image metadata, and optional tag poses.
 *
 * <p>The JSON format contains the top-level objects "name", "season", "game", "field-dimensions",
 * and "program", and can optionally contain "field-image" and "field-tags". The "field-dimensions"
 * object stores "length" and "width" in meters. The "field-image" object stores the image path and
 * the field boundaries in image pixels. The "field-tags" object stores all field tags contained
 * within a layout.
 *
 * <p>Pose3ds in the JSON are measured using the normal FRC coordinate system, NWU with the origin
 * at the bottom-right corner of the blue alliance wall. {@link #setOrigin(OriginPosition)} can be
 * used to change the poses returned from {@link Field#getTagPose(int)} to be from the perspective
 * of a specific alliance.
 *
 * <p>Tag poses represent the center of the tag, with a zero rotation representing a tag that is
 * upright and facing away from the (blue) alliance wall (that is, towards the opposing alliance).
 */
@Json
public final class Field {
  /** Common origin positions for the field tag coordinate system. */
  public enum OriginPosition {
    /** Blue alliance wall, right side. */
    BLUE_ALLIANCE_WALL_RIGHT_SIDE,
    /** Red alliance wall, right side. */
    RED_ALLIANCE_WALL_RIGHT_SIDE,
  }

  /** JSON representation of the physical field dimensions. */
  @Json
  static class FieldDimensions {
    /** Field length in meters. */
    @Json.Property("length")
    public double length;

    /** Field width in meters. */
    @Json.Property("width")
    public double width;

    FieldDimensions() {}

    FieldDimensions(double length, double width) {
      this.length = length;
      this.width = width;
    }
  }

  /** Human-readable field name. */
  @Json.Property("name")
  public String name;

  /** Field season. */
  @Json.Property("season")
  public String season;

  /** Game name. */
  @Json.Property("game")
  public String game;

  /** Field image metadata, or null if this field has no image. */
  @Json.Property("field-image")
  public FieldImage fieldImage;

  /** Field length in meters. */
  @Json.Ignore public double length;

  /** Field width in meters. */
  @Json.Ignore public double width;

  /** FIRST program. */
  @Json.Property("program")
  public String program;

  /** Field tag metadata, or null if this field has no tag layout. */
  @Json.Property("field-tags")
  public List<FieldTag> tags;

  /** JSON resource path for this field, or null if this field was loaded from a file. */
  @Json.Ignore public String resourceFile;

  @Json.Ignore private Pose3d m_origin = Pose3d.ZERO;
  @Json.Ignore private String m_json;
  @Json.Ignore private Supplier<String> m_jsonSupplier;

  /** Constructs an empty field object for JSON deserialization. */
  public Field() {}

  @Json.Creator
  Field(
      String name,
      String season,
      String game,
      FieldImage fieldImage,
      @Json.Alias("field-dimensions") FieldDimensions fieldDimensions,
      String program,
      List<FieldTag> tags) {
    this(
        name,
        season,
        game,
        fieldImage,
        requireFieldDimensions(fieldDimensions).length,
        requireFieldDimensions(fieldDimensions).width,
        program,
        tags);
  }

  private static FieldDimensions requireFieldDimensions(FieldDimensions fieldDimensions) {
    return Objects.requireNonNull(fieldDimensions, "field-dimensions");
  }

  private static FieldImage requireFieldImagePath(FieldImage fieldImage) {
    if (fieldImage != null) {
      Objects.requireNonNull(fieldImage.path, "field-image.path");
    }
    return fieldImage;
  }

  /**
   * Constructs a field object.
   *
   * @param name Human-readable field name.
   * @param season Field season.
   * @param game Game name.
   * @param fieldImage Field image metadata, or null if this field has no image.
   * @param fieldLength Field length in meters.
   * @param fieldWidth Field width in meters.
   * @param program FIRST program.
   * @param tags Field tag metadata, or null if this field has no tag layout.
   */
  public Field(
      String name,
      String season,
      String game,
      FieldImage fieldImage,
      double fieldLength,
      double fieldWidth,
      String program,
      List<FieldTag> tags) {
    this(name, season, game, fieldImage, fieldLength, fieldWidth, program, null, tags, null);
  }

  Field(
      String name,
      String season,
      String game,
      FieldImage fieldImage,
      double fieldLength,
      double fieldWidth,
      String program,
      String resourceFile,
      List<FieldTag> tags,
      Supplier<String> jsonSupplier) {
    this.name = name;
    this.season = season;
    this.game = game;
    this.fieldImage = requireFieldImagePath(fieldImage);
    this.length = fieldLength;
    this.width = fieldWidth;
    this.program = program;
    this.resourceFile = resourceFile;
    this.tags = tags == null ? null : List.copyOf(tags);
    m_jsonSupplier = jsonSupplier;
  }

  Field(Field other) {
    name = other.name;
    season = other.season;
    game = other.game;
    fieldImage =
        other.fieldImage == null
            ? null
            : new FieldImage(
                other.fieldImage.path,
                other.fieldImage.top,
                other.fieldImage.left,
                other.fieldImage.bottom,
                other.fieldImage.right);
    length = other.length;
    width = other.width;
    program = other.program;
    tags = other.tags == null ? null : List.copyOf(other.tags);
    resourceFile = other.resourceFile;
    m_origin = other.m_origin;
    m_json = other.m_json;
    m_jsonSupplier = other.m_jsonSupplier;
  }

  /**
   * Returns the human-readable field name.
   *
   * @return The human-readable field name.
   */
  public String getName() {
    return name;
  }

  /**
   * Returns true if this field has field image metadata.
   *
   * @return True if this field has field image metadata.
   */
  public boolean hasImage() {
    return fieldImage != null;
  }

  /**
   * Gets the resource path of the field image described by this field.
   *
   * @return The image resource path, or null if this field has no image.
   */
  public String getImageResource() {
    if (fieldImage == null) {
      return null;
    }
    if (fieldImage.path.startsWith("/")) {
      return fieldImage.path;
    }
    if (resourceFile != null && resourceFile.startsWith("/")) {
      return getResourceParent(resourceFile) + fieldImage.path;
    }
    return Fields.BASE_RESOURCE_DIR + getImageResourcePath();
  }

  /**
   * Gets the URL of the field image described by this field.
   *
   * <p>This returns null unless this field has an image and the separate field image resources
   * artifact is present on the runtime classpath.
   *
   * @return The image URL, or null if the image resource is not available.
   */
  public URL getImageUrl() {
    String resourcePath = getImageResource();
    if (resourcePath == null) {
      return null;
    }
    return getClass().getResource(resourcePath);
  }

  /**
   * Gets an input stream for the field image described by this field.
   *
   * <p>This returns null unless this field has an image and the separate field image resources
   * artifact is present on the runtime classpath.
   *
   * @return The image input stream, or null if the image resource is not available.
   */
  public InputStream getImageAsStream() {
    String resourcePath = getImageResource();
    if (resourcePath == null) {
      return null;
    }
    return getClass().getResourceAsStream(resourcePath);
  }

  private String getImageResourcePath() {
    String prefix = program + "/";
    return fieldImage.path.startsWith(prefix) ? fieldImage.path : prefix + fieldImage.path;
  }

  private static String getResourceParent(String resourcePath) {
    int lastSeparator = resourcePath.lastIndexOf('/');
    return lastSeparator < 0 ? "" : resourcePath.substring(0, lastSeparator + 1);
  }

  /**
   * Returns the original JSON contents for this field.
   *
   * @return The original JSON contents for this field, or null if none were retained.
   */
  public String getJson() {
    if (m_json != null) {
      return m_json;
    }
    if (m_jsonSupplier == null) {
      return null;
    }
    m_json = m_jsonSupplier.get();
    return m_json;
  }

  /**
   * Returns true if this field has tag metadata.
   *
   * @return True if this field has tag metadata.
   */
  public boolean hasTags() {
    return tags != null;
  }

  /**
   * Returns a List of the {@link FieldTag FieldTags} used by this field.
   *
   * @return The {@link FieldTag FieldTags} used by this field.
   */
  public List<FieldTag> getTags() {
    return tags == null ? List.of() : List.copyOf(tags);
  }

  /**
   * Returns the field length in meters.
   *
   * @return The field length in meters.
   */
  public double getFieldLength() {
    return length;
  }

  /**
   * Returns the field width in meters.
   *
   * @return The field width in meters.
   */
  public double getFieldWidth() {
    return width;
  }

  /**
   * Sets the origin based on a predefined enumeration of coordinate frame origins. The origins are
   * calculated from the field dimensions.
   *
   * <p>This transforms the Pose3d objects returned by {@link #getTagPose(int)} to return the
   * correct pose relative to a predefined coordinate frame.
   *
   * @param origin The predefined origin.
   */
  public void setOrigin(OriginPosition origin) {
    var pose =
        switch (origin) {
          case BLUE_ALLIANCE_WALL_RIGHT_SIDE -> Pose3d.ZERO;
          case RED_ALLIANCE_WALL_RIGHT_SIDE ->
              new Pose3d(new Translation3d(length, width, 0), new Rotation3d(0, 0, Math.PI));
        };
    setOrigin(pose);
  }

  /**
   * Sets the origin for tag pose transformation.
   *
   * <p>This transforms the Pose3d objects returned by {@link #getTagPose(int)} to return the
   * correct pose relative to the provided origin.
   *
   * @param origin The new origin for tag transformations.
   */
  public void setOrigin(Pose3d origin) {
    m_origin = origin;
  }

  /**
   * Returns the origin used for tag pose transformation.
   *
   * @return The origin.
   */
  public Pose3d getOrigin() {
    return m_origin;
  }

  /**
   * Gets a tag pose by its ID.
   *
   * @param ID The ID of the tag.
   * @return The pose corresponding to the ID passed in or an empty optional if a tag with that ID
   *     was not found.
   */
  @SuppressWarnings("ParameterName")
  public Optional<Pose3d> getTagPose(int ID) {
    if (tags == null) {
      return Optional.empty();
    }
    for (FieldTag tag : tags) {
      if (tag.getID() == ID) {
        return Optional.of(tag.getPose().relativeTo(m_origin));
      }
    }
    return Optional.empty();
  }

  /**
   * Serializes a field to a JSON file.
   *
   * @param path The path to write to.
   * @throws IOException If writing to the file fails.
   */
  public void serialize(String path) throws IOException {
    serialize(Path.of(path));
  }

  /**
   * Serializes a field to a JSON file.
   *
   * @param path The path to write to.
   * @throws IOException If writing to the file fails.
   */
  public void serialize(Path path) throws IOException {
    Jsonb.instance().type(Field.class).toJson(this, Files.newBufferedWriter(path));
  }

  @Json.Property("field-dimensions")
  FieldDimensions getFieldDimensions() {
    return new FieldDimensions(length, width);
  }

  /**
   * Loads a predefined field.
   *
   * @param field The predefined field.
   * @return The field.
   */
  public static Field loadField(Fields field) {
    return field.loadField();
  }

  /**
   * Loads a field from a file on disk.
   *
   * @param file The JSON file to load.
   * @return The field.
   * @throws IOException Throws if the file could not be loaded.
   */
  public static Field loadFromFile(Path file) throws IOException {
    return fromJson(Files.readString(file));
  }

  /**
   * Loads a field from a resource file located inside the program's jar file.
   *
   * @param resourcePath The path to the resource file.
   * @return The field.
   * @throws IOException Throws if the resource could not be loaded.
   */
  public static Field loadFromResource(String resourcePath) throws IOException {
    try (InputStream stream = Field.class.getResourceAsStream(resourcePath)) {
      if (stream == null) {
        throw new IOException("Could not locate resource: " + resourcePath);
      }
      Field field = fromJson(new String(stream.readAllBytes(), StandardCharsets.UTF_8));
      field.resourceFile = normalizeResourcePath(resourcePath);
      return field;
    }
  }

  private static String normalizeResourcePath(String resourcePath) {
    if (resourcePath.startsWith("/")) {
      return resourcePath;
    }
    return "/" + Field.class.getPackageName().replace('.', '/') + "/" + resourcePath;
  }

  Field copy() {
    return new Field(this);
  }

  void setJson(String json) {
    m_json = json;
  }

  private static Field fromJson(String json) {
    Field field = Jsonb.instance().type(Field.class).fromJson(json);
    field.setJson(json);
    return field;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof Field other
        && Objects.equals(getName(), other.getName())
        && Objects.equals(season, other.season)
        && Objects.equals(game, other.game)
        && Objects.equals(fieldImage, other.fieldImage)
        && length == other.length
        && width == other.width
        && Objects.equals(program, other.program)
        && Objects.equals(tags, other.tags)
        && Objects.equals(m_origin, other.m_origin);
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        getName(), season, game, fieldImage, length, width, program, tags, m_origin);
  }
}
