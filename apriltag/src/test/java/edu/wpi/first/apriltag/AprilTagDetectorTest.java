// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.util.RuntimeLoader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

@SuppressWarnings("PMD.MutableStaticState")
class AprilTagDetectorTest {
  @SuppressWarnings("MemberName")
  AprilTagDetector detector;

  @BeforeAll
  static void beforeAll() {
    try {
      RuntimeLoader.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    } catch (IOException e) {
      try {
        // Try adding a debug postfix
        RuntimeLoader.loadLibrary(Core.NATIVE_LIBRARY_NAME + "d");
      } catch (IOException ex) {
        fail(ex);
      }
    }
  }

  @BeforeEach
  void beforeEach() {
    detector = new AprilTagDetector();
  }

  @AfterEach
  void afterEach() {
    detector.close();
  }

  @Test
  void testConfigDefaults() {
    var config = detector.getConfig();
    assertEquals(new AprilTagDetector.Config(), config);
  }

  @Test
  void testQtpDefaults() {
    var params = detector.getQuadThresholdParameters();
    assertEquals(new AprilTagDetector.QuadThresholdParameters(), params);
  }

  @Test
  void testSetConfigNumThreads() {
    var newConfig = new AprilTagDetector.Config();
    newConfig.numThreads = 2;
    detector.setConfig(newConfig);
    var config = detector.getConfig();
    assertEquals(2, config.numThreads);
  }

  @Test
  void testQtpMinClusterPixels() {
    var newParams = new AprilTagDetector.QuadThresholdParameters();
    newParams.minClusterPixels = 8;
    detector.setQuadThresholdParameters(newParams);
    var params = detector.getQuadThresholdParameters();
    assertEquals(8, params.minClusterPixels);
  }

  @Test
  void testAdd16h5() {
    assertDoesNotThrow(() -> detector.addFamily("tag16h5"));
    // duplicate addition is also okay
    assertDoesNotThrow(() -> detector.addFamily("tag16h5"));
  }

  @Test
  void testAdd36h11() {
    assertDoesNotThrow(() -> detector.addFamily("tag36h11"));
  }

  @Test
  void testAddMultiple() {
    assertDoesNotThrow(() -> detector.addFamily("tag16h5"));
    assertDoesNotThrow(() -> detector.addFamily("tag36h11"));
  }

  @Test
  void testRemoveFamily() {
    // okay to remove non-existent family
    detector.removeFamily("tag16h5");

    // add and remove
    detector.addFamily("tag16h5");
    detector.removeFamily("tag16h5");
  }

  @SuppressWarnings("PMD.AssignmentInOperand")
  public Mat loadImage(String resource) throws IOException {
    Mat encoded;
    try (InputStream is = getClass().getResource(resource).openStream()) {
      try (ByteArrayOutputStream os = new ByteArrayOutputStream(is.available())) {
        byte[] buffer = new byte[4096];
        int bytesRead;
        while ((bytesRead = is.read(buffer)) != -1) {
          os.write(buffer, 0, bytesRead);
        }
        encoded = new Mat(1, os.size(), CvType.CV_8U);
        encoded.put(0, 0, os.toByteArray());
      }
    }
    Mat image = Imgcodecs.imdecode(encoded, Imgcodecs.IMREAD_COLOR);
    encoded.release();
    Imgproc.cvtColor(image, image, Imgproc.COLOR_BGR2GRAY);
    return image;
  }

  @Test
  void testDecodeCropped() {
    detector.addFamily("tag16h5");
    detector.addFamily("tag36h11");

    Mat image;
    try {
      image = loadImage("tag1_640_480.jpg");
    } catch (IOException ex) {
      fail(ex);
      return;
    }

    // Pre-knowledge -- the tag is within this ROI of this particular test image
    var cropped = image.submat(100, 400, 220, 570);

    try {
      AprilTagDetection[] results = detector.detect(cropped);
      assertEquals(1, results.length);
      assertEquals("tag36h11", results[0].getFamily());
      assertEquals(1, results[0].getId());
      assertEquals(0, results[0].getHamming());
    } finally {
      cropped.release();
      image.release();
    }
  }

