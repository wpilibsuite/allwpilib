package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Transform2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.VecBuilder;

@SuppressWarnings("CheckStyle")
public class SwerveDrivePoseEstimatorTest {
    @Test
    public void testAccuracy() {
        var kinematics = new SwerveDriveKinematics(
                new Translation2d(1, 1),
                new Translation2d(1, -1),
                new Translation2d(-1, -1),
                new Translation2d(-1, 1)
        );
        var estimator = new SwerveDrivePoseEstimator(
                new Rotation2d(), new Pose2d(), kinematics,
                VecBuilder.fill(3, 3, 3),
                VecBuilder.fill(0.1, 0.1, 0.1)
        );

        var traj = TrajectoryGenerator.generateTrajectory(
                List.of(new Pose2d(), new Pose2d(20, 20, Rotation2d.fromDegrees(0)),
                        new Pose2d(23, 23, Rotation2d.fromDegrees(173)), new Pose2d(54, 54, new Rotation2d())),
                new TrajectoryConfig(0.5, 2));

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

            if (lastVisionUpdateTime + visionUpdateRate < t) {
                if (lastVisionPose != null) {
                    estimator.addVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
                }
                lastVisionPose = groundtruthState.poseMeters.transformBy(
                        new Transform2d(new Translation2d(rand.nextGaussian() * 0.1, rand.nextGaussian() * 0.1),
                                new Rotation2d(rand.nextGaussian() * 0.01)));
                lastVisionUpdateTime = t;

                visionXs.add(lastVisionPose.getTranslation().getX());
                visionYs.add(lastVisionPose.getTranslation().getY());
            }

            var moduleStates = kinematics.toSwerveModuleStates(new ChassisSpeeds(
                    groundtruthState.velocityMetersPerSecond,
                    0.0,
                    groundtruthState.velocityMetersPerSecond * groundtruthState.curvatureRadPerMeter)
            );
            for (var moduleState : moduleStates) {
                moduleState.angle = moduleState.angle.plus(new Rotation2d(rand.nextGaussian() * 0.5));
                moduleState.speedMetersPerSecond += rand.nextGaussian() * 1;
            }

            var xHat = estimator.updateWithTime(
                    t,
                    groundtruthState.poseMeters.getRotation().plus(new Rotation2d(rand.nextGaussian() * 0.001)),
                    moduleStates);

            double error = groundtruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
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

        System.out.println("Mean error (meters): " + errorSum / (traj.getTotalTimeSeconds() / dt));
        System.out.println("Max error (meters):  " + maxError);

        //var chartBuilder = new XYChartBuilder();
        //chartBuilder.title = "The Magic of Sensor Fusion";
        //var chart = chartBuilder.build();

        //chart.addSeries("Vision", visionXs, visionYs);
        //chart.addSeries("Trajectory", trajXs, trajYs);
        //chart.addSeries("xHat", observerXs, observerYs);

        //new SwingWrapper<>(chart).displayChart();
        //try {
        //    Thread.sleep(1000000000);
        //} catch (InterruptedException e) {
        //}
    }
}
