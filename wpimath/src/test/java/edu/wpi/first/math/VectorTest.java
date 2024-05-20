// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class VectorTest {
  @Test
  void testVectorPlus() {
    var vec1 = VecBuilder.fill(1.0, 2.0, 3.0);
    var vec2 = VecBuilder.fill(4.0, 5.0, 6.0);
    var result1 = vec1.plus(vec2);

    assertEquals(5.0, result1.get(0, 0));
    assertEquals(7.0, result1.get(1, 0));
    assertEquals(9.0, result1.get(2, 0));

    var vec3 = VecBuilder.fill(-1.0, 2.0, -3.0);
    var vec4 = VecBuilder.fill(4.0, -5.0, 6.0);
    var result2 = vec3.plus(vec4);

    assertEquals(3.0, result2.get(0, 0));
    assertEquals(-3.0, result2.get(1, 0));
    assertEquals(3.0, result2.get(2, 0));
  }

  @Test
  void testVectorMinus() {
    var vec1 = VecBuilder.fill(1.0, 2.0, 3.0);
    var vec2 = VecBuilder.fill(4.0, 5.0, 6.0);
    var result1 = vec1.minus(vec2);

    assertEquals(-3.0, result1.get(0, 0));
    assertEquals(-3.0, result1.get(1, 0));
    assertEquals(-3.0, result1.get(2, 0));

    var vec3 = VecBuilder.fill(-1.0, 2.0, -3.0);
    var vec4 = VecBuilder.fill(4.0, -5.0, 6.0);
    var result2 = vec3.minus(vec4);

    assertEquals(-5.0, result2.get(0, 0));
    assertEquals(7.0, result2.get(1, 0));
    assertEquals(-9.0, result2.get(2, 0));
  }

  @Test
  void testVectorDot() {
    var vec1 = VecBuilder.fill(1.0, 2.0, 3.0);
    var vec2 = VecBuilder.fill(4.0, 5.0, 6.0);

    assertEquals(32.0, vec1.dot(vec2));

    var vec3 = VecBuilder.fill(-1.0, 2.0, -3.0);
    var vec4 = VecBuilder.fill(4.0, -5.0, 6.0);

    assertEquals(-32.0, vec3.dot(vec4));
  }

  @Test
  void testVectorNorm() {
    assertEquals(Math.sqrt(14.0), VecBuilder.fill(1.0, 2.0, 3.0).norm());
    assertEquals(Math.sqrt(14.0), VecBuilder.fill(1.0, -2.0, 3.0).norm());
  }

  @Test
  void testVectorUnit() {
    assertEquals(VecBuilder.fill(3.0, 4.0).unit(), VecBuilder.fill(3.0 / 5.0, 4.0 / 5.0));
    assertEquals(VecBuilder.fill(8.0, 15.0).unit(), VecBuilder.fill(8.0 / 17.0, 15.0 / 17.0));
  }

  @Test
  void testVectorProjection() {
    var vec1 = VecBuilder.fill(1.0, 2.0, 3.0);
    var vec2 = VecBuilder.fill(4.0, 5.0, 6.0);
    var res1 = vec1.projection(vec2);

    assertEquals(res1.get(0), 1.662, 0.01);
    assertEquals(res1.get(1), 2.077, 0.01);
    assertEquals(res1.get(2), 2.49, 0.01);

    var vec3 = VecBuilder.fill(-1.0, 2.0, -3.0);
    var vec4 = VecBuilder.fill(4.0, -5.0, 6.0);
    var res2 = vec4.projection(vec3);

    assertEquals(res2.get(0), 2.29, 0.01);
    assertEquals(res2.get(1), -4.57, 0.01);
    assertEquals(res2.get(2), 6.86, 0.01);
  }

  @Test
  void testVectorCross() {
    var e1 = VecBuilder.fill(1.0, 0.0, 0.0);
    var e2 = VecBuilder.fill(0.0, 1.0, 0.0);
    assertEquals(Vector.cross(e1, e2), VecBuilder.fill(0.0, 0.0, 1.0));

    var vec1 = VecBuilder.fill(1.0, 2.0, 3.0);
    var vec2 = VecBuilder.fill(3.0, 4.0, 5.0);
    assertEquals(Vector.cross(vec1, vec2), VecBuilder.fill(-2.0, 4.0, -2.0));
  }
}
