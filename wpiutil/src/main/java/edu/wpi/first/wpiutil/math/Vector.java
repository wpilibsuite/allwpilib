package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

public class Vector<R extends Num> extends Matrix<R, N1> {

    public Vector(SimpleMatrix storage) {
        super(storage);
    }

}
