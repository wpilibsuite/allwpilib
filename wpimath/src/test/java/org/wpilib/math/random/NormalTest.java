// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.random;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.UtilityClassTest;
import org.wpilib.math.linalg.VecBuilder;

class NormalTest extends UtilityClassTest<Normal> {
  NormalTest() {
    super(Normal.class);
  }

  @Test
  void testNormal() {
    var firstData = new ArrayList<Double>();
    var secondData = new ArrayList<Double>();
    for (int i = 0; i < 1000; i++) {
      var noiseVec = Normal.normal(VecBuilder.fill(1.0, 2.0));
      firstData.add(noiseVec.get(0, 0));
      secondData.add(noiseVec.get(1, 0));
    }
    assertEquals(1.0, calculateStandardDeviation(firstData), 0.2);
    assertEquals(2.0, calculateStandardDeviation(secondData), 0.2);
  }

  private double calculateStandardDeviation(List<Double> numArray) {
    double sum = 0.0;
    double standardDeviation = 0.0;
    int length = numArray.size();

    for (double num : numArray) {
      sum += num;
    }

    double mean = sum / length;

    for (double num : numArray) {
      standardDeviation += Math.pow(num - mean, 2);
    }

    return Math.sqrt(standardDeviation / length);
  }
}
