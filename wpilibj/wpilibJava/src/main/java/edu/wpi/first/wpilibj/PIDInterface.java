package edu.wpi.first.wpilibj;

public interface PIDInterface extends Controller {

    public void setPID(double p, double i, double d);
    public double getP();
    public double getI();
    public double getD();
    public void setSetpoint(double setpoint);
    public double getSetpoint();
    public double getError();
    public void enable();
    public void disable();
    public boolean isEnabled();
    public void reset();
}
