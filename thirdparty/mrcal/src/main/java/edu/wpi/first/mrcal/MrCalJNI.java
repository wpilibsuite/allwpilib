// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.mrcal;

import java.util.Arrays;
import java.util.List;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfPoint2f;

public class MrCalJNI {
  @SuppressWarnings("MemberName")
  public static class MrCalResult {
    public boolean success;
    public double[] intrinsics;
    public double rms_error;
    public double[] residuals;
    public double warp_x;
    public double warp_y;
    public int nOutliers;

    public MrCalResult(boolean success) {
      this.success = success;
    }

    public MrCalResult(
        boolean success,
        double[] intrinsics,
        double rms_error,
        double[] residuals,
        double warp_x,
        double warp_y,
        int nOutliers) {
      this.success = success;
      this.intrinsics = intrinsics;
      this.rms_error = rms_error;
      this.residuals = residuals;
      this.warp_x = warp_x;
      this.warp_y = warp_y;
      this.nOutliers = nOutliers;
    }

    @Override
    public String toString() {
      return "MrCalResult [success="
          + success
          + ", intrinsics="
          + Arrays.toString(intrinsics)
          + ", rms_error="
          + rms_error
          + ", warp_x="
          + warp_x
          + ", warp_y="
          + warp_y
          + ", nOutliers="
          + nOutliers
          + "]";
    }
  }

  public static native MrCalResult mrcal_calibrate_camera(
      double[] observations_board,
      int boardWidth,
      int boardHeight,
      double boardSpacing,
      int imageWidth,
      int imageHeight,
      double focalLen);

  public static native boolean undistort_mrcal(
      long srcMat,
      long dstMat,
      long cameraMat,
      long distCoeffsMat,
      int lensModelOrdinal,
      int order,
      int Nx,
      int Ny,
      int fov_x_deg);

  public static MrCalResult calibrateCamera(
      List<MatOfPoint2f> board_corners,
      List<MatOfFloat> board_corner_levels,
      int boardWidth,
      int boardHeight,
      double boardSpacing,
      int imageWidth,
      int imageHeight,
      double focalLen) {
    double[] observations = new double[boardWidth * boardHeight * 3 * board_corners.size()];

    if (!(board_corners.size() == board_corner_levels.size())) {
      return new MrCalResult(false);
    }

    int i = 0;
    for (int b = 0; b < board_corners.size(); b++) {
      var board = board_corners.get(b);
      var levels = board_corner_levels.get(b).toArray();
      var corners = board.toArray();

      if (!(corners.length == levels.length && corners.length == boardWidth * boardHeight)) {
        return new MrCalResult(false);
      }

      // Assume that we're correct in terms of row/column major-ness (lol)
      for (int c = 0; c < corners.length; c++) {
        var corner = corners[c];
        float level = levels[c];

        observations[i * 3 + 0] = corner.x;
        observations[i * 3 + 1] = corner.y;
        observations[i * 3 + 2] = level;

        i += 1;
      }
    }

    if (i * 3 != observations.length) {
      return new MrCalResult(false);
    }

    return mrcal_calibrate_camera(
        observations, boardWidth, boardHeight, boardSpacing, imageWidth, imageHeight, focalLen);
  }
}
