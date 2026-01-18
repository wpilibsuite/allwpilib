// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import java.util.OptionalInt;

/** Represents a sequence of elements in an iterable object. */
@SuppressWarnings("PMD.UnusedFormalParameter")
public class Slice {
  /** Type tag used to designate an omitted argument of the slice. */
  public static class None {
    /** Default constructor. */
    public None() {}
  }

  /** Designates an omitted argument of the slice. */
  public static final None __ = null;

  /** Start index (inclusive). */
  public int start = 0;

  /** Stop index (exclusive). */
  public int stop = 0;

  /** Step. */
  public int step = 1;

  /** Constructs a Slice. */
  public Slice() {}

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   */
  public Slice(None start) {
    this(OptionalInt.of(0), OptionalInt.of(Integer.MAX_VALUE), OptionalInt.of(1));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   */
  public Slice(int start) {
    this.start = start;
    this.stop = (start == -1) ? Integer.MAX_VALUE : start + 1;
    this.step = 1;
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   */
  public Slice(None start, None stop) {
    this(OptionalInt.empty(), OptionalInt.empty(), OptionalInt.of(1));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   */
  public Slice(None start, int stop) {
    this(OptionalInt.empty(), OptionalInt.of(stop), OptionalInt.of(1));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   */
  public Slice(int start, None stop) {
    this(OptionalInt.of(start), OptionalInt.empty(), OptionalInt.of(1));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   */
  public Slice(int start, int stop) {
    this(OptionalInt.of(start), OptionalInt.of(stop), OptionalInt.of(1));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(None start, None stop, None step) {
    this(OptionalInt.empty(), OptionalInt.empty(), OptionalInt.empty());
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(None start, None stop, int step) {
    this(OptionalInt.empty(), OptionalInt.empty(), OptionalInt.of(step));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(None start, int stop, None step) {
    this(OptionalInt.empty(), OptionalInt.of(stop), OptionalInt.empty());
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(None start, int stop, int step) {
    this(OptionalInt.empty(), OptionalInt.of(stop), OptionalInt.of(step));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(int start, None stop, None step) {
    this(OptionalInt.of(start), OptionalInt.empty(), OptionalInt.empty());
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(int start, None stop, int step) {
    this(OptionalInt.of(start), OptionalInt.empty(), OptionalInt.of(step));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(int start, int stop, None step) {
    this(OptionalInt.of(start), OptionalInt.of(stop), OptionalInt.empty());
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(int start, int stop, int step) {
    this(OptionalInt.of(start), OptionalInt.of(stop), OptionalInt.of(step));
  }

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  public Slice(OptionalInt start, OptionalInt stop, OptionalInt step) {
    if (!step.isPresent()) {
      this.step = 1;
    } else {
      assert step.getAsInt() != 0;

      this.step = step.getAsInt();
    }

    // Avoid UB for step = -step if step is INT_MIN
    if (this.step == Integer.MIN_VALUE) {
      this.step = -Integer.MAX_VALUE;
    }

    if (!start.isPresent()) {
      if (this.step < 0) {
        this.start = Integer.MAX_VALUE;
      } else {
        this.start = 0;
      }
    } else {
      this.start = start.getAsInt();
    }

    if (!stop.isPresent()) {
      if (this.step < 0) {
        this.stop = Integer.MIN_VALUE;
      } else {
        this.stop = Integer.MAX_VALUE;
      }
    } else {
      this.stop = stop.getAsInt();
    }
  }

  /**
   * Adjusts start and end slice indices assuming a sequence of the specified length.
   *
   * @param length The sequence length.
   * @return The slice length.
   */
  public int adjust(int length) {
    assert step != 0;
    assert step >= -Integer.MAX_VALUE;

    if (start < 0) {
      start += length;

      if (start < 0) {
        start = (step < 0) ? -1 : 0;
      }
    } else if (start >= length) {
      start = (step < 0) ? length - 1 : length;
    }

    if (stop < 0) {
      stop += length;

      if (stop < 0) {
        stop = (step < 0) ? -1 : 0;
      }
    } else if (stop >= length) {
      stop = (step < 0) ? length - 1 : length;
    }

    if (step < 0) {
      if (stop < start) {
        return (start - stop - 1) / -step + 1;
      } else {
        return 0;
      }
    } else {
      if (start < stop) {
        return (stop - start - 1) / step + 1;
      } else {
        return 0;
      }
    }
  }
}
