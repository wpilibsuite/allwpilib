package edu.wpi.first.wpilibj.system;

import edu.wpi.first.wpilibj.controller.LinearQuadraticRegulator;
import edu.wpi.first.wpilibj.estimator.KalmanFilter;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

public class LinearSystemLoop<States extends Num, Inputs extends Num,
        Outputs extends Num> {

    private final Nat<States> states;
    private final Nat<Inputs> inputs;
    private final Nat<Outputs> outputs;
    private LinearSystem<States, Inputs, Outputs> m_plant;
    private LinearQuadraticRegulator<States, Inputs, Outputs> m_controller;
    private KalmanFilter<States, Inputs, Outputs> m_observer;
    private Matrix<States, N1> m_nextR;

    public LinearSystemLoop(Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs,
                            LinearSystem<States, Inputs, Outputs> plant,
                            LinearQuadraticRegulator<States, Inputs, Outputs> controller,
                            KalmanFilter<States, Inputs, Outputs> observer) {

        this.states = states;
        this.inputs = inputs;
        this.outputs = outputs;
        this.m_plant = plant;
        this.m_controller = controller;
        this.m_observer = observer;

        reset();
    }

    /**
     * Enables the controller.
     */
    public void enable() {
        m_controller.enable();
    }

    /**
     * Disables the controller and zeros the control input.
     */
    public void disable() {
        m_controller.disable();
    }

    /**
     * If the controller is enabled.
     * @return if the controller is enabled.
     */
    public boolean isEnabled() {
        return m_controller.isEnabled();
    }

    /**
     * Returns the observer's state estimate x-hat.
     */
    public Matrix<States, N1> getXHat() {
        return m_observer.getXhat();
    }

    /**
     * Returns an element of the observer's state estimate x-hat.
     *
     * @param i Row of x-hat.
     */
    public double getXHat(int i) {
        return m_observer.getXhat(i);
    }

    /**
     * Returns the controller's next reference r.
     */
    public Matrix<States, N1> getNextR() {
        return m_nextR;
    }

    /**
     * Returns an element of the controller's next reference r.
     *
     * @param i Row of r.
     */
    public double getNextR(int i) {
        return m_nextR.get(i, 0);
    }

    /**
     * Returns the controller's calculated control input u.
     */
    public Matrix<Inputs, N1> getU() {
        return m_plant.clampInput(m_controller.getU());
    }

    /**
     * Returns an element of the controller's calculated control input u.
     *
     * @param i Row of u.
     */
    public double getU(int i) {
        return getU().get(i, 0);
    }

    /**
     * Set the initial state estimate x-hat.
     *
     * @param xHat The initial state estimate x-hat.
     */
    public void setXHat(Matrix<States, N1> xHat) {
        m_observer.setXhat(xHat);
    }

    /**
     * Set an element of the initial state estimate x-hat.
     *
     * @param i     Row of x-hat.
     * @param value Value for element of x-hat.
     */
    public void setXHat(int i, double value) {
        m_observer.setXhat(i, value);
    }

    /**
     * Set the next reference r.
     *
     * @param nextR Next reference.
     */
    public void setNextR(Matrix<States, N1> nextR) {
        m_nextR = nextR;
    }

    /**
     * Return the plant used internally.
     */
    public LinearSystem<States, Inputs, Outputs> getPlant() {
        return m_plant;
    }

    /**
     * Return the controller used internally.
     */
    public LinearQuadraticRegulator<States, Inputs, Outputs> getController() {
        return m_controller;
    }

    /**
     * Return the observer used internally.
     */
    public KalmanFilter<States, Inputs, Outputs> getObserver() {
        return m_observer;
    }

    /**
     * Zeroes reference r, controller output u, plant output y, and state estimate
     * x-hat.
     */
    public void reset() {
        m_plant.reset();
        m_controller.reset();
        m_observer.reset();
        m_nextR = new Matrix<>(new SimpleMatrix(states.getNum(), 1));
    }

    /**
     * Returns difference between reference r and x-hat.
     */
    public Matrix<States, N1> getError() {
        return m_controller.getR().minus(m_observer.getXhat());
    }

    /**
     * Correct the state estimate x-hat using the measurements in y.
     *
     * @param y Measurement vector.
     */
    public void correct(Matrix<Outputs, N1> y) {
        m_observer.correct(m_controller.getU(), y);
    }

    /**
     * Sets new controller output, projects model forward, and runs observer
     * prediction.
     *
     * After calling this, the user should send the elements of u to the
     * actuators.
     *
     * @param dtSeconds Timestep for model update.
     */
    public void predict(double dtSeconds) {
        m_controller.update(m_observer.getXhat(), m_nextR);
        m_observer.predict(m_controller.getU(), dtSeconds);
    }


    /**
     * Sets the current controller to be "index". This can be used for gain
     * scheduling.
     *
     * @param index The controller index.
     */
    public void setIndex(int index) {
//        m_plant.setIndex(index); // TODO get impl from Tyler
//        m_controller.setIndex(index);
//        m_observer.setIndex(index);
    }

    /**
     * Returns the current controller index.
     */
    public int getIndex() {
//        return m_plant.getIndex(); // TODO get impl from Tyler
        return 0;
    }

}
