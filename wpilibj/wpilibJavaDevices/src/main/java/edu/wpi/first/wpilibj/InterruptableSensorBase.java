/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.InterruptJNI;
import edu.wpi.first.wpilibj.util.AllocationException;
import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * Base for sensors to be used with interrupts
 */
public abstract class InterruptableSensorBase extends SensorBase {
	/**
	 * This is done to store the JVM variable in the InterruptJNI
	 * This is done because the HAL must have access to the JVM variable
	 * in order to attach the newly spawned thread when an interrupt is fired.
	 */
	static{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.initializeInterruptJVM(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * The interrupt resource
	 */
	protected ByteBuffer m_interrupt = null;

	/**
	 * Flags if the interrupt being allocated is synchronous
	 */
	protected boolean m_isSynchronousInterrupt = false;

	/**
	 * The index of the interrupt
	 */
	protected int m_interruptIndex;
	/**
	 * Resource manager
	 */
	protected static Resource interrupts = new Resource(8);

	/**
	 * Create a new InterrupatableSensorBase
	 */
	public InterruptableSensorBase() {
		m_interrupt = null;
	}

	/**
	 * @return true if this is an analog trigger
	 */
	abstract boolean getAnalogTriggerForRouting();

	/**
	 * @return channel routing number
	 */
	abstract int getChannelForRouting();

	/**
	 * @return module routing number
	 */
	abstract byte getModuleForRouting();

	/**
	 * Request one of the 8 interrupts asynchronously on this digital input.
	 *
	 * @param handler
	 *            The {@link InterruptHandlerFunction} that contains the method
	 *            {@link InterruptHandlerFunction#interruptFired(int, Object)} that
	 *            will be called whenever there is an interrupt on this device.
	 *            Request interrupts in synchronous mode where the user program
	 *            interrupt handler will be called when an interrupt occurs. The
	 *            default is interrupt on rising edges only.
	 */
	public void requestInterrupts(InterruptHandlerFunction<?> handler) {
		if(m_interrupt != null){
			throw new AllocationException("The interrupt has already been allocated");
		}

		allocateInterrupts(false);

		assert (m_interrupt != null);

		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.requestInterrupts(m_interrupt, getModuleForRouting(),
				getChannelForRouting(),
				(byte) (getAnalogTriggerForRouting() ? 1 : 0), status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		setUpSourceEdge(true, false);
		InterruptJNI.attachInterruptHandler(m_interrupt, handler.function, handler.overridableParamater(), status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Request one of the 8 interrupts synchronously on this digital input. Request
	 * interrupts in synchronous mode where the user program will have to
	 * explicitly wait for the interrupt to occur using {@link #waitForInterrupt}. 
	 * The default is interrupt on rising edges only.
	 */
	public void requestInterrupts() {
		if(m_interrupt != null){
			throw new AllocationException("The interrupt has already been allocated");
		}

		allocateInterrupts(true);

		assert (m_interrupt != null);

		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.requestInterrupts(m_interrupt, getModuleForRouting(),
				getChannelForRouting(),
				(byte) (getAnalogTriggerForRouting() ? 1 : 0), status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		setUpSourceEdge(true, false);

	}

	/**
	 * Allocate the interrupt
	 *
	 * @param watcher true if the interrupt should be in synchronous mode where the user
	 * program will have to explicitly wait for the interrupt to occur.
	 */
	protected void allocateInterrupts(boolean watcher) {
		try {
			m_interruptIndex = interrupts.allocate();
		} catch (CheckedAllocationException e) {
			throw new AllocationException(
					"No interrupts are left to be allocated");
		}
		m_isSynchronousInterrupt = watcher;

		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		m_interrupt = InterruptJNI.initializeInterrupts(m_interruptIndex,
				(byte) (watcher ? 1 : 0), status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Cancel interrupts on this device. This deallocates all the chipobject
	 * structures and disables any interrupts.
	 */
	public void cancelInterrupts() {
		if (m_interrupt == null) {
			throw new IllegalStateException("The interrupt is not allocated.");
		}
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.cleanInterrupts(m_interrupt, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		m_interrupt = null;
		interrupts.free(m_interruptIndex);
	}

	/**
	 * In synchronous mode, wait for the defined interrupt to occur.
	 *
	 * @param timeout
	 *            Timeout in seconds
	 * @param ignorePrevious
	 *            If true, ignore interrupts that happened before
	 *            waitForInterrupt was called.
	 */
	public void waitForInterrupt(double timeout, boolean ignorePrevious) {
		if (m_interrupt == null) {
			throw new IllegalStateException("The interrupt is not allocated.");
		}
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.waitForInterrupt(m_interrupt, (float) timeout, ignorePrevious, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * In synchronous mode, wait for the defined interrupt to occur.
	 *
	 * @param timeout
	 *            Timeout in seconds
	 */
	public void waitForInterrupt(double timeout) {
		waitForInterrupt(timeout, true);
	}

	/**
	 * Enable interrupts to occur on this input. Interrupts are disabled when
	 * the RequestInterrupt call is made. This gives time to do the setup of the
	 * other options before starting to field interrupts.
	 */
	public void enableInterrupts() {
		if (m_interrupt == null) {
			throw new IllegalStateException("The interrupt is not allocated.");
		}
		if(m_isSynchronousInterrupt){
			throw new IllegalStateException("You do not need to enable synchronous interrupts");
		}
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.enableInterrupts(m_interrupt, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Disable Interrupts without without deallocating structures.
	 */
	public void disableInterrupts() {
		if (m_interrupt == null) {
			throw new IllegalStateException("The interrupt is not allocated.");
		}
		if(m_isSynchronousInterrupt){
			throw new IllegalStateException("You can not disable synchronous interrupts");
		}
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		InterruptJNI.disableInterrupts(m_interrupt, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Return the timestamp for the rising interrupt that occurred most
	 * recently. This is in the same time domain as getClock().
	 * The rising-edge interrupt should be enabled with
	 * {@link #setUpSourceEdge}
	 * @return Timestamp in seconds since boot.
	 */
	public double readRisingTimestamp() {
		if (m_interrupt == null) {
			throw new IllegalStateException("The interrupt is not allocated.");
		}
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double timestamp = InterruptJNI.readRisingTimestamp(m_interrupt, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return timestamp;
	}

	/**
	* Return the timestamp for the falling interrupt that occurred most
	* recently. This is in the same time domain as getClock().
	* The falling-edge interrupt should be enabled with
	* {@link #setUpSourceEdge}
	* @return Timestamp in seconds since boot.
	*/
	public double readFallingTimestamp() {
		if (m_interrupt == null) {
			throw new IllegalStateException("The interrupt is not allocated.");
		}
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double timestamp = InterruptJNI.readFallingTimestamp(m_interrupt, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return timestamp;
	}

	/**
	 * Set which edge to trigger interrupts on
	 *
	 * @param risingEdge
	 *            true to interrupt on rising edge
	 * @param fallingEdge
	 *            true to interrupt on falling edge
	 */
	public void setUpSourceEdge(boolean risingEdge, boolean fallingEdge) {
		if (m_interrupt != null) {
			ByteBuffer status = ByteBuffer.allocateDirect(4);
			// set the byte order
			status.order(ByteOrder.LITTLE_ENDIAN);
			InterruptJNI.setInterruptUpSourceEdge(m_interrupt,
					(byte) (risingEdge ? 1 : 0), (byte) (fallingEdge ? 1 : 0),
					status.asIntBuffer());
			HALUtil.checkStatus(status.asIntBuffer());
		} else {
			throw new IllegalArgumentException(
					"You must call RequestInterrupts before setUpSourceEdge");
		}
	}
}
