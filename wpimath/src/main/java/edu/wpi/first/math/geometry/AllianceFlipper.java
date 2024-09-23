package edu.wpi.first.math.geometry;

import java.util.HashMap;
import java.util.function.BooleanSupplier;

/**
 * A utility to standardize flipping of coordinate data based on the current alliance across
 * different years.
 *
 * <p>If every vendor used this, the user would be able to specify the year and no matter the year
 * the vendor's code is from, the user would be able to flip as expected.
 */
public class AllianceFlipper {
  private AllianceFlipper() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /** The flipper to use for flipping coordinates. */
  public static enum Flipper {
    /**
     * X becomes fieldLength - x, leaves the y coordinate unchanged, and heading becomes PI -
     * heading.
     */
    VERTICALLY_MIRRORED {
      public double flipX(double x) {
        return activeYear.fieldLength - x;
      }

      public double flipY(double y) {
        return y;
      }

      public double flipHeading(double heading) {
        return Math.PI - heading;
      }
    },
    /** X becomes fieldLength - x, Y becomes fieldWidth - y, and heading becomes PI - heading. */
    ROTATIONALLY_MIRRORED {
      public double flipX(double x) {
        return activeYear.fieldLength - x;
      }

      public double flipY(double y) {
        return activeYear.fieldWidth - y;
      }

      public double flipHeading(double heading) {
        return Math.PI - heading;
      }
    };

    /**
     * Flips the X coordinate.
     *
     * @param x The X coordinate to flip.
     * @return The flipped X coordinate.
     */
    public abstract double flipX(double x);

    /**
     * Flips the Y coordinate.
     *
     * @param y The Y coordinate to flip.
     * @return The flipped Y coordinate.
     */
    public abstract double flipY(double y);

    /**
     * Flips the heading.
     *
     * @param heading The heading to flip.
     * @return The flipped heading.
     */
    public abstract double flipHeading(double heading);
  }

  /**
   * An interface for objects that can be flipped based on the current alliance.
   */
  public static interface Flippable<Self extends Flippable<Self>> {
    /**
     * Flips the object based on the supplied flipper.
     */
    public Self flip(Flipper flipper);

    /**
     * Flips the object based on the active flipper.
     */
    public default Self flip() {
      return flip(getFlipper());
    }

    /**
     * Flips the object if on the red alliance, otherwise returns the object unchanged.
     */
    @SuppressWarnings("unchecked")
    public default Self flipIfRed() {
        return onRed() ? flip() : (Self) this;
    }
  }

  private static record YearInfo(Flipper flipper, double fieldLength, double fieldWidth) {}

  private static final HashMap<Integer, YearInfo> flipperMap =
      new HashMap<Integer, YearInfo>() {
        {
          put(2022, new YearInfo(Flipper.ROTATIONALLY_MIRRORED, 16.4592, 8.2296));
          put(2023, new YearInfo(Flipper.VERTICALLY_MIRRORED, 16.54175, 8.0137));
          put(2024, new YearInfo(Flipper.VERTICALLY_MIRRORED, 16.54175, 8.211));
        }
      };

  private static YearInfo activeYear = flipperMap.get(2024);
  private static BooleanSupplier onRed = () -> false;

  /**
   * Get the flipper that is currently active for flipping coordinates. It's reccomended not to
   * store this locally as the flipper may change.
   *
   * @return The active flipper.
   */
  public static Flipper getFlipper() {
    return activeYear.flipper;
  }

  /**
   * Returns if you are on red alliance, if the alliance is unknown it will return false.
   *
   * @return If you are on red alliance.
   */
  public static boolean onRed() {
    return onRed.getAsBoolean();
  }

  /**
   * Returns if you are on blue alliance, if the alliance is unknown it will return true.
   *
   * @return If you are on blue alliance.
   */
  public static boolean onBlue() {
    return !onRed.getAsBoolean();
  }

  /**
   * Set the year to determine the Alliance Coordinate Flipper to use.
   *
   * @param year The year to set the flipper to. [2022 - 2024]
   */
  public static void setYear(int year) {
    if (!flipperMap.containsKey(year)) {
      // Throw an exception instead of just reporting an error
      // because not flipping correctly during an auto routine
      // could cause a robot to damage itself or others.
      throw new IllegalArgumentException("Year " + year + " is not supported.");
    }
    activeYear = flipperMap.get(year);
  }

  /**
   * Set the `onRed` resolver to determine if the robot is on the red alliance.
   * 
   * @param onRedResolver The resolver to determine if the robot is on the red alliance.
   */
  public static void setOnRed(BooleanSupplier onRedResolver) {
    // cannot access driverstation in wpimath
    onRed = onRedResolver;
  }

  /**
   * Flips the X coordinate.
   *
   * @param x The X coordinate to flip.
   * @return The flipped X coordinate.
   */
  public static double flipX(double x) {
    return activeYear.flipper.flipX(x);
  }

  /**
   * Flips the Y coordinate.
   *
   * @param y The Y coordinate to flip.
   * @return The flipped Y coordinate.
   */
  public static double flipY(double y) {
    return activeYear.flipper.flipY(y);
  }

  /**
   * Flips the heading.
   *
   * @param heading The heading to flip.
   * @return The flipped heading.
   */
  public static double flipHeading(double heading) {
    return activeYear.flipper.flipHeading(heading);
  }

  /**
   * Flips the {@link Flippable} object.
   *
   * @param <T> The type of the object to flip.
   * @param flippable The object to flip.
   * @return The flipped object.
   */
  public static <T extends Flippable<T>> T flip(Flippable<T> flippable) {
    return flippable.flip();
  }

  /**
   * Flips the {@link Flippable} object if on the red alliance.
   *
   * @param <T> The type of the object to flip.
   * @param flippable The object to flip.
   * @return The flipped object.
   * 
   * @see #onRed() A way of determining if you are on the red alliance.
   * @see Flippable#flipIfRed() An instance method that does the same thing.
   */
  public static <T extends Flippable<T>> T flipIfRed(Flippable<T> flippable) {
    return flippable.flipIfRed();
  }
}
