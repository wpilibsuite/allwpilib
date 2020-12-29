// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.Test;

/** Test for the DigitalGlitchFilter class. */
public class DigitalGlitchFilterTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(DigitalGlitchFilterTest.class.getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * This is a test to make sure that filters can be created and are consistent. This assumes that
   * the FPGA implementation to actually implement the filter has been tested. It does validate that
   * we are successfully able to set and get the active filters for ports and makes sure that the
   * FPGA filter is changed correctly, and does the same for the period.
   */
  @Test
  public void testDigitalGlitchFilterBasic() {
    final DigitalInput input1 = new DigitalInput(1);
    final DigitalInput input2 = new DigitalInput(2);
    final DigitalInput input3 = new DigitalInput(3);
    final DigitalInput input4 = new DigitalInput(4);
    final Encoder encoder5 = new Encoder(5, 6);
    final Counter counter7 = new Counter(7);

    final DigitalGlitchFilter filter1 = new DigitalGlitchFilter();
    filter1.add(input1);
    filter1.setPeriodNanoSeconds(4200);

    final DigitalGlitchFilter filter2 = new DigitalGlitchFilter();
    filter2.add(input2);
    filter2.add(input3);
    filter2.setPeriodNanoSeconds(97100);

    final DigitalGlitchFilter filter3 = new DigitalGlitchFilter();
    filter3.add(input4);
    filter3.add(encoder5);
    filter3.add(counter7);
    filter3.setPeriodNanoSeconds(167800);

    assertEquals(4200, filter1.getPeriodNanoSeconds());
    assertEquals(97100, filter2.getPeriodNanoSeconds());
    assertEquals(167800, filter3.getPeriodNanoSeconds());

    filter1.remove(input1);

    filter2.remove(input2);
    filter2.remove(input3);

    filter3.remove(input4);
    filter3.remove(encoder5);
    filter3.remove(counter7);

    input1.close();
    input2.close();
    input3.close();
    input4.close();
    encoder5.close();
    counter7.close();
    filter1.close();
    filter2.close();
    filter3.close();
  }
}
