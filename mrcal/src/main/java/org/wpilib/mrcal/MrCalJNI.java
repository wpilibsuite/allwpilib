// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.mrcal;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfPoint2f;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.linalg.VecBuilder;

public class MrCalJNI {
  public enum CameraLensModel {
    LENSMODEL_OPENCV5,
    LENSMODEL_OPENCV8,
    LENSMODEL_STEREOGRAPHIC,
    LENSMODEL_SPLINED_STEREOGRAPHIC;
  }

  public static class MrCalResult {
    public boolean success;
    public double[] intrinsics;
    public double rms_error;
    public double[] residuals;
    public double warp_x;
    public double warp_y;
    public int N_OUTLIERS;
    public List<Pose3d> optimizedPoses;
    public List<boolean[]> cornersUsed;

    public MrCalResult(boolean success) {
      this.success = success;
    }

    public MrCalResult(
        boolean success,
        int width,
        int height,
        double[] intrinsics,
        double[] optimized_rt_rtoref,
        double rms_error,
        double[] residuals,
        double warp_x,
        double warp_y,
        int N_OUTLIERS,
        boolean[] cornerUseMask) {
      this.success = success;
      this.intrinsics = intrinsics;
      this.rms_error = rms_error;
      this.residuals = residuals;
      this.warp_x = warp_x;
      this.warp_y = warp_y;
      this.N_OUTLIERS = N_OUTLIERS;

      optimizedPoses = new ArrayList<>();
      for (int i = 0; i < optimized_rt_rtoref.length; i += 6) {
        var rot =
            new Rotation3d(
                VecBuilder.fill(
                    optimized_rt_rtoref[i + 0],
                    optimized_rt_rtoref[i + 1],
                    optimized_rt_rtoref[i + 2]));
        var t =
            new Translation3d(
                optimized_rt_rtoref[i + 3], optimized_rt_rtoref[i + 4], optimized_rt_rtoref[i + 5]);

        optimizedPoses.add(new Pose3d(t, rot));
      }

      var cornersPerBoard = width * height;
      cornersUsed = new ArrayList<>();
      for (int cornerIdx = 0; cornerIdx < cornerUseMask.length; cornerIdx += cornersPerBoard) {
        cornersUsed.add(Arrays.copyOfRange(cornerUseMask, cornerIdx, cornerIdx + cornersPerBoard));
      }
    }

    public double[] framePosesToRtToref() {
      double[] ret = new double[optimizedPoses.size() * 6];

      for (int i = 0; i < optimizedPoses.size(); i++) {
        var pose = optimizedPoses.get(i);
        var r = pose.getRotation().toVector();
        ret[i * 6 + 0] = r.get(0);
        ret[i * 6 + 1] = r.get(1);
        ret[i * 6 + 2] = r.get(2);
        var t = pose.getTranslation().toVector();
        ret[i * 6 + 3] = t.get(0);
        ret[i * 6 + 4] = t.get(1);
        ret[i * 6 + 5] = t.get(2);
      }

      return ret;
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
          + ", N_OUTLIERS="
          + N_OUTLIERS
          + "]";
    }
  }

  /**
   * Performs camera calibration using [mrcal](https://mrcal.secretsauce.net/formulation.html)
   *
   * @param observations_board a packed double array containing all observations across all frames.
   *     The array is structured as a flattened list where each observation consists of 3
   *     consecutive doubles: [x, y, level] for each corner of each board in each frame. Structure:
   *     For N frames, each observing a boardWidth×boardHeight checkerboard, the array has length N
   *     × boardWidth × boardHeight × 3. The level value (0-based) represents detection quality and
   *     will be converted to a weight using weight = 0.5^level. A negative level indicates the
   *     corner was not detected (will be marked as an outlier).
   * @param boardWidth the number of internal corners in the horizontal direction of the calibration
   *     board
   * @param boardHeight the number of internal corners in the vertical direction of the calibration
   *     board
   * @param boardSpacing the physical distance (in arbitrary but consistent units) between adjacent
   *     corners on the calibration board. PhotonVision uses meters.
   * @param imageWidth the width in pixels of the images used for calibration
   * @param imageHeight the height in pixels of the images used for calibration
   * @param focalLen an initial guess for the focal length in pixels, used to seed the optimization
   * @return an {@link MrCalResult} object containing the calibration results, including optimized
   *     intrinsics, board poses, RMS error, per-observation residuals, detected outliers, and a
   *     boolean mask indicating which corners were actually used in the final optimization
   */
  public static native MrCalResult mrcal_calibrate_camera(
      double[] observations_board,
      int boardWidth,
      int boardHeight,
      double boardSpacing,
      int imageWidth,
      int imageHeight,
      double focalLen);

