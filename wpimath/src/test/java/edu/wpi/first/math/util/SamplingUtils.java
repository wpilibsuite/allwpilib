package edu.wpi.first.math.util;

import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.numbers.N3;
import java.util.Random;

public class SamplingUtils {
  public static Rotation2d sampleRotation2d(Random rand, double stdev) {
    return Rotation2d.fromRadians(rand.nextGaussian() * stdev);
  }

  public static Twist2d sampleTwist2d(Random rand, Vector<N3> stdev) {
    return new Twist2d(
        rand.nextGaussian() * stdev.get(0, 0),
        rand.nextGaussian() * stdev.get(1, 0),
        sampleRotation2d(rand, stdev.get(2, 0)).getRadians());
  }
}
