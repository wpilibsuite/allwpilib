// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.DMA;
import edu.wpi.first.wpilibj.DMASample;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.VictorSP;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

@SuppressWarnings("all")
public class MyRobot extends TimedRobot {
  private DMA m_dma;
  private DMASample m_dmaSample;

  // DMA needs a trigger, can use an output as trigger.
  // 8 Triggers exist per DMA object, can be triggered on any
  // DigitalSource.
  private DigitalOutput m_dmaTrigger;

  // Analog input to read with DMA
  private AnalogInput m_analogInput;

  // Encoder to read with DMA
  private Encoder m_encoder;

  private VictorSP m_vsp;

  @Override
  public void robotInit() {
    m_dmaTrigger = new DigitalOutput(2);
    m_dmaTrigger.set(true);
    m_analogInput = new AnalogInput(0);
    m_encoder = new Encoder(0, 1);
    m_dmaSample = new DMASample();
    m_dma = new DMA();
    m_vsp = new VictorSP(11);

    m_vsp.set(0);



    // m_dma.setTimedTriggerCycles(1);
    // Trigger on falling edge of dma trigger output
    m_dma.setPwmEdgeTrigger(m_vsp, false, true);
    //m_dma.setExternalTrigger(m_dmaTrigger, false, true);
    // m_dma.setTimedTrigger(0.005);

    // Add inputs we want to read via DMA
    m_dma.addAnalogInput(m_analogInput);
    m_dma.addEncoder(m_encoder);
    m_dma.addEncoderPeriod(m_encoder);

    // Make sure trigger is set to off.

    // Start DMA. No triggers or inputs can be added after this call
    // unless DMA is stopped.
    m_dma.startDMA(1023);
  }

  int count = 0;
  int count2 = 0;
  int count3 = 0;
  double existing = 0;

  @Override
  public void robotPeriodic() {
    // Manually Trigger DMA read
    m_dmaTrigger.set(false);

    do {

      // Update our sample. remaining is the number of samples remaining in the
      // buffer status is more specific error messages if readStatus is not OK.
      // Wait 1ms if buffer is empty
      DMASample.DMAReadStatus readStatus = m_dmaSample.update(m_dma, 0.0001);

      SmartDashboard.putNumber("status", readStatus.getValue());

      count2++;
      SmartDashboard.putNumber("count2", count2);

      if (readStatus == DMASample.DMAReadStatus.Ok) {
        // Status value in all these reads should be checked, a non 0 value means
        // value could not be read

        count++;
        SmartDashboard.putNumber("count", count);

        // If DMA is good, values exist
        double encoderDistance = m_dmaSample.getEncoderDistance(m_encoder);
        // Period is not scaled, and is a raw value
        int encoderPeriod = m_dmaSample.getEncoderPeriodRaw(m_encoder);
        double analogVoltage = m_dmaSample.getAnalogInputVoltage(m_analogInput);

        if (m_dmaSample.getRemaining() != 0) {
          SmartDashboard.putBoolean("HadData", true);
        }

        SmartDashboard.putNumber("CapSize", m_dmaSample.getCaptureSize());
        SmartDashboard.putNumber("Triggers", m_dmaSample.getTriggerChannels());

        SmartDashboard.putNumber("Distance", encoderDistance);
        SmartDashboard.putNumber("Period", encoderPeriod);
        SmartDashboard.putNumber("Input", analogVoltage);
        SmartDashboard.putNumber("Remaining", m_dmaSample.getRemaining());
        SmartDashboard.putNumber("Timestamp", m_dmaSample.getTimeStamp());
        double delta = m_dmaSample.getTimeStamp() - existing;
        existing = m_dmaSample.getTimeStamp();
        SmartDashboard.putNumber("Delta", delta);
        SmartDashboard.putNumber("Timestamp2", Timer.getFPGATimestamp());
      } else {
        count3++;
        SmartDashboard.putNumber("Timeouts", count3);
      }
    } while (m_dmaSample.getRemaining() != 0);

    // Unset trigger
    m_dmaTrigger.set(true);
  }
}
