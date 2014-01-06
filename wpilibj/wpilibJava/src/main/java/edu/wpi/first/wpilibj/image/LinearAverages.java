/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.image;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

/**
 *
 * @author koconnor
 */
public class LinearAverages {

    Pointer columnAveragesPtr;
    int columnCount;
    float[] columnAverages;
    Pointer rowAveragesPtr;
    int rowCount;
    float[] rowAverages;
    Pointer risingDiagAveragesPtr;
    int risingDiagCount;
    float[] risingDiagAverages;
    Pointer fallingDiagAveragesPtr;
    int fallingDiagCount;
    float[] fallingDiagAverages;

    public static class LinearAveragesMode {

        public final int value;
        static final int IMAQ_COLUMN_AVERAGES_val = 1;
        static final int IMAQ_ROW_AVERAGES_val = 2;
        static final int IMAQ_RISING_DIAG_AVERAGES_val = 4;
        static final int IMAQ_FALLING_DIAG_AVERAGES_val = 8;
        static final int IMAQ_ALL_LINEAR_AVERAGES_val = 15;
        public static final LinearAveragesMode IMAQ_COLUMN_AVERAGES = new LinearAveragesMode(IMAQ_COLUMN_AVERAGES_val);
        public static final LinearAveragesMode IMAQ_ROW_AVERAGES = new LinearAveragesMode(IMAQ_ROW_AVERAGES_val);
        public static final LinearAveragesMode IMAQ_RISING_DIAG_AVERAGES = new LinearAveragesMode(IMAQ_RISING_DIAG_AVERAGES_val);
        public static final LinearAveragesMode IMAQ_FALLING_DIAG_AVERAGES = new LinearAveragesMode(IMAQ_FALLING_DIAG_AVERAGES_val);
        public static final LinearAveragesMode IMAQ_ALL_LINEAR_AVERAGES = new LinearAveragesMode(IMAQ_ALL_LINEAR_AVERAGES_val);

        private LinearAveragesMode (int value) {
            this.value = value;
        }
    }

    private class LinearAveragesStruct extends Structure {

        public void read() {
            columnCount = backingNativeMemory.getInt(4);
            columnAveragesPtr = backingNativeMemory.getPointer(0, 4*columnCount);
            columnAverages = new float[columnCount];
            columnAveragesPtr.getFloats(0, columnAverages, 0, columnCount);
            rowCount = backingNativeMemory.getInt(12);
            rowAveragesPtr = backingNativeMemory.getPointer(8, 4*rowCount);
            rowAverages = new float[rowCount];
            rowAveragesPtr.getFloats(0, rowAverages, 0, rowCount);
            risingDiagCount = backingNativeMemory.getInt(20);
            risingDiagAveragesPtr = backingNativeMemory.getPointer(16, 4*risingDiagCount);
            risingDiagAverages = new float[risingDiagCount];
            risingDiagAveragesPtr.getFloats(0, risingDiagAverages, 0, risingDiagCount);
            fallingDiagCount = backingNativeMemory.getInt(28);
            fallingDiagAveragesPtr = backingNativeMemory.getPointer(24, 4*fallingDiagCount);
            fallingDiagAverages = new float[fallingDiagCount];
            fallingDiagAveragesPtr.getFloats(0, fallingDiagAverages, 0, fallingDiagCount);
        }

        public void write() {
            backingNativeMemory.setPointer(0, columnAveragesPtr);
            backingNativeMemory.setInt(4, columnCount);
            backingNativeMemory.setPointer(8, rowAveragesPtr);
            backingNativeMemory.setInt(12, rowCount);
            backingNativeMemory.setPointer(16, risingDiagAveragesPtr);
            backingNativeMemory.setInt(20, risingDiagCount);
            backingNativeMemory.setPointer(24, fallingDiagAveragesPtr);
            backingNativeMemory.setInt(28, fallingDiagCount);
        }

        public int size() {
            return 32;
        }

        /**
         * Free the memory used by this range
         */
        public void free() {
            release();
        }

        /**
         * Create a new range with the specified upper and lower boundaries
         * @param lower The lower limit
         * @param upper The upper limit
         */
        public LinearAveragesStruct(int address) {
            useMemory(new Pointer(address, size()));
            read();
        }
    }

    LinearAverages(int address) {
        new LinearAveragesStruct(address);
    }

    public float[] getColumnAverages() {
        return columnAverages;
    }

    public float[] getRowAverages() {
        return rowAverages;
    }

    public float[] getRisingDiagAverages() {
        return risingDiagAverages;
    }

    public float[] getFallingDiagAverages() {
        return fallingDiagAverages;
    }
}
