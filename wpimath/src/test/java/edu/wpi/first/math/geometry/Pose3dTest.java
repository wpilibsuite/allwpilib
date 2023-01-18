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

    for (int i = 0; i < initial_poses.size(); i++) {
      var start = initial_poses.get(i);
      var end = final_poses.get(i);

      var twist = start.log(end);
      var start_exp = start.exp(twist);

      assertAll(
          () -> assertEquals(start_exp.getX(), end.getX(), kEpsilon),
          () -> assertEquals(start_exp.getY(), end.getY(), kEpsilon),
          () -> assertEquals(start_exp.getZ(), end.getZ(), kEpsilon),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getW(),
                  end.getRotation().getQuaternion().getW(),
                  kEpsilon),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getX(),
                  end.getRotation().getQuaternion().getX(),
                  kEpsilon),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getY(),
                  end.getRotation().getQuaternion().getY(),
                  kEpsilon),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getZ(),
                  end.getRotation().getQuaternion().getZ(),
                  kEpsilon));
    }
  }
}
