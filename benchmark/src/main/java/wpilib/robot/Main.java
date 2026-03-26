// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import com.alibaba.fastjson2.JSONReader;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.squareup.moshi.Moshi;
import io.avaje.jsonb.Jsonb;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.TimeUnit;
import okio.Okio;
import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.profile.GCProfiler;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.vision.apriltag.AprilTag;
import org.wpilib.vision.apriltag.AprilTagFieldLayout;
import org.wpilib.vision.apriltag.AprilTagFields;

public class Main {
  public Main() {}
  /**
   * Main initialization function. Do not perform any initialization here.
   *
   * <p>If you change your main robot class, change the parameter type.
   */
  public static void main(String... args) {
    try {
      Options opt =
          new OptionsBuilder()
              .include(Main.class.getSimpleName())
              .addProfiler(GCProfiler.class)
              .forks(2)
              .warmupIterations(0)
              .measurementIterations(4)
              .build();

      new Runner(opt).run();
    } catch (Exception e) {
    }
  }

  @Benchmark
  @BenchmarkMode(Mode.SingleShotTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public InputStream getResourceAsStream() throws IOException {
    InputStream stream =
        AprilTagFieldLayout.class.getResourceAsStream(
            AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    if (stream == null) {
      // Class.getResourceAsStream() returns null if the resource does not exist.
      throw new IOException(
          "Could not locate resource: " + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
    return stream;
  }

  @Benchmark
  @BenchmarkMode(Mode.SingleShotTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public AprilTag loadClasses() throws IOException {
    return new AprilTag(1, new Pose3d(1, 2, 3, Rotation3d.kZero));
  }

  @Benchmark
  @BenchmarkMode(Mode.SingleShotTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public AprilTagFieldLayout jacksonAtfl() throws IOException {
    InputStream stream =
        AprilTagFieldLayout.class.getResourceAsStream(
            AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    if (stream == null) {
      // Class.getResourceAsStream() returns null if the resource does not exist.
      throw new IOException(
          "Could not locate resource: " + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
    InputStreamReader reader = new InputStreamReader(stream, StandardCharsets.UTF_8);
    try {
      return new ObjectMapper().readerFor(AprilTagFieldLayout.class).readValue(reader);
    } catch (IOException e) {
      throw new IOException(
          "Failed to load AprilTagFieldLayout: "
              + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
  }

  @Benchmark
  @BenchmarkMode(Mode.SingleShotTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public AprilTagFieldLayout avajeAtfl() throws IOException {
    InputStream stream =
        AprilTagFieldLayout.class.getResourceAsStream(
            AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    if (stream == null) {
      // Class.getResourceAsStream() returns null if the resource does not exist.
      throw new IOException(
          "Could not locate resource: " + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
    InputStreamReader reader = new InputStreamReader(stream, StandardCharsets.UTF_8);
    try {
      return Jsonb.builder().build().type(AprilTagFieldLayout.class).fromJson(reader);
    } catch (Exception e) {
      e.printStackTrace();
      throw new IOException(
          "Failed to load AprilTagFieldLayout: "
              + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
  }

  @Benchmark
  @BenchmarkMode(Mode.SingleShotTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public AprilTagFieldLayout moshiAtfl() throws IOException {
    InputStream stream =
        AprilTagFieldLayout.class.getResourceAsStream(
            AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    if (stream == null) {
      // Class.getResourceAsStream() returns null if the resource does not exist.
      throw new IOException(
          "Could not locate resource: " + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
    try {
      return new Moshi.Builder()
          .build()
          .adapter(AprilTagFieldLayout.class)
          .fromJson(Okio.buffer(Okio.source(stream)));
    } catch (Exception e) {
      e.printStackTrace();
      throw new IOException(
          "Failed to load AprilTagFieldLayout: "
              + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
  }

  @Benchmark
  @BenchmarkMode(Mode.SingleShotTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public AprilTagFieldLayout fastjson2Atfl() throws IOException {
    InputStream stream =
        AprilTagFieldLayout.class.getResourceAsStream(
            AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    if (stream == null) {
      // Class.getResourceAsStream() returns null if the resource does not exist.
      throw new IOException(
          "Could not locate resource: " + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
    try {
      return JSONReader.of(stream, StandardCharsets.UTF_8).read(AprilTagFieldLayout.class);
    } catch (Exception e) {
      e.printStackTrace();
      throw new IOException(
          "Failed to load AprilTagFieldLayout: "
              + AprilTagFields.k2025ReefscapeWelded.m_resourceFile);
    }
  }
}
