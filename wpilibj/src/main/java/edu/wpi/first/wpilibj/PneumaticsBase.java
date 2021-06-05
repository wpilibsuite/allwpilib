package edu.wpi.first.wpilibj;

public interface PneumaticsBase extends AutoCloseable {

    /**
     * Sets solenoids on a pneumatics module.
     *
     * @param mask mask
     * @param values values
     */
    void setSolenoids(int mask, int values);

    /**
     * Gets solenoid values.
     *
     * @return values
     */
    int getSolenoids();

    /**
     * Get module number for this module.
     *
     * @return module number
     */
    int getModuleNumber();
}
