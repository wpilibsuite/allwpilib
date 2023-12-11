// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.networktables.NetworkTableInstance;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import org.junit.jupiter.api.Test;

class SendableBuilderImplTest {
  @Test
  void cache() {
    try (var impl = new SendableBuilderImpl();
        var inst = NetworkTableInstance.create()) {
      impl.setTable(inst.getTable("table"));

      var hashcode = new AtomicInteger(0);
      var updateCalls = new AtomicLong(0);
      impl.caching(
          "The Property",
          hashcode::get,
          () -> new long[] {updateCalls.incrementAndGet()},
          null,
          impl::addIntegerArrayProperty);

      impl.update();
      assertEquals(1, updateCalls.get(), "Initial update queries the value");

      impl.update();
      assertEquals(
          1, updateCalls.get(), "Update without cache invalidation does not query the value");

      hashcode.incrementAndGet();
      impl.update();
      assertEquals(2, updateCalls.get(), "Update after cache invalidation queries the value");
    }
  }
}
