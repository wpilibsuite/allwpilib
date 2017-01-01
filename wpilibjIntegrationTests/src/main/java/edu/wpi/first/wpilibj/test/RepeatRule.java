/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.test;

import org.junit.rules.TestRule;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * This JUnit Rule allows you to apply this rule to any test to allow it to run multiple times. This
 * is important if you have a test that fails only "sometimes" and needs to be rerun to find the
 * issue.
 *
 * <p>This code was originally found here:
 * <a href="http://www.codeaffine.com/2013/04/10/running-junit-tests-repeatedly-without-loops/">
 * Running JUnit Tests Repeatedly Without Loops</a>
 */
public class RepeatRule implements TestRule {
  @Retention(RetentionPolicy.RUNTIME)
  @Target({java.lang.annotation.ElementType.METHOD})
  public @interface Repeat {
    /**
     * The number of times to repeat the test.
     */
    int times();
  }


  private static class RepeatStatement extends Statement {

    private final int m_times;
    private final Statement m_statement;

    private RepeatStatement(int times, Statement statement) {
      m_times = times;
      m_statement = statement;
    }

    @Override
    public void evaluate() throws Throwable {
      for (int i = 0; i < m_times; i++) {
        m_statement.evaluate();
      }
    }
  }

  @Override
  public Statement apply(Statement statement, Description description) {
    Statement result = statement;
    Repeat repeat = description.getAnnotation(Repeat.class);
    if (repeat != null) {
      int times = repeat.times();
      result = new RepeatStatement(times, statement);
    }
    return result;
  }
}
