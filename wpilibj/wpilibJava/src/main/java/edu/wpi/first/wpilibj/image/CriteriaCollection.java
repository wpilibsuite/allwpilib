/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.image;

import com.sun.jna.Pointer;
import edu.wpi.first.wpilibj.image.NIVision.MeasurementType;
import java.util.Vector;

class ParticleFilterCriteria {
    MeasurementType type;
    float lower;
    float upper;
    boolean outsideRange;

    ParticleFilterCriteria(MeasurementType type, float lower, float upper, boolean outsideRange) {
        this.type = type;
        this.lower = lower;
        this.upper = upper;
        this.outsideRange = outsideRange;
    }
}

public class CriteriaCollection {
    Vector criteria = new Vector();

    public void addCriteria(MeasurementType type, float lower, float upper, boolean outsideRange) {
        criteria.addElement(new ParticleFilterCriteria(type, lower, upper, outsideRange));
    }

    public int getNumberOfCriteria() {
        return criteria.size();
    }

    public Pointer getCriteriaArray() {
        Pointer p = new Pointer(criteria.size() * 5 * 4);  // 5 elements each 4 bytes
        for (int i = 0; i < criteria.size(); i++) {
            ParticleFilterCriteria pfc = (ParticleFilterCriteria) criteria.elementAt(i);
            p.setInt(i * 20, pfc.type.value);
            p.setFloat(i * 20 + 4, pfc.lower);
            p.setFloat(i * 20 + 8, pfc.upper);
            p.setInt(i * 20 + 12, 0);   // always use pixel measurements for now
            p.setInt(i * 20 + 16, pfc.outsideRange ? 1 : 0);
        }
        return p;
    }
}

