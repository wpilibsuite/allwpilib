/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/TimedRobot.h>

#include <hal/DutyCycle.h>
#include <hal/DMA.h>
#include <hal/DIO.h>
#include <hal/HALBase.h>
#include <hal/AnalogAccumulator.h>
#include <hal/AnalogOutput.h>
#include <hal/AnalogInput.h>
#include <hal/AnalogGyro.h>
#include <frc/smartdashboard/SmartDashboard.h>

class MyRobot : public frc::TimedRobot {
  HAL_DutyCycleHandle dutyCycle;
  HAL_DigitalHandle dioHandle;
  HAL_DigitalPWMHandle pwmHandle;
  HAL_DigitalHandle interruptDIO;

  HAL_AnalogInputHandle aiHandle;
  HAL_AnalogOutputHandle aoHandle;

  HAL_DMAHandle dma;
  HAL_GyroHandle gyro;


  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    int32_t status = 0;
    dioHandle = HAL_InitializeDIOPort(HAL_GetPort(0), false, &status);

    std::cout << "init dio " << status << std::endl;

    pwmHandle = HAL_AllocateDigitalPWM(&status);
    std::cout << "init pwm " << status << std::endl;
    HAL_SetDigitalPWMRate(250, &status);
    std::cout << "pwm rate " << status << std::endl;
    HAL_SetDigitalPWMDutyCycle(pwmHandle, .5, &status);
    std::cout << "pwm duty cycle " << status << std::endl;
    HAL_SetDigitalPWMOutputChannel(pwmHandle, 0, &status);
    std::cout << "pwm channel " << status << std::endl;


    aiHandle = HAL_InitializeAnalogInputPort(HAL_GetPort(0), &status);
    aoHandle = HAL_InitializeAnalogOutputPort(HAL_GetPort(0), &status);

    HAL_SetAnalogOutput(aoHandle, 2.5, &status);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    gyro = HAL_InitializeAnalogGyro(aiHandle, &status);

    dutyCycle = HAL_InitializeDutyCycle(dioHandle, HAL_AnalogTriggerType::HAL_Trigger_kFallingPulse, &status);
    std::cout << "init dutycycle " << status << std::endl;

    interruptDIO = HAL_InitializeDIOPort(HAL_GetPort(1), false, &status);
    std::cout << "init interrupt dio " << status << std::endl;

    HAL_SetDIO(interruptDIO, true, &status);
    std::cout << "set interrupt dio " << status << std::endl;


    dma = HAL_InitializeDMA(&status);
    std::cout << "init dma " << status << std::endl;

    HAL_AddDMADutyCycle(dma, dutyCycle, &status);
    std::cout << "add duty cycle " << status << std::endl;

    HAL_AddDMAAnalogInput(dma, aiHandle, &status);
    HAL_AddDMAAnalogAccumulator(dma, aiHandle, &status);

    HAL_SetDMAExternalTrigger(dma, interruptDIO, HAL_AnalogTriggerType::HAL_Trigger_kFallingPulse, false, true, &status);
    std::cout << "init dma trigger " << status << std::endl;

    HAL_StartDMA(dma, 1024, &status);
    std::cout << "start dma " << status << std::endl;

    frc::SmartDashboard::PutNumber("SetDC", 0.5);
    frc::SmartDashboard::PutNumber("SetFreq", 250);
    frc::SmartDashboard::PutNumber("SetAO", 2.5);
  }

  /**
   * This function is run once each time the robot enters autonomous mode
   */
  void AutonomousInit() override {}

  /**
   * This function is called periodically during autonomous
   */
  void AutonomousPeriodic() override {}

  /**
   * This function is called once each time the robot enters tele-operated mode
   */
  void TeleopInit() override {}

  /**
   * This function is called periodically during operator control
   */
  void TeleopPeriodic() override {}

  /**
   * This function is called periodically during test mode
   */
  void TestPeriodic() override {}

  int count = 0;

  /**
   * This function is called periodically during all modes
   */
  void RobotPeriodic() override {
    int32_t status = 0;

    HAL_SetDIO(interruptDIO, false, &status);

    HAL_DMASample sample;
    int32_t remaining = 0;

    auto dmastatus = HAL_ReadDMA(dma, &sample, 5, &remaining, &status);

    if (dmastatus == HAL_DMAReadStatus::HAL_DMA_OK) {
      double op = HAL_GetDMASampleDutyCycleOutput(&sample, dutyCycle, &status);
      auto rawop = op;
      op = op / (double)HAL_GetDutyCycleOutputScaleFactor(dutyCycle, &status);

      frc::SmartDashboard::PutNumber("DMAOutputRaw", rawop);
      frc::SmartDashboard::PutNumber("DMAOutput", op);
      frc::SmartDashboard::PutNumber("DMAcount", count++);
      frc::SmartDashboard::PutNumber("DMATime", HAL_GetDMASampleTime(&sample, &status));
      wpi::SmallVector<double, 128> dmaValues;
      for (uint32_t i = 0; i < sample.captureSize; i++) {
        dmaValues.emplace_back(sample.readBuffer[i]);
      }

      frc::SmartDashboard::PutNumberArray("DMAVals", dmaValues);

      frc::SmartDashboard::PutNumber("DMAAI", HAL_GetDMASampleAnalogInput(&sample, aiHandle, &status));
      frc::SmartDashboard::PutNumber("DMAAccumCount", HAL_GetDMASampleAnalogAccumulatorCount(&sample, aiHandle, &status));
      frc::SmartDashboard::PutNumber("DMAAccumValue", HAL_GetDMASampleAnalogAccumulatorValue(&sample, aiHandle, &status));
    }

    HAL_SetDIO(interruptDIO, true, &status);

    frc::SmartDashboard::PutNumber("DMAResult", (int)dmastatus);
    frc::SmartDashboard::PutNumber("Remaining", (int)remaining);
    frc::SmartDashboard::PutNumber("Frequency", HAL_GetDutyCycleFrequency(dutyCycle, &status));
    frc::SmartDashboard::PutNumber("Output", HAL_GetDutyCycleOutput(dutyCycle, &status));

    frc::SmartDashboard::PutNumber("AccumCount", HAL_GetAccumulatorCount(aiHandle, &status));
    frc::SmartDashboard::PutNumber("AccumValue", HAL_GetAccumulatorValue(aiHandle, &status));
    frc::SmartDashboard::PutNumber("AI", HAL_GetAnalogValue(aiHandle, &status));



    HAL_SetDigitalPWMDutyCycle(pwmHandle, frc::SmartDashboard::GetNumber("SetDC", 0.5), &status);
    HAL_SetDigitalPWMRate(frc::SmartDashboard::GetNumber("SetFreq", 250), &status);
    HAL_SetAnalogOutput(aoHandle, frc::SmartDashboard::GetNumber("SetAO", 2.5), &status);
  }
};

int main() { return frc::StartRobot<MyRobot>(); }
