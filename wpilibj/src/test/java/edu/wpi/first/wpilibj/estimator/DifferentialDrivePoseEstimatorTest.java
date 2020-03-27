package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Nat;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class DifferentialDrivePoseEstimatorTest {
  @SuppressWarnings("LocalVariableName")
  @Test
  public void testAccuracy() {
    var estimator = new DifferentialDrivePoseEstimator(new Rotation2d(), new Pose2d(),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.02, 0.02, 0.01),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.1, 0.1, 0.01));

    var traj = TrajectoryGenerator.generateTrajectory(
            List.of(
                    new Pose2d(),
                    new Pose2d(20, 20, Rotation2d.fromDegrees(0)),
                    new Pose2d(23, 23, Rotation2d.fromDegrees(173)),
                    new Pose2d(54, 54, new Rotation2d())
            ),
            new TrajectoryConfig(0.5, 2));

    var kinematics = new DifferentialDriveKinematics(1);
    var rand = new Random(4915);

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> visionXs = new ArrayList<>();
    List<Double> visionYs = new ArrayList<>();

    final double dt = 0.02;
    double t = 0.0;

    final double visionUpdateRate = 0.1;
    Pose2d lastVisionPose = null;
    double lastVisionUpdateTime = Double.NEGATIVE_INFINITY;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= traj.getTotalTimeSeconds()) {
      var groundtruthState = traj.sample(t);
      var input = kinematics.toWheelSpeeds(new ChassisSpeeds(
              groundtruthState.velocityMetersPerSecond, 0.0,
              // ds/dt * dtheta/ds = dtheta/dt
              groundtruthState.velocityMetersPerSecond * groundtruthState.curvatureRadPerMeter
      ));

      if (lastVisionUpdateTime + visionUpdateRate + rand.nextGaussian() * 0.4 < t) {
        if (lastVisionPose != null) {
          estimator.addVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
        }
        var groundPose = groundtruthState.poseMeters;
        lastVisionPose = new Pose2d(
                new Translation2d(
                        groundPose.getTranslation().getX() + rand.nextGaussian() * 0.1,
                        groundPose.getTranslation().getY() + rand.nextGaussian() * 0.1
                ),
                new Rotation2d(rand.nextGaussian() * 0.01).plus(groundPose.getRotation())
        );
        lastVisionUpdateTime = t;

        visionXs.add(lastVisionPose.getTranslation().getX());
        visionYs.add(lastVisionPose.getTranslation().getY());
      }

      input.leftMetersPerSecond += rand.nextGaussian() * 0.02;
      input.rightMetersPerSecond += rand.nextGaussian() * 0.02;

      var rotNoise = new Rotation2d(rand.nextGaussian() * 0.01);
      var xHat = estimator.updateWithTime(
              t,
              groundtruthState.poseMeters.getRotation().plus(rotNoise),
              input
      );

      double error =
              groundtruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      trajXs.add(groundtruthState.poseMeters.getTranslation().getX());
      trajYs.add(groundtruthState.poseMeters.getTranslation().getY());
      observerXs.add(xHat.getTranslation().getX());
      observerYs.add(xHat.getTranslation().getY());

      t += dt;
    }

    assertEquals(
            0.009139510897112207, errorSum / (traj.getTotalTimeSeconds() / dt),
            "Incorrect mean error"
    );
    assertEquals(
            0.02270612283984096, maxError,
            "Incorrect max error"
    );

    //System.out.println("Mean error (meters): " + errorSum / (traj.getTotalTimeSeconds() / dt));
    //System.out.println("Max error (meters):  " + maxError);

    //var chartBuilder = new XYChartBuilder();
    //chartBuilder.title = "The Magic of Sensor Fusion";
    //var chart = chartBuilder.build();

    //chart.addSeries("Vision", visionXs, visionYs);
    //chart.addSeries("Trajectory", trajXs, trajYs);
    //chart.addSeries("xHat", observerXs, observerYs);

    //new SwingWrapper<>(chart).displayChart();
    //try {
    //  Thread.sleep(1000000000);
    //} catch (InterruptedException e) {
    //}
  }
}
