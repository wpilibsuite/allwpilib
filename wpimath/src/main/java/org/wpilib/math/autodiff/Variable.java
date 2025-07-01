// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

/** An autodiff variable pointing to an expression node. */
public class Variable implements AutoCloseable {
  /** Handle type tag. */
  public static final class Handle {
    /** Constructor for Handle. */
    public Handle() {}
  }

  /** Instance of handle type tag. */
  public static final Handle HANDLE = new Handle();

  private long m_handle;

  /** Constructs a linear Variable with a value of zero. */
  @SuppressWarnings("this-escape")
  public Variable() {
    m_handle = VariableJNI.createDefault();
    VariablePool.register(this);
  }

  /**
   * Constructs a Variable from a floating point type.
   *
   * @param value The value of the Variable.
   */
  @SuppressWarnings("this-escape")
  public Variable(double value) {
    m_handle = VariableJNI.createDouble(value);
    VariablePool.register(this);
  }

  /**
   * Constructs a Variable from an integral type.
   *
   * @param value The value of the Variable.
   */
  @SuppressWarnings("this-escape")
  public Variable(int value) {
    m_handle = VariableJNI.createInt(value);
    VariablePool.register(this);
  }

  /**
   * Constructs a Variable from the given handle.
   *
   * <p>This constructor is for internal use only.
   *
   * @param handleTypeTag Handle type tag.
   * @param handle Variable handle.
   */
  @SuppressWarnings({"PMD.UnusedFormalParameter", "this-escape"})
  public Variable(Handle handleTypeTag, long handle) {
    m_handle = handle;
    VariablePool.register(this);
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      VariableJNI.destroy(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Sets Variable's internal value.
   *
   * @param value The value of the Variable.
   */
  public void setValue(double value) {
    VariableJNI.setValue(m_handle, value);
  }

  /**
   * Returns the value of this variable.
   *
   * @return The value of this variable.
   */
  public double value() {
    return VariableJNI.value(m_handle);
  }

  /**
   * Returns the type of this expression (constant, linear, quadratic, or nonlinear).
   *
   * @return The type of this expression.
   */
  public ExpressionType type() {
    return ExpressionType.fromInt(VariableJNI.type(m_handle));
  }

  /**
   * Returns internal handle.
   *
   * @return Internal handle.
   */
  public long getHandle() {
    return m_handle;
  }

  /**
   * Variable-Variable multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  public Variable times(Variable rhs) {
    return new Variable(HANDLE, VariableJNI.times(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  public Variable times(double rhs) {
    return times(new Variable(rhs));
  }

  /**
   * Variable-Variable division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  public Variable div(Variable rhs) {
    return new Variable(HANDLE, VariableJNI.div(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  public Variable div(double rhs) {
    return div(new Variable(rhs));
  }

  /**
   * Variable-Variable addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public Variable plus(Variable rhs) {
    return new Variable(HANDLE, VariableJNI.plus(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public Variable plus(double rhs) {
    return plus(new Variable(rhs));
  }

  /**
   * Variable-Variable subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public Variable minus(Variable rhs) {
    return new Variable(HANDLE, VariableJNI.minus(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public Variable minus(double rhs) {
    return minus(new Variable(rhs));
  }

  /**
   * Unary minus operator.
   *
   * @return Result of unary minus.
   */
  public Variable unaryMinus() {
    return new Variable(HANDLE, VariableJNI.unaryMinus(m_handle));
  }

  /**
   * Unary plus operator.
   *
   * @return Result of unary plus.
   */
  public Variable unaryPlus() {
    return this;
  }

  /**
   * Math.abs() for Variables.
   *
   * @param x The argument.
   * @return Result of abs().
   */
  public static Variable abs(Variable x) {
    return new Variable(HANDLE, VariableJNI.abs(x.getHandle()));
  }

  /**
   * Math.acos() for Variables.
   *
   * @param x The argument.
   * @return Result of acos().
   */
  public static Variable acos(Variable x) {
    return new Variable(HANDLE, VariableJNI.acos(x.getHandle()));
  }

  /**
   * Math.asin() for Variables.
   *
   * @param x The argument.
   * @return Result of asin().
   */
  public static Variable asin(Variable x) {
    return new Variable(HANDLE, VariableJNI.asin(x.getHandle()));
  }

  /**
   * Math.atan() for Variables.
   *
   * @param x The argument.
   * @return Result of atan().
   */
  public static Variable atan(Variable x) {
    return new Variable(HANDLE, VariableJNI.atan(x.getHandle()));
  }

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   * @return Result of atan2().
   */
  public static Variable atan2(double y, Variable x) {
    return atan2(new Variable(y), x);
  }

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   * @return Result of atan2().
   */
  public static Variable atan2(Variable y, double x) {
    return atan2(y, new Variable(x));
  }

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   * @return Result of atan2().
   */
  public static Variable atan2(Variable y, Variable x) {
    return new Variable(HANDLE, VariableJNI.atan2(y.getHandle(), x.getHandle()));
  }

  /**
   * Math.cbrt() for Variables.
   *
   * @param x The argument.
   * @return Result of cbrt().
   */
  public static Variable cbrt(Variable x) {
    return new Variable(HANDLE, VariableJNI.cbrt(x.getHandle()));
  }

  /**
   * Math.cos() for Variables.
   *
   * @param x The argument.
   * @return Result of cos().
   */
  public static Variable cos(Variable x) {
    return new Variable(HANDLE, VariableJNI.cos(x.getHandle()));
  }

  /**
   * Math.cosh() for Variables.
   *
   * @param x The argument.
   * @return Result of cosh().
   */
  public static Variable cosh(Variable x) {
    return new Variable(HANDLE, VariableJNI.cosh(x.getHandle()));
  }

  /**
   * Math.exp() for Variables.
   *
   * @param x The argument.
   * @return Result of exp().
   */
  public static Variable exp(Variable x) {
    return new Variable(HANDLE, VariableJNI.exp(x.getHandle()));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, Variable y) {
    return hypot(new Variable(x), y);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, double y) {
    return hypot(x, new Variable(y));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, Variable y) {
    return new Variable(HANDLE, VariableJNI.hypot(x.getHandle(), y.getHandle()));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, double y, Variable z) {
    return hypot(new Variable(x), new Variable(y), z);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, Variable y, double z) {
    return hypot(new Variable(x), y, new Variable(z));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, Variable y, Variable z) {
    return hypot(new Variable(x), y, z);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, double y, double z) {
    return hypot(x, new Variable(y), new Variable(z));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, double y, Variable z) {
    return hypot(x, new Variable(y), z);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, Variable y, double z) {
    return hypot(x, y, new Variable(z));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, Variable y, Variable z) {
    return sqrt(pow(x, 2).plus(pow(y, 2)).plus(pow(z, 2)));
  }

  /**
   * Math.log() for Variables.
   *
   * @param x The argument.
   * @return Result of log().
   */
  public static Variable log(Variable x) {
    return new Variable(HANDLE, VariableJNI.log(x.getHandle()));
  }

  /**
   * Math.log10() for Variables.
   *
   * @param x The argument.
   * @return Result of log10().
   */
  public static Variable log10(Variable x) {
    return new Variable(HANDLE, VariableJNI.log10(x.getHandle()));
  }

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   * @return Result of pow().
   */
  public static Variable pow(double base, Variable power) {
    return pow(new Variable(base), power);
  }

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   * @return Result of pow().
   */
  public static Variable pow(Variable base, double power) {
    return pow(base, new Variable(power));
  }

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   * @return Result of pow().
   */
  public static Variable pow(Variable base, Variable power) {
    return new Variable(HANDLE, VariableJNI.pow(base.getHandle(), power.getHandle()));
  }

  /**
   * Math.signum() for Variables.
   *
   * @param x The argument.
   * @return Result of signum().
   */
  public static Variable signum(Variable x) {
    return new Variable(HANDLE, VariableJNI.signum(x.getHandle()));
  }

  /**
   * Math.sin() for Variables.
   *
   * @param x The argument.
   * @return Result of sin().
   */
  public static Variable sin(Variable x) {
    return new Variable(HANDLE, VariableJNI.sin(x.getHandle()));
  }

  /**
   * Math.sinh() for Variables.
   *
   * @param x The argument.
   * @return Result of sinh().
   */
  public static Variable sinh(Variable x) {
    return new Variable(HANDLE, VariableJNI.sinh(x.getHandle()));
  }

  /**
   * Math.sqrt() for Variables.
   *
   * @param x The argument.
   * @return Result of sqrt().
   */
  public static Variable sqrt(Variable x) {
    return new Variable(HANDLE, VariableJNI.sqrt(x.getHandle()));
  }

  /**
   * Math.tan() for Variables.
   *
   * @param x The argument.
   * @return Result of tan().
   */
  public static Variable tan(Variable x) {
    return new Variable(HANDLE, VariableJNI.tan(x.getHandle()));
  }

  /**
   * Math.tanh() for Variables.
   *
   * @param x The argument.
   * @return Result of tanh().
   */
  public static Variable tanh(Variable x) {
    return new Variable(HANDLE, VariableJNI.tanh(x.getHandle()));
  }

  /**
   * Returns the total native memory usage of all Variables in bytes.
   *
   * @return The total native memory usage of all Variables in bytes.
   */
  public static long totalNativeMemoryUsage() {
    return VariableJNI.totalNativeMemoryUsage();
  }
}