  /**
   * Applies mrcal lens distortion correction to undistort pixel coordinates.
   *
   * <p>This method takes distorted pixel coordinates and applies the inverse of mrcal's lens
   * distortion model to produce undistorted (corrected) coordinates. The input coordinates are
   * unprojected through the specified lens model to 3D ray vectors, then reprojected through a
   * pinhole model.
   *
   * @param dstMat a pointer to a cv::Mat containing distorted pixel coordinates (passed as a long
   *     representing the memory address). Must be a CV_64FC2 continuous matrix where each row
   *     contains one (x, y) coordinate pair. The matrix is modified in-place with the undistorted
   *     coordinates as output.
   * @param cameraMat a pointer to the camera calibration matrix cv::Mat (3×3, opencv format).
   * @param distCoeffsMat a pointer to the distortion coefficients cv::Mat. The interpretation of
   *     these coefficients depends on the specified lens model.
   * @param lensModelOrdinal the ordinal value of the {@link CameraLensModel} enum indicating which
   *     lens distortion model to use for undistortion
   * @param order the spline interpolation order to use when warping pixels, if the lensmodel is
   *     splined. Unused otherwise
   * @param Nx the number of control points in the x-direction for the distortion spline model.
   *     Unused otherwise
   * @param Ny the number of control points in the y-direction for the distortion spline model.
   *     Unused otherwise
   * @param fov_x_deg the horizontal field of view in degrees used for certain lens model, if
   *     splined. Unused otherwise calculations
   * @return true if the undistortion was successful, false otherwise
   */
  public static native boolean undistort_mrcal(
      long dstMat,
      long cameraMat,
      long distCoeffsMat,
      int lensModelOrdinal,
      int order,
      int Nx,
      int Ny,
      int fov_x_deg);

  /**
   * Computes projection uncertainty for camera calibration across a grid of image points.
   *
   * <p>This method evaluates the uncertainty in 3D point projection due to calibration error. It
   * computes uncertainty at a regular grid of points across the image plane.
   *
   * @param observations_board a packed double array containing calibration observations with the
   *     same structure as used in {@link #mrcal_calibrate_camera}. For N frames observing a
   *     boardWidth×boardHeight checkerboard, the array has length N × boardWidth × boardHeight × 3,
   *     where each triplet is [x, y, level].
   * @param intrinsics a double array containing the camera intrinsic parameters. The exact content
   *     and length depend on the lens distortion model being used, but typically includes focal
   *     length, principal point, and distortion coefficients.
   * @param rt_ref_frames a packed double array containing the poses (rotations and translations) of
   *     the calibration board relative to the camera for each frame. The array is structured as N
   *     consecutive 6-element groups, where each group represents one frame's pose: [rx, ry, rz,
   *     tx, ty, tz]. The rotation components (rx, ry, rz) form a rotation vector (axis-angle
   *     representation), and (tx, ty, tz) represent the translation. Total array length is N × 6,
   *     where N is the number of observed frames.
   * @param boardWidth the number of internal corners in the horizontal direction of the calibration
   *     board
   * @param boardHeight the number of internal corners in the vertical direction of the calibration
   *     board
   * @param boardSpacing the physical distance between adjacent corners on the calibration board
   *     (must match the value used in calibration). PhotonVision uses meters.
   * @param imageWidth the width in pixels of the camera imager at this particular VideoMode
   * @param imageHeight the height in pixels of the camera imager at this particular VideoMode
   * @param sampleGridWidth the number of sample points in the horizontal direction for uncertainty
   *     computation
   * @param sampleGridHeight the number of sample points in the vertical direction for uncertainty
   *     computation
   * @param warpX the x-component of lens distortion warp, or zero if no warp was estimated.
   * @param warpY the y-component of lens distortion warp
   * @return a packed double array containing uncertainty values at each sampled grid point. The
   *     array is structured as a sequence of 3D points (mrcal_point3_t), representing [u, v,
   *     uncertainty] for each point we sampled. Total array length is sampleGridWidth ×
   *     sampleGridHeight × 3. Returns null if computation fails.
   */
  public static native double[] compute_uncertainty(
      double[] observations_board,
      double[] intrinsics,
      double[] rt_ref_frames,
      int boardWidth,
      int boardHeight,
      double boardSpacing,
      int imageWidth,
      int imageHeight,
      int sampleGridWidth,
      int sampleGridHeight,
      double warpX,
      double warpY);

