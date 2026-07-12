// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class SliceTest {
  @Test
  void testDefaultConstructor() {
    var slice = new Slice();
    assertEquals(0, slice.start);
    assertEquals(0, slice.stop);
    assertEquals(1, slice.step);

    assertEquals(0, slice.adjust(3));
    assertEquals(0, slice.start);
    assertEquals(0, slice.stop);
    assertEquals(1, slice.step);
  }

  @Test
  void testOneArgConstructor() {
    // none
    {
      var slice = new Slice(Slice.__);
      assertEquals(0, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(3, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(1, slice.step);
    }

    // +
    {
      var slice = new Slice(1);
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);
    }

    // -1
    {
      var slice = new Slice(-1);
      assertEquals(-1, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(1, slice.step);
    }

    // -2
    {
      var slice = new Slice(-2);
      assertEquals(-2, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);
    }
  }

  @Test
  void testTwoArgConstructor() {
    // none, none
    {
      var slice = new Slice(Slice.__, Slice.__);
      assertEquals(0, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(3, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(1, slice.step);
    }

    // none, +
    {
      var slice = new Slice(Slice.__, 1);
      assertEquals(0, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(1, slice.step);
    }

    // none, -
    {
      var slice = new Slice(Slice.__, -1);
      assertEquals(0, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(2, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);
    }

    // +, none
    {
      var slice = new Slice(1, Slice.__);
      assertEquals(1, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(2, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(1, slice.step);
    }

    // -, none
    {
      var slice = new Slice(-1, Slice.__);
      assertEquals(-1, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(1, slice.step);
    }

    // +, +
    {
      var slice = new Slice(1, 2);
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);
    }

    // +, -
    {
      var slice = new Slice(1, -1);
      assertEquals(1, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);
    }

    // -, -
    {
      var slice = new Slice(-2, -1);
      assertEquals(-2, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(1, slice.step);
    }
  }

  @Test
  void testThreeArgConstructor() {
    // none, none, none
    {
      var slice = new Slice(Slice.__, Slice.__, Slice.__);
      assertEquals(0, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(1, slice.step);

      assertEquals(3, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(1, slice.step);
    }

    // none, none, +
    {
      var slice = new Slice(Slice.__, Slice.__, 2);
      assertEquals(0, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(2, slice.step);

      assertEquals(2, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(2, slice.step);
    }

    // none, none, -
    {
      var slice = new Slice(Slice.__, Slice.__, -2);
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(-2, slice.step);

      assertEquals(2, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(-2, slice.step);
    }

    // none, +, +
    {
      var slice = new Slice(Slice.__, 1, 2);
      assertEquals(0, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(2, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(0, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(2, slice.step);
    }

    // none, +, -
    {
      var slice = new Slice(Slice.__, 1, -2);
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(-2, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(-2, slice.step);
    }

    // none, -, -
    {
      var slice = new Slice(Slice.__, -2, -1);
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(-1, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(-1, slice.step);
    }

    // +, none, +
    {
      var slice = new Slice(1, Slice.__, 2);
      assertEquals(1, slice.start);
      assertEquals(Integer.MAX_VALUE, slice.stop);
      assertEquals(2, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(3, slice.stop);
      assertEquals(2, slice.step);
    }

    // +, none, -
    {
      var slice = new Slice(1, Slice.__, -2);
      assertEquals(1, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(-2, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(-2, slice.step);
    }

    // +, +, +
    {
      var slice = new Slice(1, 2, 2);
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(2, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(2, slice.step);
    }

    // +, +, -
    {
      var slice = new Slice(2, 1, -2);
      assertEquals(2, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(-2, slice.step);

      assertEquals(1, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(-2, slice.step);
    }
  }

  @Test
  void testEmptySlices() {
    // +, +, +
    {
      var slice = new Slice(2, 1, 2);
      assertEquals(2, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(2, slice.step);

      assertEquals(0, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(1, slice.stop);
      assertEquals(2, slice.step);
    }

    // +, +, -
    {
      var slice = new Slice(1, 2, -2);
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(-2, slice.step);

      assertEquals(0, slice.adjust(3));
      assertEquals(1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(-2, slice.step);
    }

    // +, -, -
    {
      var slice = new Slice(3, -1, -2);
      assertEquals(3, slice.start);
      assertEquals(-1, slice.stop);
      assertEquals(-2, slice.step);

      assertEquals(0, slice.adjust(3));
      assertEquals(2, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(-2, slice.step);
    }
  }

  @Test
  void testStepUBGuard() {
    {
      // none, none, INT_MIN
      var slice = new Slice(Slice.__, Slice.__, Integer.MIN_VALUE);
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // none, +, INT_MIN
      var slice = new Slice(Slice.__, 2, Integer.MIN_VALUE);
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // none, -, INT_MIN
      var slice = new Slice(Slice.__, -2, Integer.MIN_VALUE);
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(Integer.MAX_VALUE, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // +, none, INT_MIN
      var slice = new Slice(1, Slice.__, Integer.MIN_VALUE);
      assertEquals(1, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(1, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // -, none, INT_MIN
      var slice = new Slice(-2, Slice.__, Integer.MIN_VALUE);
      assertEquals(-2, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(-2, slice.start);
      assertEquals(Integer.MIN_VALUE, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // +, +, INT_MIN
      var slice = new Slice(1000, 0, Integer.MIN_VALUE);
      assertEquals(1000, slice.start);
      assertEquals(0, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(1000, slice.start);
      assertEquals(0, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // +, -, INT_MIN
      var slice = new Slice(1000, -2, Integer.MIN_VALUE);
      assertEquals(1000, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(1000, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // -, +, INT_MIN
      var slice = new Slice(-1, 2, Integer.MIN_VALUE);
      assertEquals(-1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(-1, slice.start);
      assertEquals(2, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }

    {
      // -, -, INT_MIN
      var slice = new Slice(-1, -2, Integer.MIN_VALUE);
      assertEquals(-1, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(Integer.MIN_VALUE + 1, slice.step);

      slice.step = -slice.step;
      assertEquals(-1, slice.start);
      assertEquals(-2, slice.stop);
      assertEquals(Integer.MAX_VALUE, slice.step);
    }
  }
}
