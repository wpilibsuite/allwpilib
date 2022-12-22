// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag.jni;

import edu.wpi.first.apriltag.AprilTagDetection;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public class AprilTagJNI {
  static boolean libraryLoaded = false;

  static RuntimeLoader<AprilTagJNI> loader = null;

  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        loader =
            new RuntimeLoader<>(
                "apriltagjni", RuntimeLoader.getDefaultExtractionRoot(), AprilTagJNI.class);
        loader.loadLibrary();
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  public static native long createDetector();

  public static native void destroyDetector(long det);

  public static native void setNumThreads(long det, int val);

  public static native int getNumThreads(long det);

  public static native void setQuadDecimate(long det, float val);

  public static native float getQuadDecimate(long det);

  public static native void setQuadSigma(long det, float val);

  public static native float getQuadSigma(long det);

  public static native void setRefineEdges(long det, boolean val);

  public static native boolean getRefineEdges(long det);

  public static native void setDecodeSharpening(long det, double val);

  public static native double getDecodeSharpening(long det);

  public static native void setDebug(long det, boolean val);

  public static native boolean getDebug(long det);

  public static native void setQuadMinClusterPixels(long det, int val);

  public static native int getQuadMinClusterPixels(long det);

  public static native void setQuadMaxNumMaxima(long det, int val);

  public static native int getQuadMaxNumMaxima(long det);

  public static native void setQuadCriticalAngle(long det, float val);

  public static native float getQuadCriticalAngle(long det);

  public static native void setQuadMaxLineFitMSE(long det, float val);

  public static native float getQuadMaxLineFitMSE(long det);

  public static native void setQuadMinWhiteBlackDiff(long det, int val);

  public static native int getQuadMinWhiteBlackDiff(long det);

  public static native void setQuadDeglitch(long det, boolean val);

  public static native boolean getQuadDeglitch(long det);

  public static native boolean addFamily(long det, String fam, int bitsCorrected);

  public static native void removeFamily(long det, String fam);

  public static native void clearFamilies(long det);

  public static native AprilTagDetection[] detect(
      long det, int width, int height, int stride, long bufAddr);

  public static native Transform3d estimatePoseHomography(
      double[] homography, double tagSize, double fx, double fy, double cx, double cy);

  public static native AprilTagDetection.PoseEstimate estimatePoseOrthogonalIteration(
      double[] homography,
      double[] corners,
      double tagSize,
      double fx,
      double fy,
      double cx,
      double cy,
      int nIters);

  public static native Transform3d estimatePose(
      double[] homography,
      double[] corners,
      double tagSize,
      double fx,
      double fy,
      double cx,
      double cy);
}
