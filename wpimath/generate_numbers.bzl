__NUM_NUMS = 20

def _java_nat_file_gen_impl(ctx):
    nat_out = ctx.actions.declare_file("src/main/edu/wpi/first/math/Nat.java")

    content = "package edu.wpi.first.math;\n"
    for i in range(__NUM_NUMS + 1):
        content += "import edu.wpi.first.math.numbers.N" + str(i) + ";\n"

    content += """
//CHECKSTYLE.ON

/**
 * A natural number expressed as a java class.
 * The counterpart to {@link Num} that should be used as a concrete value.
 *
 * @param <T> The {@link Num} this represents.
 */
@SuppressWarnings({"MethodName", "unused"})
public interface Nat<T extends Num> {
  /**
   * The number this interface represents.
   *
   * @return The number backing this value.
   */
  int getNum();

  static Nat<N0> N0() {
    return N0.instance;
  }

  static Nat<N1> N1() {
    return N1.instance;
  }

  static Nat<N2> N2() {
    return N2.instance;
  }

  static Nat<N3> N3() {
    return N3.instance;
  }

  static Nat<N4> N4() {
    return N4.instance;
  }

  static Nat<N5> N5() {
    return N5.instance;
  }

  static Nat<N6> N6() {
    return N6.instance;
  }

  static Nat<N7> N7() {
    return N7.instance;
  }

  static Nat<N8> N8() {
    return N8.instance;
  }

  static Nat<N9> N9() {
    return N9.instance;
  }

  static Nat<N10> N10() {
    return N10.instance;
  }

  static Nat<N11> N11() {
    return N11.instance;
  }

  static Nat<N12> N12() {
    return N12.instance;
  }

  static Nat<N13> N13() {
    return N13.instance;
  }

  static Nat<N14> N14() {
    return N14.instance;
  }

  static Nat<N15> N15() {
    return N15.instance;
  }

  static Nat<N16> N16() {
    return N16.instance;
  }

  static Nat<N17> N17() {
    return N17.instance;
  }

  static Nat<N18> N18() {
    return N18.instance;
  }

  static Nat<N19> N19() {
    return N19.instance;
  }

  static Nat<N20> N20() {
    return N20.instance;
  }

}
"""

    ctx.actions.write(
        output = nat_out,
        content = content,
    )

    out_files = [nat_out]

    for i in range(__NUM_NUMS + 1):
        file_out = ctx.actions.declare_file("src/main/edu/wpi/first/math/numbers/N" + str(i) + ".java")

        content = """/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.math.numbers;

import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;

/**
 * A class representing the number {i}.
*/
public final class N{i} extends Num implements Nat<N{i}> {{
  private N{i}() {{
  }}

  /**
   * The integer this class represents.
   *
   * @return The literal number {i}.
  */
  @Override
  public int getNum() {{
    return {i};
  }}

  /**
   * The singleton instance of this class.
  */
  public static final N{i} instance = new N{i}();
}}
""".format(i = i)

        ctx.actions.write(
            output = file_out,
            content = content,
        )
        out_files.append(file_out)

    return [DefaultInfo(files = depset(out_files))]

wpimath_java_nat_file_gen = rule(
    implementation = _java_nat_file_gen_impl,
)