  /**
   * Convert a list of board-pixel-corners and detection levels for each snapshot of a chessboard
   * boardWidth x boardHeight to a packed double[] suitable to pass to
   * MrCalJni::mrcal_calibrate_camera. Levels will be converted to weights using weight = 0.5^level,
   * as explained
   * [here](https://github.com/dkogan/mrcal/blob/7cd9ac4c854a4b244a35f554c9ebd0464d59e9ff/mrcal-calibrate-cameras#L152)
   */
  private static double[] makeObservations(
      List<MatOfPoint2f> board_corners,
      List<MatOfFloat> board_corner_levels,
      int boardWidth,
      int boardHeight) {
    double[] observations = new double[boardWidth * boardHeight * 3 * board_corners.size()];

    int i = 0;
    for (int b = 0; b < board_corners.size(); b++) {
      var board = board_corners.get(b);
      var levels = board_corner_levels.get(b).toArray();
      var corners = board.toArray();

      if (!(corners.length == levels.length && corners.length == boardWidth * boardHeight)) {
        return null;
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
      return null;
    }

    return observations;
  }

  /**
   * High-level wrapper for camera calibration using mrcal.
   *
   * <p>Converts detected chessboard corners and their detection levels into a packed double array,
   * then calls {@link #mrcal_calibrate_camera} to perform calibration. Each corner's detection
   * level is converted to a weight (0.5^level), and negative levels indicate undetected corners.
   *
   * @param board_corners List of detected chessboard corners for each frame
   * @param board_corner_levels List of detection levels for each corner in each frame. Point weight
   *     is calculated as weight = 0.5^level. Negative weight will ignore this observation
   * @param boardWidth Number of internal corners horizontally
   * @param boardHeight Number of internal corners vertically
   * @param boardSpacing Physical spacing between corners (meters)
   * @param imageWidth Image width in pixels
   * @param imageHeight Image height in pixels
   * @param focalLen Initial guess for focal length (pixels)
   * @return Calibration result with optimized intrinsics, poses, and error metrics
   */
  public static MrCalResult calibrateCamera(
      List<MatOfPoint2f> board_corners,
      List<MatOfFloat> board_corner_levels,
      int boardWidth,
      int boardHeight,
      double boardSpacing,
      int imageWidth,
      int imageHeight,
      double focalLen) {
    if (!(board_corners.size() == board_corner_levels.size())) {
      return new MrCalResult(false);
    }

    var observations =
        makeObservations(board_corners, board_corner_levels, boardWidth, boardHeight);

    return mrcal_calibrate_camera(
        observations, boardWidth, boardHeight, boardSpacing, imageWidth, imageHeight, focalLen);
  }
}
