/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.testing;

/**
 *
 * @author alex
 */
public class Utils {
    public static int[] combine(int[] a, int[] b) {
        int[] out = new int[a.length+b.length];
        System.arraycopy(a, 0, out, 0, a.length);
        System.arraycopy(b, 0, out, a.length, b.length);
        return out;
    }
}
