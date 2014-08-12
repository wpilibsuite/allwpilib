#include "DriveTrain.h"
#include "Commands/TankDriveWithJoystick.h"

DriveTrain::DriveTrain() : Subsystem("DriveTrain") {
	front_left_motor = new Talon(1);
	back_left_motor = new Talon(2);
	front_right_motor = new Talon(3);
	back_right_motor = new Talon(4);
	drive = new RobotDrive(front_left_motor, back_left_motor,
						   front_right_motor, back_right_motor);

	left_encoder = new Encoder(1, 2);
	right_encoder = new Encoder(3, 4);

	// Encoders may measure differently in the real world and in
	// simulation. In this example the robot moves 0.042 barleycorns
	// per tick in the real world, but the simulated encoders
	// simulate 360 tick encoders. This if statement allows for the
	// real robot to handle this difference in devices.
	#ifdef REAL
		left_encoder->SetDistancePerPulse(0.042);
		right_encoder->SetDistancePerPulse(0.042);
	#else
		// Circumference in ft = 4in/12(in/ft)*PI
		left_encoder->SetDistancePerPulse((double) (4.0/12.0*M_PI) / 360.0);
		right_encoder->SetDistancePerPulse((double) (4.0/12.0*M_PI) / 360.0);
	#endif

	rangefinder = new AnalogInput(6);
	gyro = new Gyro(1);

	// Let's show everything on the LiveWindow
	// TODO: LiveWindow::GetInstance()->AddActuator("Drive Train", "Front_Left Motor", (Talon) front_left_motor);
	// TODO: LiveWindow::GetInstance()->AddActuator("Drive Train", "Back Left Motor", (Talon) back_left_motor);
	// TODO: LiveWindow::GetInstance()->AddActuator("Drive Train", "Front Right Motor", (Talon) front_right_motor);
	// TODO: LiveWindow::GetInstance()->AddActuator("Drive Train", "Back Right Motor", (Talon) back_right_motor);
	LiveWindow::GetInstance()->AddSensor("Drive Train", "Left Encoder", left_encoder);
	LiveWindow::GetInstance()->AddSensor("Drive Train", "Right Encoder", right_encoder);
	LiveWindow::GetInstance()->AddSensor("Drive Train", "Rangefinder", rangefinder);
	LiveWindow::GetInstance()->AddSensor("Drive Train", "Gyro", gyro);
}

/**
 * When no other command is running let the operator drive around
 * using the PS3 joystick.
 */
void DriveTrain::InitDefaultCommand() {
	SetDefaultCommand(new TankDriveWithJoystick());
}

/**
 * The log method puts interesting information to the SmartDashboard.
 */
void DriveTrain::Log() {
	SmartDashboard::PutNumber("Left Distance", left_encoder->GetDistance());
	SmartDashboard::PutNumber("Right Distance", right_encoder->GetDistance());
	SmartDashboard::PutNumber("Left Speed", left_encoder->GetRate());
	SmartDashboard::PutNumber("Right Speed", right_encoder->GetRate());
	SmartDashboard::PutNumber("Gyro", gyro->GetAngle());
}

void DriveTrain::Drive(double left, double right) {
	drive->TankDrive(left, right);
}

void DriveTrain::Drive(Joystick* joy) {
	Drive(-joy->GetY(), -joy->GetRawAxis(4));
}

double DriveTrain::GetHeading() {
	return gyro->GetAngle();
}

void DriveTrain::Reset() {
	gyro->Reset();
	left_encoder->Reset();
	right_encoder->Reset();
}

double DriveTrain::GetDistance() {
	return (left_encoder->GetDistance() + right_encoder->GetDistance())/2;
}

double DriveTrain::GetDistanceToObstacle() {
	// Really meters in simulation since it's a rangefinder...
	return rangefinder->GetAverageVoltage();
}
