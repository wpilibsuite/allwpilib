#include "Wrist.h"
#include "SmartDashboard/SmartDashboard.h"
#include "LiveWindow/LiveWindow.h"

Wrist::Wrist() : PIDSubsystem("Wrist", kP_real, 0.0, 0.0) {
	#ifdef SIMULATION // Check for simulation and update PID values
        GetPIDController()->SetPID(kP_simulation, 0, 0, 0);
	#endif
    SetAbsoluteTolerance(2.5);

    motor = new Victor(6);

    // Conversion value of potentiometer varies between the real world and simulation
	#ifdef REAL
        pot = new AnalogPotentiometer(3, -270.0/5);
	#else
        pot = new AnalogPotentiometer(3); // Defaults to degrees
	#endif

	// Let's show everything on the LiveWindow
    // TODO: LiveWindow::GetInstance()->AddActuator("Wrist", "Motor", (Victor) motor);
    // TODO: LiveWindow::GetInstance()->AddSensor("Wrist", "Pot", (AnalogPotentiometer) pot);
    LiveWindow::GetInstance()->AddActuator("Wrist", "PID", GetPIDController());
}

void Wrist::Log() {
    // TODO: SmartDashboard::PutData("Wrist Angle", (AnalogPotentiometer) pot);
}

double Wrist::ReturnPIDInput() {
    return pot->Get();
}

void Wrist::UsePIDOutput(double d) {
    motor->Set(d);
}
