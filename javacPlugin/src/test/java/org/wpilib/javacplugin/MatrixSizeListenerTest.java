// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import javax.tools.JavaFileObject;
import org.junit.jupiter.api.Test;

class MatrixSizeListenerTest {
  private static final JavaFileObject NUM =
      JavaFileObjects.forSourceString(
          "org.wpilib.math.util.Num",
          """
      package org.wpilib.math.util;

      public class Num {}
      """);

  private static final JavaFileObject NAT =
      JavaFileObjects.forSourceString(
          "org.wpilib.math.util.Nat",
          """
      package org.wpilib.math.util;

      public interface Nat<T> {
        int getNum();
        static Nat<org.wpilib.math.numbers.N3> N3() { return null; }
        static Nat<org.wpilib.math.numbers.N2> N2() { return null; }
        static Nat<org.wpilib.math.numbers.N1> N1() { return null; }
      }
      """);

  private static final JavaFileObject N3 =
      JavaFileObjects.forSourceString(
          "org.wpilib.math.numbers.N3",
          """
      package org.wpilib.math.numbers;

      import org.wpilib.math.util.*;

      public final class N3 extends Num implements Nat<N3> {
        public int getNum() { return 3; }
        public static final N3 instance = new N3();
      }
      """);

  private static final JavaFileObject N2 =
      JavaFileObjects.forSourceString(
          "org.wpilib.math.numbers.N2",
          """
      package org.wpilib.math.numbers;

      import org.wpilib.math.util.*;

      public final class N2 extends Num implements Nat<N2> {
        public int getNum() { return 2; }
        public static final N2 instance = new N2();
      }
      """);

  private static final JavaFileObject MATRIX =
      JavaFileObjects.forSourceString(
          "org.wpilib.math.linalg.Matrix",
          """
      package org.wpilib.math.linalg;

      import org.wpilib.math.util.*;

      public class Matrix<R extends Num, C extends Num> {
        public Matrix(Nat<R> rows, Nat<C> cols) {}
        public Matrix(Nat<R> rows, Nat<C> cols, double[] storage) {}
      }
      """);

  private static final JavaFileObject MAT_BUILDER =
      JavaFileObjects.forSourceString(
          "org.wpilib.math.linalg.MatBuilder",
          """
      package org.wpilib.math.linalg;

      import org.wpilib.math.util.*;

      public final class MatBuilder {
        public static <R extends Num, C extends Num> Matrix<R, C> fill(
          Nat<R> rows, Nat<C> cols, double... data
        ) {
          return null;
        }
      }
      """);

  @Test
  void matBuilderFillValid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = MatBuilder.fill(Nat.N3(), Nat.N2(), 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NUM,
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void matBuilderFillInvalid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = MatBuilder.fill(Nat.N3(), Nat.N2(), 1.0, 2.0, 3.0, 4.0);
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    System.out.println(errors);
    assertEquals(1, errors.size());
    assertEquals(
        "Invalid matrix data provided. Wanted 3 x 2 matrix, but got 4 elements",
        errors.get(0).getMessage(null));
  }

  @Test
  void matrixConstructorValid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = new Matrix(
            Nat.N3(), Nat.N2(), new double[] { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 }
          );
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void matrixConstructorInvalid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = new Matrix(Nat.N3(), Nat.N2(), new double[] { 1.0, 2.0 });
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    assertEquals(
        "Invalid matrix data provided. Wanted 3 x 2 matrix, but got 2 elements",
        errors.get(0).getMessage(null));
  }

  @Test
  void matBuilderFillWithArrayValid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = MatBuilder.fill(
            Nat.N3(), Nat.N2(), new double[] { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 }
          );
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void matBuilderFillWithArrayInvalid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = MatBuilder.fill(
            Nat.N3(), Nat.N2(), new double[] { 1.0, 2.0, 3.0, 4.0 }
          );
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    assertEquals(
        "Invalid matrix data provided. Wanted 3 x 2 matrix, but got 4 elements",
        errors.get(0).getMessage(null));
  }

  @Test
  void matBuilderWithNoElementsInvalid() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          Matrix<N3, N2> mat = MatBuilder.fill(Nat.N3(), Nat.N2());
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    assertEquals(
        "Invalid matrix data provided. Wanted 3 x 2 matrix, but got 0 elements",
        errors.get(0).getMessage(null));
  }

  @Test
  void dynamicExpressionIgnored() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.math.linalg.MatBuilder;
        import org.wpilib.math.util.Nat;
        import org.wpilib.math.linalg.Matrix;
        import org.wpilib.math.numbers.*;

        class Example {
          double[] getDoubleArray() { return new double[10]; }
          Matrix<N3, N2> mat1 = new Matrix(Nat.N3(), Nat.N2(), getDoubleArray());
          Matrix<N3, N2> mat2 = MatBuilder.fill(Nat.N3(), Nat.N2(), getDoubleArray());
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(
                NAT,
                N3,
                N2,
                MATRIX,
                MAT_BUILDER,
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }
}
