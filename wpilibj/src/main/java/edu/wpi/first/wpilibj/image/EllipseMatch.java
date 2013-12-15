/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

/**
 *
 * @author dtjones
 */
public class EllipseMatch {

    static final int ellipseMatchSize = 40;
    public double m_xPos;
    public double m_yPos;
    public double m_rotation;
    public double m_majorRadius;
    public double m_minorRadius;
    public double m_score;

    private class EllipseMatchStructure extends Structure {

        public EllipseMatchStructure(int address) {
            useMemory(new Pointer(address, ellipseMatchSize));
            read();
        }

        public void read() {
            m_xPos = backingNativeMemory.getFloat(0);
            m_yPos = backingNativeMemory.getFloat(4);
            m_rotation = backingNativeMemory.getDouble(8);
            m_majorRadius = backingNativeMemory.getDouble(16);
            m_minorRadius = backingNativeMemory.getDouble(24);
            m_score = backingNativeMemory.getDouble(32);

        }

        public void write() {
        }

        public int size() {
            return ellipseMatchSize;
        }
    }

    EllipseMatch(int address) {
        new EllipseMatchStructure(address);
    }

    public String toString() {
        return "Ellipse Match:\n" +
               "  Pos x: " + m_xPos + "  y: " + m_yPos + "\n" +
               "  Radius major: " + m_majorRadius + " minor: " + m_minorRadius + "\n" +
               "  Rotation: " + m_rotation + " Score: " + m_score + "\n";
    }

    protected static EllipseMatch[] getMatchesFromMemory(int address, int number) {
        if (address == 0) {
            return new EllipseMatch[0];
        }

        EllipseMatch[] toReturn = new EllipseMatch[number];
        for (int i = 0; i < number; i++) {
            toReturn[i] = new EllipseMatch(address + i * ellipseMatchSize);
        }
        return toReturn;
    }
}
