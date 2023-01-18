// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import java.util.Arrays;
import org.junit.jupiter.api.Test;

class Pose3dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testTransformByRotations() {
    var initialPose =
        new Pose3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0)));

    var transform1 =
        new Transform3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(90.0),
                Units.degreesToRadians(45.0),
                Units.degreesToRadians(0.0)));

    var transform2 =
        new Transform3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(-90.0),
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0)));

    var transform3 =
        new Transform3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(-45.0),
                Units.degreesToRadians(0.0)));

    // This sequence of rotations should diverge from the origin and eventually
    // return to it. When
    // each rotation occurs, it should be performed intrinsicly, i.e. 'from the
    // viewpoint' of and
    // with
    // the axes of the pose that is being transformed, just like how the translation
    // is done 'from
    // the
    // viewpoint' of the pose that is being transformed.
    var finalPose =
        initialPose.transformBy(transform1).transformBy(transform2).transformBy(transform3);

    assertAll(
        () ->
            assertEquals(
                finalPose.getRotation().getX(), initialPose.getRotation().getX(), kEpsilon),
        () ->
            assertEquals(
                finalPose.getRotation().getY(), initialPose.getRotation().getY(), kEpsilon),
        () ->
            assertEquals(
                finalPose.getRotation().getZ(), initialPose.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testTransformBy() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial =
        new Pose3d(
            new Translation3d(1.0, 2.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var transformation =
        new Transform3d(
            new Translation3d(5.0, 0.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(5.0)));

    var transformed = initial.plus(transformation);

    assertAll(
        () -> assertEquals(1.0 + 5.0 / Math.sqrt(2.0), transformed.getX(), kEpsilon),
        () -> assertEquals(2.0 + 5.0 / Math.sqrt(2.0), transformed.getY(), kEpsilon),
        () ->
            assertEquals(Units.degreesToRadians(50.0), transformed.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testRelativeTo() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial = new Pose3d(0.0, 0.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var last = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));

    var finalRelativeToInitial = last.relativeTo(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), finalRelativeToInitial.getX(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getY(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testEquality() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    var two = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    var two = new Pose3d(0.0, 1.524, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    assertNotEquals(one, two);
  }

  @Test
  void testMinus() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial = new Pose3d(0.0, 0.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var last = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));

    final var transform = last.minus(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), transform.getX(), kEpsilon),
        () -> assertEquals(0.0, transform.getY(), kEpsilon),
        () -> assertEquals(0.0, transform.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testToPose2d() {
    var pose =
        new Pose3d(
            1.0,
            2.0,
            3.0,
            new Rotation3d(
                Units.degreesToRadians(20.0),
                Units.degreesToRadians(30.0),
                Units.degreesToRadians(40.0)));
    var expected = new Pose2d(1.0, 2.0, new Rotation2d(Units.degreesToRadians(40.0)));

    assertEquals(expected, pose.toPose2d());
  }

  @Test
  void testComplexTwists() {
    var initial_poses =
        Arrays.asList(
            new Pose3d(
                new Translation3d(0, 0, 0),
                new Rotation3d(new Quaternion(0.646945, 0.436419, 0.436269, 0.447963))),
            new Pose3d(
                new Translation3d(0.002418, 0.029928, -0.013237),
                new Rotation3d(new Quaternion(0.645881, 0.443403, 0.432498, 0.446293))),
            new Pose3d(
                new Translation3d(0.016711, 0.069211, -0.035857),
                new Rotation3d(new Quaternion(0.649753, 0.439876, 0.422109, 0.454043))),
            new Pose3d(
                new Translation3d(0.000012, 0.117392, -0.032208),
                new Rotation3d(new Quaternion(0.656768, 0.444939, 0.414394, 0.446052))),
            new Pose3d(
                new Translation3d(0.040425, 0.213553, -0.03497),
                new Rotation3d(new Quaternion(0.659185, 0.430746, 0.42971, 0.441907))),
            new Pose3d(
                new Translation3d(0.058289, 0.304794, -0.078752),
                new Rotation3d(new Quaternion(0.667931, 0.406728, 0.444564, 0.436809))),
            new Pose3d(
                new Translation3d(0.088465, 0.425428, -0.129523),
                new Rotation3d(new Quaternion(0.67926, 0.366995, 0.471908, 0.425703))),
            new Pose3d(
                new Translation3d(0.100632, 0.564067, -0.208609),
                new Rotation3d(new Quaternion(0.687381, 0.321302, 0.514702, 0.399192))),
            new Pose3d(
                new Translation3d(0.111362, 0.685452, -0.289918),
                new Rotation3d(new Quaternion(0.697177, 0.223015, 0.5728, 0.368929))),
            new Pose3d(
                new Translation3d(0.138538, 0.795872, -0.400473),
                new Rotation3d(new Quaternion(0.706117, 0.103366, 0.604871, 0.353333))),
            new Pose3d(
                new Translation3d(0.168653, 0.916497, -0.604651),
                new Rotation3d(new Quaternion(0.706459, -0.042527, 0.62839, 0.322852))),
            new Pose3d(
                new Translation3d(0.218663, 0.99004, -0.77593),
                new Rotation3d(new Quaternion(0.727952, -0.252, 0.571972, 0.281834))),
            new Pose3d(
                new Translation3d(0.33186, 1.035646, -0.979254),
                new Rotation3d(new Quaternion(0.740613, -0.433328, 0.444034, 0.257976))),
            new Pose3d(
                new Translation3d(0.495884, 1.131095, -1.100259),
                new Rotation3d(new Quaternion(0.771682, -0.560141, 0.201834, 0.223634))),
            new Pose3d(
                new Translation3d(0.638315, 1.347734, -1.15594),
                new Rotation3d(new Quaternion(0.802104, -0.54912, -0.092608, 0.215684))),
            new Pose3d(
                new Translation3d(0.724588, 1.579227, -1.075315),
                new Rotation3d(new Quaternion(0.861402, -0.343246, -0.338095, 0.160814))),
            new Pose3d(
                new Translation3d(0.874686, 1.682747, -0.939834),
                new Rotation3d(new Quaternion(0.908368, -0.005462, -0.404882, 0.104445))),
            new Pose3d(
                new Translation3d(1.169334, 1.734017, -0.933983),
                new Rotation3d(new Quaternion(0.948036, 0.287819, -0.135097, -0.011707))),
            new Pose3d(
                new Translation3d(1.427445, 1.805259, -1.163432),
                new Rotation3d(new Quaternion(0.943182, 0.130344, 0.259623, -0.161286))),
            new Pose3d(
                new Translation3d(1.585354, 1.721356, -1.335473),
                new Rotation3d(new Quaternion(0.879762, -0.304312, 0.085612, -0.355082))),
            new Pose3d(
                new Translation3d(1.592492, 1.555068, -1.33676),
                new Rotation3d(new Quaternion(0.819799, 0.018802, -0.316522, -0.476853))),
            new Pose3d(
                new Translation3d(1.742662, 1.378978, -1.672011),
                new Rotation3d(new Quaternion(0.809444, 0.18385, 0.142247, -0.539226))),
            new Pose3d(
                new Translation3d(1.777277, 1.264806, -1.77956),
                new Rotation3d(new Quaternion(0.809626, -0.221591, -0.034411, -0.542419))),
            new Pose3d(
                new Translation3d(1.789636, 1.02339, -1.952459),
                new Rotation3d(new Quaternion(0.779724, 0.1509, -0.173351, -0.582417))),
            new Pose3d(
                new Translation3d(1.734082, 0.776963, -2.196558),
                new Rotation3d(new Quaternion(0.691954, -0.046864, 0.151437, -0.704322))),
            new Pose3d(
                new Translation3d(1.57598, 0.582877, -2.29137),
                new Rotation3d(new Quaternion(-0.527384, -0.045167, 0.202696, 0.823857))),
            new Pose3d(
                new Translation3d(1.357266, 0.412143, -2.487767),
                new Rotation3d(new Quaternion(-0.381064, 0.019135, -0.156678, 0.910976))),
            new Pose3d(
                new Translation3d(1.104084, 0.296487, -2.596022),
                new Rotation3d(new Quaternion(-0.273116, -0.146909, 0.124671, 0.942487))),
            new Pose3d(
                new Translation3d(0.82837, 0.206019, -2.563144),
                new Rotation3d(new Quaternion(-0.201729, 0.115288, -0.008962, 0.972591))),
            new Pose3d(
                new Translation3d(0.564039, 0.142354, -2.594488),
                new Rotation3d(new Quaternion(-0.140176, -0.13947, -0.112267, 0.973804))),
            new Pose3d(
                new Translation3d(0.230599, 0.058644, -2.564471),
                new Rotation3d(new Quaternion(-0.095968, -0.065097, 0.125858, 0.985247))),
            new Pose3d(
                new Translation3d(-0.041733, 0.056102, -2.379469),
                new Rotation3d(new Quaternion(-0.067172, 0.078849, -0.051168, 0.993304))),
            new Pose3d(
                new Translation3d(-0.274601, 0.060245, -2.292462),
                new Rotation3d(new Quaternion(-0.035553, -0.125654, -0.076543, 0.988478))),
            new Pose3d(
                new Translation3d(-0.554576, 0.051821, -2.130242),
                new Rotation3d(new Quaternion(0.006153, -0.064071, 0.091603, 0.993713))),
            new Pose3d(
                new Translation3d(-0.766964, 0.100462, -1.863828),
                new Rotation3d(new Quaternion(0.055759, 0.083158, -0.012584, 0.994896))),
            new Pose3d(
                new Translation3d(-0.940952, 0.136307, -1.682399),
                new Rotation3d(new Quaternion(0.08858, -0.041836, -0.10884, 0.98922))),
            new Pose3d(
                new Translation3d(-1.105031, 0.115062, -1.489813),
                new Rotation3d(new Quaternion(0.127886, -0.108958, -0.00729, 0.985759))),
            new Pose3d(
                new Translation3d(-1.278122, 0.215271, -1.170281),
                new Rotation3d(new Quaternion(0.172712, -0.000584, 0.030043, 0.984514))),
            new Pose3d(
                new Translation3d(-1.419099, 0.305232, -0.869529),
                new Rotation3d(new Quaternion(0.172591, 0.026793, -0.061806, 0.982687))),
            new Pose3d(
                new Translation3d(-1.522071, 0.38909, -0.627772),
                new Rotation3d(new Quaternion(0.159117, -0.062068, -0.089471, 0.981236))),
            new Pose3d(
                new Translation3d(-1.650471, 0.395294, -0.374411),
                new Rotation3d(new Quaternion(0.113005, -0.079626, 0.001003, 0.990398))),
            new Pose3d(
                new Translation3d(-1.791018, 0.444432, -0.085048),
                new Rotation3d(new Quaternion(0.082426, 0.012633, 0.011058, 0.996456))),
            new Pose3d(
                new Translation3d(-1.858863, 0.446594, 0.166246),
                new Rotation3d(new Quaternion(0.045403, 0.000016, -0.069826, 0.996525))),
            new Pose3d(
                new Translation3d(-1.95012, 0.389727, 0.443221),
                new Rotation3d(new Quaternion(-0.004535, -0.072473, -0.048339, 0.996188))),
            new Pose3d(
                new Translation3d(-2.052926, 0.380351, 0.729784),
                new Rotation3d(new Quaternion(-0.036637, -0.0415, 0.031407, 0.997972))),
            new Pose3d(
                new Translation3d(-2.142614, 0.29528, 1.032337),
                new Rotation3d(new Quaternion(-0.073118, 0.010599, -0.022478, 0.997014))),
            new Pose3d(
                new Translation3d(-2.179071, 0.19625, 1.271926),
                new Rotation3d(new Quaternion(-0.116093, -0.047098, -0.028082, 0.991724))),
            new Pose3d(
                new Translation3d(-2.207945, 0.111731, 1.532317),
                new Rotation3d(new Quaternion(-0.165724, -0.051214, 0.025227, 0.984518))),
            new Pose3d(
                new Translation3d(-2.270528, 0.016934, 1.786634),
                new Rotation3d(new Quaternion(-0.212987, -0.015348, -0.00094, 0.976934))),
            new Pose3d(
                new Translation3d(-2.256726, -0.15104, 1.977827),
                new Rotation3d(new Quaternion(-0.30187, -0.039096, 0.007467, 0.952518))),
            new Pose3d(
                new Translation3d(-2.25853, -0.271041, 2.2149),
                new Rotation3d(new Quaternion(-0.409363, -0.03323, 0.028445, 0.911323))),
            new Pose3d(
                new Translation3d(-2.242057, -0.516777, 2.438478),
                new Rotation3d(new Quaternion(0.580632, 0.033447, -0.016381, -0.813314))),
            new Pose3d(
                new Translation3d(-2.089382, -0.791271, 2.633688),
                new Rotation3d(new Quaternion(0.720799, -0.003875, -0.021815, -0.69279))),
            new Pose3d(
                new Translation3d(-1.83882, -1.035836, 2.773106),
                new Rotation3d(new Quaternion(0.791788, 0.030271, -0.039491, -0.608765))),
            new Pose3d(
                new Translation3d(-1.560147, -1.326146, 2.847799),
                new Rotation3d(new Quaternion(0.839334, -0.011324, -0.017645, -0.543212))),
            new Pose3d(
                new Translation3d(-1.317383, -1.577454, 2.908512),
                new Rotation3d(new Quaternion(0.841533, -0.024897, -0.062093, -0.536048))),
            new Pose3d(
                new Translation3d(-1.051403, -1.830944, 2.871154),
                new Rotation3d(new Quaternion(0.89043, 0.029674, -0.024109, -0.453511))),
            new Pose3d(
                new Translation3d(-0.792927, -1.998094, 2.782673),
                new Rotation3d(new Quaternion(0.957441, -0.034293, -0.004942, -0.286541))),
            new Pose3d(
                new Translation3d(-0.481851, -2.105274, 2.70522),
                new Rotation3d(new Quaternion(0.993823, -0.061462, -0.041658, -0.082485))),
            new Pose3d(
                new Translation3d(-0.177319, -2.116726, 2.542369),
                new Rotation3d(new Quaternion(0.995771, -0.045684, -0.036502, 0.070854))),
            new Pose3d(
                new Translation3d(0.095757, -2.075046, 2.29593),
                new Rotation3d(new Quaternion(0.985018, -0.050386, -0.005109, 0.164848))),
            new Pose3d(
                new Translation3d(0.314386, -2.033622, 2.048947),
                new Rotation3d(new Quaternion(0.972285, -0.049728, 0.014413, 0.227992))),
            new Pose3d(
                new Translation3d(0.448766, -1.93292, 1.75516),
                new Rotation3d(new Quaternion(0.958278, -0.064648, 0.043723, 0.274978))),
            new Pose3d(
                new Translation3d(0.589136, -1.798648, 1.49871),
                new Rotation3d(new Quaternion(0.942529, -0.091614, 0.059805, 0.315703))),
            new Pose3d(
                new Translation3d(0.675486, -1.677442, 1.219257),
                new Rotation3d(new Quaternion(0.931028, -0.089901, 0.069096, 0.346886))),
            new Pose3d(
                new Translation3d(0.746987, -1.513519, 0.947569),
                new Rotation3d(new Quaternion(0.915732, -0.090811, 0.084217, 0.382225))),
            new Pose3d(
                new Translation3d(0.745735, -1.317015, 0.692479),
                new Rotation3d(new Quaternion(0.906379, -0.092692, 0.094457, 0.401201))),
            new Pose3d(
                new Translation3d(0.749803, -1.167927, 0.45359),
                new Rotation3d(new Quaternion(0.882684, -0.100739, 0.125774, 0.441476))),
            new Pose3d(
                new Translation3d(0.698303, -0.959096, 0.271076),
                new Rotation3d(new Quaternion(0.86403, -0.076866, 0.147234, 0.475254))),
            new Pose3d(
                new Translation3d(0.634892, -0.765209, 0.117543),
                new Rotation3d(new Quaternion(0.84987, -0.070829, 0.162097, 0.496415))),
            new Pose3d(
                new Translation3d(0.584827, -0.590303, -0.02557),
                new Rotation3d(new Quaternion(0.832743, -0.041991, 0.202188, 0.513708))),
            new Pose3d(
                new Translation3d(0.505038, -0.451479, -0.112835),
                new Rotation3d(new Quaternion(0.816515, -0.002673, 0.226182, 0.531166))),
            new Pose3d(
                new Translation3d(0.428178, -0.329692, -0.189707),
                new Rotation3d(new Quaternion(0.807886, 0.029298, 0.257788, 0.529157))));

    var final_poses =
        Arrays.asList(
            new Pose3d(
                new Translation3d(-0.009568, 0.079731, -0.066398),
                new Rotation3d(new Quaternion(0.663769, 0.443899, 0.484523, 0.357213))),
            new Pose3d(
                new Translation3d(0.025353, 0.003182, -0.103806),
                new Rotation3d(new Quaternion(0.678309, 0.430928, 0.38995, 0.449596))),
            new Pose3d(
                new Translation3d(-0.176395, 0.024834, 0.042698),
                new Rotation3d(new Quaternion(0.652713, 0.503946, 0.396673, 0.403305))),
            new Pose3d(
                new Translation3d(0.124272, 0.240377, 0.023838),
                new Rotation3d(new Quaternion(0.675582, 0.400544, 0.448656, 0.426452))),
            new Pose3d(
                new Translation3d(-0.005538, 0.060648, -0.133301),
                new Rotation3d(new Quaternion(0.669942, 0.3901, 0.423568, 0.468604))),
            new Pose3d(
                new Translation3d(0.098728, 0.33687, -0.157766),
                new Rotation3d(new Quaternion(0.661691, 0.419349, 0.404801, 0.471644))),
            new Pose3d(
                new Translation3d(0.041999, 0.48557, -0.235005),
                new Rotation3d(new Quaternion(0.659019, 0.423079, 0.500136, 0.369543))),
            new Pose3d(
                new Translation3d(0.113551, 0.500931, -0.110124),
                new Rotation3d(new Quaternion(0.648912, 0.405925, 0.509636, 0.392948))),
            new Pose3d(
                new Translation3d(0.247067, 0.578213, -0.129356),
                new Rotation3d(new Quaternion(0.710994, 0.420635, 0.427709, 0.366904))),
            new Pose3d(
                new Translation3d(0.349056, 0.846888, -0.353905),
                new Rotation3d(new Quaternion(0.57227, 0.455692, 0.567576, 0.37777))),
            new Pose3d(
                new Translation3d(0.311825, 1.037472, -0.199237),
                new Rotation3d(new Quaternion(0.718287, 0.302073, 0.571185, 0.257997))),
            new Pose3d(
                new Translation3d(0.341663, 1.174906, -0.653725),
                new Rotation3d(new Quaternion(0.683058, 0.317919, 0.60924, 0.247359))),
            new Pose3d(
                new Translation3d(0.362015, 1.287232, -0.663067),
                new Rotation3d(new Quaternion(0.785656, 0.303237, 0.496079, 0.211419))),
            new Pose3d(
                new Translation3d(0.398888, 1.692897, -0.738958),
                new Rotation3d(new Quaternion(0.746235, 0.293148, 0.518231, 0.297715))),
            new Pose3d(
                new Translation3d(0.532308, 1.883249, -0.799984),
                new Rotation3d(new Quaternion(0.784194, 0.220607, 0.551787, 0.178615))),
            new Pose3d(
                new Translation3d(0.634233, 1.874007, -1.111138),
                new Rotation3d(new Quaternion(0.784071, 0.211857, 0.554233, 0.182138))),
            new Pose3d(
                new Translation3d(0.789832, 2.065675, -1.35612),
                new Rotation3d(new Quaternion(0.768397, 0.242817, 0.590484, 0.043977))),
            new Pose3d(
                new Translation3d(0.865986, 2.177519, -1.566077),
                new Rotation3d(new Quaternion(0.773292, 0.141952, 0.615515, 0.054871))),
            new Pose3d(
                new Translation3d(0.881272, 2.034327, -1.971478),
                new Rotation3d(new Quaternion(0.800663, 0.029008, 0.577297, -0.157564))),
            new Pose3d(
                new Translation3d(0.781989, 1.993487, -1.74355),
                new Rotation3d(new Quaternion(0.750586, -0.105331, 0.610296, -0.230359))),
            new Pose3d(
                new Translation3d(0.83855, 1.942638, -2.200285),
                new Rotation3d(new Quaternion(0.662335, -0.21617, 0.619414, -0.361813))),
            new Pose3d(
                new Translation3d(0.840569, 1.671618, -2.37315),
                new Rotation3d(new Quaternion(0.73507, -0.24571, 0.519443, -0.35983))),
            new Pose3d(
                new Translation3d(0.90221, 1.464558, -2.550468),
                new Rotation3d(new Quaternion(0.671209, -0.272141, 0.570495, -0.387238))),
            new Pose3d(
                new Translation3d(0.74479, 1.019096, -2.478096),
                new Rotation3d(new Quaternion(0.652191, -0.242211, 0.478421, -0.535813))),
            new Pose3d(
                new Translation3d(0.822021, 0.965392, -2.604612),
                new Rotation3d(new Quaternion(0.53507, -0.364403, 0.58614, -0.487186))),
            new Pose3d(
                new Translation3d(0.678698, 0.799951, -2.520934),
                new Rotation3d(new Quaternion(-0.451969, 0.565353, -0.399901, 0.562298))),
            new Pose3d(
                new Translation3d(0.809595, 0.642015, -2.623569),
                new Rotation3d(new Quaternion(-0.267894, 0.440896, -0.403869, 0.755469))),
            new Pose3d(
                new Translation3d(0.768399, 0.400464, -2.412477),
                new Rotation3d(new Quaternion(-0.195642, 0.502644, -0.327508, 0.775765))),
            new Pose3d(
                new Translation3d(0.709982, 0.248029, -2.226506),
                new Rotation3d(new Quaternion(-0.134513, 0.605275, -0.165908, 0.766826))),
            new Pose3d(
                new Translation3d(0.309018, -0.032719, -2.031912),
                new Rotation3d(new Quaternion(-0.116616, 0.5537, -0.189951, 0.802332))),
            new Pose3d(
                new Translation3d(0.599569, 0.082213, -1.790561),
                new Rotation3d(new Quaternion(-0.166511, 0.60108, -0.223038, 0.749154))),
            new Pose3d(
                new Translation3d(0.378165, 0.06998, -1.642373),
                new Rotation3d(new Quaternion(-0.087433, 0.599921, -0.244952, 0.756603))),
            new Pose3d(
                new Translation3d(0.204086, -0.055204, -1.346623),
                new Rotation3d(new Quaternion(-0.039128, 0.577072, -0.220418, 0.785413))),
            new Pose3d(
                new Translation3d(0.292934, -0.034148, -1.138357),
                new Rotation3d(new Quaternion(0.026814, 0.558929, -0.140793, 0.816736))),
            new Pose3d(
                new Translation3d(0.032853, -0.023263, -0.990296),
                new Rotation3d(new Quaternion(0.040426, 0.531068, -0.113663, 0.838697))),
            new Pose3d(
                new Translation3d(0.153496, -0.194568, -0.752453),
                new Rotation3d(new Quaternion(0.021075, 0.570677, -0.04245, 0.819806))),
            new Pose3d(
                new Translation3d(0.265501, 0.057341, -0.399475),
                new Rotation3d(new Quaternion(0.170516, 0.640576, -0.00626, 0.748698))),
            new Pose3d(
                new Translation3d(0.01831, 0.108485, -0.161855),
                new Rotation3d(new Quaternion(0.066929, 0.639486, -0.042558, 0.7647))),
            new Pose3d(
                new Translation3d(-0.030501, 0.24263, -0.056107),
                new Rotation3d(new Quaternion(0.138699, 0.645774, -0.045584, 0.749441))),
            new Pose3d(
                new Translation3d(-0.079438, 0.086165, 0.265465),
                new Rotation3d(new Quaternion(0.086668, 0.630113, -0.03672, 0.770778))),
            new Pose3d(
                new Translation3d(-0.267971, 0.220608, 0.443483),
                new Rotation3d(new Quaternion(0.170981, 0.653288, 0.031202, 0.73689))),
            new Pose3d(
                new Translation3d(-0.25004, 0.220174, 0.68471),
                new Rotation3d(new Quaternion(0.06229, 0.543141, -0.021529, 0.837051))),
            new Pose3d(
                new Translation3d(-0.165242, 0.163221, 1.079066),
                new Rotation3d(new Quaternion(-0.003096, 0.641049, -0.03346, 0.766764))),
            new Pose3d(
                new Translation3d(-0.327207, 0.202123, 1.258883),
                new Rotation3d(new Quaternion(0.091151, 0.626164, -0.112141, 0.766182))),
            new Pose3d(
                new Translation3d(-0.406139, -0.124985, 1.368477),
                new Rotation3d(new Quaternion(-0.035745, 0.675283, -0.136395, 0.723956))),
            new Pose3d(
                new Translation3d(-0.419653, -0.036418, 1.76672),
                new Rotation3d(new Quaternion(-0.08478, 0.538632, -0.11791, 0.829931))),
            new Pose3d(
                new Translation3d(-0.362304, 0.015536, 1.785827),
                new Rotation3d(new Quaternion(-0.104263, 0.604904, -0.250258, 0.748726))),
            new Pose3d(
                new Translation3d(-0.636959, -0.07726, 2.032377),
                new Rotation3d(new Quaternion(-0.102217, 0.631325, -0.22896, 0.733865))),
            new Pose3d(
                new Translation3d(-0.596777, -0.308374, 2.162762),
                new Rotation3d(new Quaternion(-0.212103, 0.473687, -0.242667, 0.819601))),
            new Pose3d(
                new Translation3d(-0.649817, -0.160797, 2.501027),
                new Rotation3d(new Quaternion(-0.184036, 0.511596, -0.368617, 0.754004))),
            new Pose3d(
                new Translation3d(-0.862102, -0.557954, 2.614027),
                new Rotation3d(new Quaternion(-0.330992, 0.585028, -0.387949, 0.63062))),
            new Pose3d(
                new Translation3d(-0.78049, -0.83359, 2.690669),
                new Rotation3d(new Quaternion(0.433016, -0.442997, 0.400361, -0.675249))),
            new Pose3d(
                new Translation3d(-0.706368, -0.836325, 2.560178),
                new Rotation3d(new Quaternion(0.57712, -0.393222, 0.492418, -0.519454))),
            new Pose3d(
                new Translation3d(-0.771753, -1.314457, 2.373044),
                new Rotation3d(new Quaternion(0.659052, -0.244542, 0.578759, -0.413385))),
            new Pose3d(
                new Translation3d(-0.836966, -1.569362, 2.479688),
                new Rotation3d(new Quaternion(0.58711, -0.250342, 0.605313, -0.475633))),
            new Pose3d(
                new Translation3d(-0.809367, -1.746919, 2.223456),
                new Rotation3d(new Quaternion(0.668644, -0.182757, 0.589503, -0.414731))),
            new Pose3d(
                new Translation3d(-0.985769, -1.74825, 2.148007),
                new Rotation3d(new Quaternion(0.721774, -0.176832, 0.597198, -0.301873))),
            new Pose3d(
                new Translation3d(-0.70514, -1.9904, 2.015953),
                new Rotation3d(new Quaternion(0.740558, -0.024331, 0.631447, -0.228597))),
            new Pose3d(
                new Translation3d(-0.838828, -1.984889, 1.742687),
                new Rotation3d(new Quaternion(0.734835, 0.074809, 0.674088, 0.005175))),
            new Pose3d(
                new Translation3d(-0.917396, -2.102496, 1.41182),
                new Rotation3d(new Quaternion(0.711695, 0.091052, 0.695068, 0.045604))),
            new Pose3d(
                new Translation3d(-0.682623, -2.153878, 1.216824),
                new Rotation3d(new Quaternion(0.787427, 0.192016, 0.582513, 0.061378))),
            new Pose3d(
                new Translation3d(-0.887468, -1.962013, 1.137486),
                new Rotation3d(new Quaternion(0.73491, 0.206635, 0.625207, 0.16225))),
            new Pose3d(
                new Translation3d(-0.463728, -1.748491, 0.916597),
                new Rotation3d(new Quaternion(0.762482, 0.235072, 0.561065, 0.220384))),
            new Pose3d(
                new Translation3d(-0.586871, -1.6381, 0.710547),
                new Rotation3d(new Quaternion(0.727288, 0.364536, 0.509882, 0.279617))),
            new Pose3d(
                new Translation3d(-0.411447, -1.311366, 0.483359),
                new Rotation3d(new Quaternion(0.773742, 0.313908, 0.507795, 0.211966))),
            new Pose3d(
                new Translation3d(-0.558211, -1.108707, 0.554833),
                new Rotation3d(new Quaternion(0.737358, 0.306788, 0.523039, 0.297683))),
            new Pose3d(
                new Translation3d(-0.410712, -1.27636, 0.345257),
                new Rotation3d(new Quaternion(0.697861, 0.323024, 0.549386, 0.326834))),
            new Pose3d(
                new Translation3d(-0.321609, -0.728113, 0.380826),
                new Rotation3d(new Quaternion(0.725253, 0.389377, 0.422351, 0.379491))),
            new Pose3d(
                new Translation3d(-0.230448, -0.511957, 0.198406),
                new Rotation3d(new Quaternion(0.753984, 0.347016, 0.409105, 0.379106))),
            new Pose3d(
                new Translation3d(-0.088932, -0.343253, 0.095018),
                new Rotation3d(new Quaternion(0.638738, 0.413016, 0.536281, 0.365833))),
            new Pose3d(
                new Translation3d(-0.107908, -0.317552, 0.133946),
                new Rotation3d(new Quaternion(0.653444, 0.417069, 0.465505, 0.427046))),
            new Pose3d(
                new Translation3d(-0.123383, -0.156411, -0.047435),
                new Rotation3d(new Quaternion(0.652983, 0.40644, 0.431566, 0.47135))),
            new Pose3d(
                new Translation3d(-0.084654, -0.019305, -0.030022),
                new Rotation3d(new Quaternion(0.620243, 0.429104, 0.479384, 0.44873))));

    var twists = Arrays.asList(
        new Twist3d(0.088852, -0.039123, -0.037993, 0.117621796570749, -0.03822548097194, -0.168390076100418),
        new Twist3d(-0.005658, -0.097026, 0.001852, -0.0857512801822794, -0.0689828168433262, 0.00226314224777519),
        new Twist3d(-0.109304, 0.100996, -0.152764, 0.100512890317755, -0.138539382739991, 0.00785360206845773),
        new Twist3d(0.135572, 0.057998, 0.109515, -0.0282642893626488, 0.0516093956017835, -0.109883103571875),
        new Twist3d(-0.13732, -0.118174, -0.048623, -0.0912643038018222, 0.0415974585324333, -0.0039536593583496),
        new Twist3d(0.054798, -0.076337, 0.009282, -0.0437958417053017, -0.0302750986887748, 0.0956016781264769),
        new Twist3d(0.080964, -0.067598, -0.076117, 0.168296591760284, -0.0315574883042051, -0.0268796348440475),
        new Twist3d(-0.094181, 0.059486, 0.038378, 0.143648494515853, -0.0389945975980083, 0.112657513539225),
        new Twist3d(-0.139732, 0.052334, 0.185912, 0.166328990629455, -0.368567603611819, 0.280918951280534),
        new Twist3d(0.015606, 0.037735, 0.2201, 0.480921400995573, -0.13800308692934, 0.576947874376838),
        new Twist3d(-0.245257, 0.284981, 0.255341, 0.543973626507271, -0.319440509910078, 0.336055944192541),
        new Twist3d(-0.019233, 0.169367, 0.205614, 0.918570090854013, -0.209909810448764, 0.683823130418327),
        new Twist3d(-0.18682, 0.270202, 0.300141, 1.32165838582248, -0.333758846954523, 0.669538362416299),
        new Twist3d(-0.156305, 0.575602, 0.508356, 1.63434873603277, 0.0396272116852317, 0.962621620709529),
        new Twist3d(-0.178935, 0.483058, 0.57865, 1.80085807842224, 0.895561684923975, 0.622073808039836),
        new Twist3d(0.00849, 0.338503, -0.003037, 1.49293353847984, 1.60097293875776, 0.370339007376286),
        new Twist3d(0.064058, 0.239467, -0.612242, 0.748159763300268, 2.02109516509819, -0.332949070647838),
        new Twist3d(0.131115, 0.070933, -0.908667, -0.196295763086983, 1.57746347613789, -0.302456697737751),
        new Twist3d(0.268334, 0.01194, -0.988016, -0.264227304333054, 0.655976159815803, -0.178309718040383),
        new Twist3d(-0.451981, 0.160761, -0.87482, -0.130091277564922, 1.07357329172067, 0.511045410818547),
        new Twist3d(-0.605971, -0.126645, -1.30703, -1.4403265146336, 1.45953051582444, 0.182572540217991),
        new Twist3d(-0.141374, -0.258235, -1.220681, -1.19969880524716, 0.249875448737415, -0.054010996689163),
        new Twist3d(-0.382169, 0.07084, -1.210512, -0.847752925074304, 0.909677403579609, 0.400562617581784),
        new Twist3d(-0.13758, -0.909656, -0.919568, -1.46687976888829, 0.588206325023883, -0.151731902013659),
        new Twist3d(-0.021185, -0.262315, -1.050288, -1.20583486292112, 0.193245661066481, 0.0256363020555752),
        new Twist3d(-0.06799, -0.78043, -0.777717, -1.77013854744968, -0.438241172388478, 0.400350898326818),
        new Twist3d(0.180025, -0.302233, -0.531857, -0.865030520842273, -0.577333708971676, -0.22062020024236),
        new Twist3d(0.300223, -0.330472, 0.013265, -1.28792038735156, -1.06806001731152, -0.029150178540446),
        new Twist3d(0.295355, -0.176881, 0.158989, -0.551090567579901, -0.987885810115117, -0.0214520090941334),
        new Twist3d(0.535773, -0.115238, 0.439204, -0.413834895722797, -1.3976417068997, -0.191948242677331),
        new Twist3d(0.141878, -0.067563, 0.942889, -0.85653514379122, -1.34028882219469, 0.343213653163441),
        new Twist3d(0.167056, -0.254243, 0.848341, -0.507301502948229, -1.11747563905412, 0.0525244478435066),
        new Twist3d(0.030151, -0.210664, 1.157393, -0.404611032165707, -1.4671407084716, -0.134979304015278),
        new Twist3d(-0.24267, 0.013955, 1.399921, -0.455728955157775, -1.32884592654054, 0.0447082617843004),
        new Twist3d(-0.117736, -0.064205, 1.23222, -0.158842222679601, -0.967146915740367, 0.0193953311139082),
        new Twist3d(-0.464491, 0.035535, 1.513351, 0.212144287681626, -1.29067055487205, -0.026052543213993),
        new Twist3d(-0.547413, -0.313258, 1.854834, 0.223140099116198, -1.59346141089434, -0.173729818918662),
        new Twist3d(-0.369376, -0.119816, 1.741766, 0.0992045316425601, -1.39059246484562, 0.185664323612532),
        new Twist3d(-0.466653, -0.423081, 1.619341, 0.236062905165727, -1.32613327195816, -0.0986117824009893),
        new Twist3d(-0.723047, -0.18873, 1.740982, 0.304637611006614, -1.46022348919841, -0.0462852543236242),
        new Twist3d(-0.697133, -0.144414, 1.653017, 0.262255895354473, -1.5662297964293, -0.184929632438737),
        new Twist3d(-0.919308, 0.021711, 1.584763, 0.0280001737996627, -1.12461633793936, 0.0278525046844682),
        new Twist3d(-0.773131, 0.05994, 1.963531, 0.107083331044095, -1.39125360432252, -0.0149085226620963),
        new Twist3d(-0.956178, -0.17764, 1.765532, -0.156571042137141, -1.48904887967828, -0.292943357793616),
        new Twist3d(-0.882454, 0.504142, 1.780991, -0.413643707722198, -1.55962987740136, 0.0551964482905508),
        new Twist3d(-1.015723, 0.401335, 1.686002, -0.290227605410075, -1.10167473669471, 0.0274346079094291),
        new Twist3d(-1.213155, 0.272977, 1.670122, -0.664852516527128, -1.33947448124068, -0.0270408296830635),
        new Twist3d(-0.944068, 0.391829, 1.538088, -0.787679151204913, -1.37728356473823, -0.0373680551792827),
        new Twist3d(-1.070111, 0.795107, 1.276761, -0.720001524326014, -0.896404477951376, 0.060225715266628),
        new Twist3d(-1.051412, 0.416901, 1.446339, -1.13239572912592, -0.882836746431293, -0.137416910070077),
        new Twist3d(-0.362205, 0.813679, 1.403065, -1.39648631611011, -0.866394864540424, 0.106061399227633),
        new Twist3d(-0.17795, 1.161851, 1.14013, -1.31503924710966, -0.309802065301065, -0.0994889911711993),
        new Twist3d(0.127964, 1.22244, 0.866745, -1.37615462962347, 0.210912562524101, 0.077936315938368),
        new Twist3d(0.689009, 1.022678, 0.360651, -1.27563662031258, 0.707455454137221, 0.144672787923124),
        new Twist3d(0.538234, 0.720331, 0.228617, -1.18456281870771, 0.849659897572959, -0.15121043693579),
        new Twist3d(0.600081, 0.681265, -0.276531, -1.0454021506707, 0.982230786094526, 0.0773624921586663),
        new Twist3d(0.332586, 0.404797, -0.598672, -0.994941670707018, 1.00152544011063, 0.0981404012588082),
        new Twist3d(0.541651, 0.244628, -0.587655, -0.390013357030167, 1.32002070280572, 0.0327371531758825),
        new Twist3d(0.379322, 0.117574, -1.08191, 0.142998400814964, 1.57524139344843, 0.232075533299233),
        new Twist3d(0.236503, -0.016149, -1.496781, 0.39047489268203, 1.59150822276062, 0.0525154896363268),
        new Twist3d(-0.026877, -0.176621, -1.432414, 0.704440691655456, 1.17653688815911, -0.0887781502385248),
        new Twist3d(-0.268802, 0.203893, -1.614864, 0.825228123009668, 1.18457113925436, 0.0530306820385683),
        new Twist3d(-0.066131, 0.276455, -1.312948, 0.898990149790477, 0.912348410588481, 0.103091718266439),
        new Twist3d(-0.270376, 0.363963, -1.471168, 1.20224105640514, 0.642642202446447, 0.222147841526293),
        new Twist3d(-0.144907, 0.532892, -1.351611, 1.1222012692925, 0.624795642847375, -0.0079621440746985),
        new Twist3d(-0.256628, 0.898415, -1.201291, 1.11915122473696, 0.583549594689858, 0.137224180288517),
        new Twist3d(-0.357103, 0.573996, -1.109328, 1.17959233599, 0.58685371575463, 0.210657584798462),
        new Twist3d(-0.114703, 0.962794, -0.763045, 1.18214338678481, 0.152065138820256, 0.226238791446961),
        new Twist3d(-0.018501, 0.872159, -0.640821, 1.0399623898323, 0.101411202363869, 0.108444161218877),
        new Twist3d(0.143982, 0.675664, -0.579841, 1.30237263022875, 0.261897881751774, 0.213252886756303),
        new Twist3d(0.000518, 0.69304, -0.408485, 1.11516643469297, 0.0493467716281251, 0.261729831321311),
        new Twist3d(0.110905, 0.582426, -0.419984, 0.950786771281139, -0.025966677341441, 0.27322321290928),
        new Twist3d(0.136388, 0.547006, -0.318257, 0.974010150836529, 0.0281458150672503, 0.273294193928219)
    );

    for (int i = 0; i < initial_poses.size(); i++) {
      var start = initial_poses.get(i);
      var end = final_poses.get(i);

      var twist = start.log(end);
      var start_exp = start.exp(twist);

      final var eps = 1E-5;

      assertAll(
          () -> assertEquals(start_exp.getX(), end.getX(), eps),
          () -> assertEquals(start_exp.getY(), end.getY(), eps),
          () -> assertEquals(start_exp.getZ(), end.getZ(), eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getW(),
                  end.getRotation().getQuaternion().getW(),
                  eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getX(),
                  end.getRotation().getQuaternion().getX(),
                  eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getY(),
                  end.getRotation().getQuaternion().getY(),
                  eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getZ(),
                  end.getRotation().getQuaternion().getZ(),
                  eps));
    }
  }
}
