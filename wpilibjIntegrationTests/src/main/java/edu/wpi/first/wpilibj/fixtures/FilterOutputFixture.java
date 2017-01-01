/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.lang.reflect.ParameterizedType;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.PIDSourceType;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Represents a filterphysically connected Motor and Encoder to allow for unit tests on these
 * different pairs<br> Designed to allow the user to easily setup and tear down the fixture to allow
 * for reuse. This class should be explicitly instantiated in the TestBed class to allow any test to
 * access this fixture. This allows tests to be mailable so that you can easily reconfigure the
 * physical testbed without breaking the tests.
 */
public abstract class FilterOutputFixture<T extends PIDSource> implements ITestFixture {
  private static final Logger logger = Logger.getLogger(FilterOutputFixture.class.getName());
  private boolean m_initialized = false;
  private boolean m_tornDown = false;
  protected T m_filter;
  private DataWrapper m_data;
  private double m_expectedOutput;

  public FilterOutputFixture(double expectedOutput) {
    m_expectedOutput = expectedOutput;
  }

  /**
   * Get expected output of fixture.
   */
  public double getExpectedOutput() {
    return m_expectedOutput;
  }

  /**
   * Where the implementer of this class should pass the filter constructor.
   */
  protected abstract T giveFilter(PIDSource source);

  private void initialize() {
    synchronized (this) {
      if (!m_initialized) {
        m_initialized = true; // This ensures it is only initialized once

        m_data = new DataWrapper() {
          @Override
          @SuppressWarnings("ParameterName")
          public double getData(double t) {
            return 100.0 * Math.sin(2.0 * Math.PI * t) + 20.0 * Math.cos(50.0 * Math.PI * t);
          }
        };
        m_filter = giveFilter(m_data);
      }
    }
  }

  @Override
  public boolean setup() {
    initialize();
    return true;
  }

  /**
   * Gets the filter for this Object.
   *
   * @return the filter this object refers too
   */
  public T getFilter() {
    initialize();
    return m_filter;
  }

  /**
   * Gets the data wrapper for this object.
   *
   * @return the data wrapper  that this object refers too
   */
  public DataWrapper getDataWrapper() {
    initialize();
    return m_data;
  }

  /**
   * Retrieves the name of the filter that this object refers to.
   *
   * @return The simple name of the filter {@link Class#getSimpleName()}
   */
  public String getType() {
    initialize();
    return m_filter.getClass().getSimpleName();
  }

  @Override
  public boolean reset() {
    m_data.reset();
    return true;
  }

  @Override
  public boolean teardown() {
    return true;
  }

  @Override
  public String toString() {
    StringBuilder string = new StringBuilder("FilterOutputFixture<");
    // Get the generic type as a class
    @SuppressWarnings("unchecked")
    Class<T> class1 =
        (Class<T>) ((ParameterizedType) getClass().getGenericSuperclass())
            .getActualTypeArguments()[0];
    string.append(class1.getSimpleName());
    string.append(">");
    return string.toString();
  }

  public abstract class DataWrapper implements PIDSource {
    // Make sure first call to pidGet() uses count == 0
    private double m_count = -TestBench.kFilterStep;

    @SuppressWarnings("ParameterName")
    public abstract double getData(double t);


    @Override
    public void setPIDSourceType(PIDSourceType pidSource) {
    }


    @Override
    public PIDSourceType getPIDSourceType() {
      return PIDSourceType.kDisplacement;
    }


    @Override
    public double pidGet() {
      m_count += TestBench.kFilterStep;
      return getData(m_count);
    }

    public void reset() {
      m_count = -TestBench.kFilterStep;
    }
  }
}
