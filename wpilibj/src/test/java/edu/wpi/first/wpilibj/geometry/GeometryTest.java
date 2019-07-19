package edu.wpi.first.wpilibj.geometry;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertTrue;

public class GeometryTest {

  private static double kEpsilon = 1E-9;

  /**
   * Asserts "val1" is within "eps" of "val2".
   *
   * @param val1 First operand in comparison.
   * @param val2 Second operand in comparison.
   * @param eps Tolerance for whether values are near to each other.
   */
  private static void assertNear(double val1, double val2, double eps) {
    assertTrue(Math.abs(val1 - val2) <= eps, "Difference between " + val1 + " and " + val2
        + " is greater than " + eps);
  }

  @Test
  void testTransforms() {
    Pose2d initial = new Pose2d(0.0, 0.0, Rotation2d.fromDegrees(45.0));
    Pose2d last = new Pose2d(5.0, 5.0, Rotation2d.fromDegrees(45.0));

    Pose2d finalRelativeToInitial = last.relativeTo(initial);

    assertNear(finalRelativeToInitial.getTranslation().getX(), 5.0 * Math.sqrt(2.0), kEpsilon);
    assertNear(finalRelativeToInitial.getTranslation().getY(), 0.0, kEpsilon);
    assertNear(finalRelativeToInitial.getRotation().getDegrees(), 0.0, kEpsilon);
  }

  @Test
  void testTranslations() {
    Translation2d one = new Translation2d(1.0, 3.0);
    Translation2d two = new Translation2d(2.0, 5.0);

    var sum = one.plus(two);

    assertNear(sum.getX(), 3.0, kEpsilon);
    assertNear(sum.getY(), 8.0, kEpsilon);

    var difference = one.minus(two);

    assertNear(difference.getX(), -1.0, kEpsilon);
    assertNear(difference.getY(), -2.0, kEpsilon);

    Translation2d another = new Translation2d(3.0, 0.0);
    var rotated = another.rotateBy(Rotation2d.fromDegrees(90.0));

    assertNear(rotated.getX(), 0.0, kEpsilon);
    assertNear(rotated.getY(), 3.0, kEpsilon);
  }

  @Test
  void testRotations() {
    Rotation2d zero = new Rotation2d();
    var rot1 = Rotation2d.fromDegrees(90.0);

    var sum = zero.plus(rot1);

    assertNear(sum.getDegrees(), 90.0, kEpsilon);

    sum = sum.plus(Rotation2d.fromDegrees(30.0));

    assertNear(sum.getDegrees(), 120.0, kEpsilon);
  }
}
