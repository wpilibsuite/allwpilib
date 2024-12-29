// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.dma;

import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.DMA;
import edu.wpi.first.wpilibj.DMASample;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/** This is a sample program showing how to to use DMA to read a sensor. */
public class Robot extends TimedRobot {
  private final DMA m_dma;
  private final DMASample m_dmaSample;

  // DMA needs a trigger, can use an output as trigger.
  // 8 Triggers exist per DMA object, can be triggered on any
  // DigitalSource.
  private final DigitalOutput m_dmaTrigger;

  // Analog input to read with DMA
  private final AnalogInput m_analogInput;

  // Encoder to read with DMA
  private final Encoder m_encoder;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    m_dma = new DMA();
    m_dmaSample = new DMASample();
    m_dmaTrigger = new DigitalOutput(2);
    m_analogInput = new AnalogInput(0);
    m_encoder = new Encoder(0, 1);

    // Trigger on falling edge of dma trigger output
    m_dma.setExternalTrigger(m_dmaTrigger, false, true);

    // Add inputs we want to read via DMA
    m_dma.addAnalogInput(m_analogInput);
    m_dma.addEncoder(m_encoder);
    m_dma.addEncoderPeriod(m_encoder);

    // Make sure trigger is set to off.
    m_dmaTrigger.set(true);

    // Start DMA. No triggers or inputs can be added after this call
    // unless DMA is stopped.
    m_dma.start(1024);
  }

  @Override
  public void robotPeriodic() {
    // Manually Trigger DMA read
    m_dmaTrigger.set(false);

    // Update our sample. remaining is the number of samples remaining in the
    // buffer status is more specific error messages if readStatus is not OK.
    // Wait 1ms if buffer is empty
    DMASample.DMAReadStatus readStatus = m_dmaSample.update(m_dma, Units.millisecondsToSeconds(1));

    // Unset trigger
    m_dmaTrigger.set(true);

    if (readStatus == DMASample.DMAReadStatus.kOk) {
      // Status value in all these reads should be checked, a non 0 value means
      // value could not be read

      // If DMA is good, values exist
      double encoderDistance = m_dmaSample.getEncoderDistance(m_encoder);
      // Period is not scaled, and is a raw value
      int encoderPeriod = m_dmaSample.getEncoderPeriodRaw(m_encoder);
      double analogVoltage = m_dmaSample.getAnalogInputVoltage(m_analogInput);

      SmartDashboard.putNumber("Distance", encoderDistance);
      SmartDashboard.putNumber("Period", encoderPeriod);
      SmartDashboard.putNumber("Input", analogVoltage);
      SmartDashboard.putNumber("Timestamp", m_dmaSample.getTimeStamp());
    }
  }
}
