package edu.wpi.first.math.system;

import edu.wpi.first.math.interpolation.InterpolatingTreeMap;
import edu.wpi.first.math.interpolation.Interpolator;
import edu.wpi.first.math.interpolation.InverseInterpolator;
import edu.wpi.first.math.trajectory.TrajectorySample;

import java.util.List;

import static edu.wpi.first.units.Units.Seconds;

public class Trajectory {
    private final InterpolatingTreeMap<Double, TrajectorySample> samples;

    public Trajectory(List<TrajectorySample> samples) {
        this.samples = new InterpolatingTreeMap<>(InverseInterpolator.forDouble(), TrajectorySample::interpolate);

        samples.forEach(sample -> {
            this.samples.put(sample.timestamp.in(Seconds), sample);
        });
    }
}
