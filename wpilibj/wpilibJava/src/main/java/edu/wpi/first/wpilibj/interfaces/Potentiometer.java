/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.interfaces;

import edu.wpi.first.wpilibj.PIDSource;

/**
 *
 * @author alex
 */
public interface Potentiometer extends PIDSource {
    double get();    
}
