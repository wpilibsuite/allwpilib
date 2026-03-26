// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.annotations.Scope;
import org.openjdk.jmh.annotations.State;
import org.openjdk.jmh.profile.GCProfiler;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.proto.ProtobufPose3d;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;

import io.avaje.jsonb.JsonType;
import io.avaje.jsonb.Jsonb;
import us.hebi.quickbuf.JsonSource;

public class Main {
  public Main() {
  }
  @State(Scope.Thread)
  public static class JSONState {
    public ObjectReader reader = new ObjectMapper().readerFor(Pose3d.class);
    public JsonType<Pose3d> builder = Jsonb.builder().build().type(Pose3d.class);
  }


  /**
   * Main initialization function. Do not perform any initialization here.
   *
   * <p>
   * If you change your main robot class, change the parameter type.
   */
  public static void main(String... args) {
    if (!HAL.initialize(500, 0)) {
      throw new IllegalStateException("Failed to initialize. Terminating");
    }
    try {
      Options opt = new OptionsBuilder()
          .include(Main.class.getSimpleName())
          .addProfiler(GCProfiler.class)
          .forks(2)
          .warmupIterations(4)
          .measurementIterations(4)
          .build();

      new Runner(opt).run();
    } catch (Exception e) {
    }
  }

  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose3d quickbufPose3d() throws IOException {
    return Pose3d.proto.unpack(
        ProtobufPose3d.parseFrom(
            JsonSource.newInstance(
                "{\"translation\":{\"x\":0,\"y\":0,\"z\":0},\"rotation\":{\"q\":{\"w\":1,\"x\":0,\"y\":0,\"z\":0}}}")));
  }

  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose3d jacksonPose3d(JSONState state) throws IOException {
    return state.reader.readValue(
        "{\"translation\":{\"x\":0,\"y\":0,\"z\":0},\"rotation\":{\"quaternion\":{\"W\":1,\"X\":0,\"Y\":0,\"Z\":0}}}");
  }


  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose3d avajePose3d(JSONState state) throws IOException {
    return state.builder.fromJson(
        "{\"translation\":{\"x\":0,\"y\":0,\"z\":0},\"rotation\":{\"quaternion\":{\"W\":1,\"X\":0,\"Y\":0,\"Z\":0}}}");
  }
}
