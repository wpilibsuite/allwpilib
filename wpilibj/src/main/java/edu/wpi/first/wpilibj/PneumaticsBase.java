package edu.wpi.first.wpilibj;

public interface PneumaticsBase extends AutoCloseable {
    void setSolenoids(int mask, int values);
    int getSolenoids();
    int getModuleNumber();
}