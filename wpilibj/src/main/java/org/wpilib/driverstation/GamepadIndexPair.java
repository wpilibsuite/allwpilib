package org.wpilib.driverstation;

public interface GamepadIndexPair {
    /** Get the NI DS index. */
    int getNiIndex();

    /** Get the SDL DS index. */
    int getSdlIndex();
}