  @Test
  void testDecodeAndPose() {
    detector.addFamily("tag16h5");
    detector.addFamily("tag36h11");

    Mat image;
    try {
      image = loadImage("tag1_640_480.jpg");
    } catch (IOException ex) {
      fail(ex);
      return;
    }
    try {
      AprilTagDetection[] results = detector.detect(image);
      assertEquals(1, results.length);
      assertEquals("tag36h11", results[0].getFamily());
      assertEquals(1, results[0].getId());
      assertEquals(0, results[0].getHamming());

      var estimator =
          new AprilTagPoseEstimator(new AprilTagPoseEstimator.Config(0.2, 500, 500, 320, 240));
      AprilTagPoseEstimate est = estimator.estimateOrthogonalIteration(results[0], 200);
      assertEquals(Transform3d.kZero, est.pose2);
      Transform3d pose = estimator.estimate(results[0]);
      assertEquals(est.pose1, pose);
    } finally {
      image.release();
    }
  }

  /**
   * This tag is rotated such that the top is closer to the camera than the bottom. In the camera
   * frame, with +x to the right, this is a rotation about +X by 45 degrees.
   */
  @Test
  void testPoseRotatedX() {
    detector.addFamily("tag16h5");

    Mat image;
    try {
      image = loadImage("tag2_45deg_X.png");
    } catch (IOException ex) {
      fail(ex);
      return;
    }
    try {
      AprilTagDetection[] results = detector.detect(image);
      assertEquals(1, results.length);

      var estimator =
          new AprilTagPoseEstimator(
              new AprilTagPoseEstimator.Config(
                  0.2, 500, 500, image.cols() / 2.0, image.rows() / 2.0));
      AprilTagPoseEstimate est = estimator.estimateOrthogonalIteration(results[0], 200);

      assertEquals(Units.degreesToRadians(45), est.pose1.getRotation().getX(), 0.1);
      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getY(), 0.1);
      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getZ(), 0.1);
    } finally {
      image.release();
    }
  }

  /**
   * This tag is rotated such that the right is closer to the camera than the left. In the camera
   * frame, with +y down, this is a rotation of 45 degrees about +y.
   */
  @Test
  void testPoseRotatedY() {
    detector.addFamily("tag16h5");

    Mat image;
    try {
      image = loadImage("tag2_45deg_y.png");
    } catch (IOException ex) {
      fail(ex);
      return;
    }
    try {
      AprilTagDetection[] results = detector.detect(image);
      assertEquals(1, results.length);

      var estimator =
          new AprilTagPoseEstimator(
              new AprilTagPoseEstimator.Config(
                  0.2, 500, 500, image.cols() / 2.0, image.rows() / 2.0));
      AprilTagPoseEstimate est = estimator.estimateOrthogonalIteration(results[0], 200);

      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getX(), 0.1);
      assertEquals(Units.degreesToRadians(45), est.pose1.getRotation().getY(), 0.1);
      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getZ(), 0.1);
    } finally {
      image.release();
    }
  }

  /** This tag is facing right at the camera -- no rotation should be observed. */
  @Test
  void testPoseStraightOn() {
    detector.addFamily("tag16h5");

    Mat image;
    try {
      image = loadImage("tag2_16h5_straight.png");
    } catch (IOException ex) {
      fail(ex);
      return;
    }
    try {
      AprilTagDetection[] results = detector.detect(image);
      assertEquals(1, results.length);

      var estimator =
          new AprilTagPoseEstimator(
              new AprilTagPoseEstimator.Config(
                  0.2, 500, 500, image.cols() / 2.0, image.rows() / 2.0));
      AprilTagPoseEstimate est = estimator.estimateOrthogonalIteration(results[0], 200);

      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getX(), 0.1);
      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getY(), 0.1);
      assertEquals(Units.degreesToRadians(0), est.pose1.getRotation().getZ(), 0.1);
    } finally {
      image.release();
    }
  }
}
