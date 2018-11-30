/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.mocks;

import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import edu.wpi.first.wpilibj.DigitalOutput;

import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * Class demonstrating basic mocking of a DigitalOutput not requiring the HAL.
 * It can get and set its state.
 */
public class DigitalOutputMock {
  // State to hold
  private boolean m_state;
  // Mocked DigitalOutput
  private final DigitalOutput m_mockedDigitalOutput;

  /**
   * Constructor to create a digital output mock.
   * 
   * @param startingState Starting state of the DigitalOutput
   */
  public DigitalOutputMock(boolean startingState) {
    // Set up instance variables...one could create another
    // constructor that would take an already created
    // mocked digital output if you wanted to chain these together.
    this.m_mockedDigitalOutput = mock(DigitalOutput.class);
    this.m_state = startingState;

    // Simply answer the state if get is called
    // NOTE: You cannot call thenReturn(m_state)...it will always return the first
    // value, not the currenly set value.
    // You can also use lambdas if you do not like this syntax...
    when(m_mockedDigitalOutput.get()).thenAnswer(new Answer<Boolean>() {
      @Override
      public Boolean answer(InvocationOnMock invocationOnMock) throws Throwable {
        return m_state;
      }
    });

    // Create an answer to set the state when the set method is called on the mock
    // You can also use lambdas if you do not like this syntax...
    doAnswer(new Answer<Void>() {
      @Override
      public Void answer(InvocationOnMock invocationOnMock) throws Throwable {
        m_state = (Boolean) invocationOnMock.getArguments()[0];
        return null;
      }
    }).when(m_mockedDigitalOutput).set(any(boolean.class));
  }

  /**
   * Get the mocked DigitalOutput so that tests can be asserted.
   * 
   * @return  The mocked DigitalOutput
   */
  public DigitalOutput getDigitalOutput() {
    return this.m_mockedDigitalOutput;
  }
}
