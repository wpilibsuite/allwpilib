/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.InterruptJNI.InterruptJNIHandlerFunction;


/**
 * It is recommended that you use this class in conjunction with classes from
 * {@link java.util.concurrent.atomic} as these objects are all thread safe.
 *
 * @author Jonathan Leitschuh
 *
 * @param <T> The type of the parameter that should be returned to the the
 *        method {@link #interruptFired(int, Object)}
 */
public abstract class InterruptHandlerFunction<T> {
  /**
   * The entry point for the interrupt. When the interrupt fires the
   * {@link #apply(int, Object)} method is called. The outer class is provided
   * as an interface to allow the implementer to pass a generic object to the
   * interrupt fired method.
   *$
   * @author Jonathan Leitschuh
   */
  private class Function implements InterruptJNIHandlerFunction {
    @SuppressWarnings("unchecked")
    @Override
    public void apply(int interruptAssertedMask, Object param) {
      interruptFired(interruptAssertedMask, (T) param);
    }
  }

  final Function function = new Function();

  /**
   * This method is run every time an interrupt is fired.
   *$
   * @param interruptAssertedMask Interrupt Mask
   * @param param The parameter provided by overriding the
   *        {@link #overridableParameter()} method.
   */
  public abstract void interruptFired(int interruptAssertedMask, T param);


  /**
   * Override this method if you would like to pass a specific parameter to the
   * {@link #interruptFired(int, Object)} when it is fired by the interrupt.
   * This method is called once when
   * {@link InterruptableSensorBase#requestInterrupts(InterruptHandlerFunction)}
   * is run.
   *$
   * @return The object that should be passed to the interrupt when it runs
   */
  public T overridableParameter() {
    return null;
  }
}
