/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.hal.InterruptJNI;
import edu.wpi.first.wpilibj.hal.InterruptJNI.InterruptHandlerFunction;
import edu.wpi.first.wpilibj.hal.HALUtil;

/**
 * Base for sensors to be used with interrupts
 */
public abstract class InterruptableSensorBase extends SensorBase {

	/**
	 * The interrupt resource
	 */
	protected ByteBuffer m_interrupt;
	/**
	 * The interrupt manager resource
	 */
	protected InterruptHandlerFunction m_manager;
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
		m_manager = null;
		m_interrupt = null;
	}

	/**
	 * Allocate the interrupt
	 * 
	 * @param watcher
	 */
	public void allocateInterrupts(boolean watcher) {
		if (!watcher) {
			throw new IllegalArgumentException(
					"Interrupt callbacks not yet supported");
		}
		// Expects the calling leaf class to allocate an interrupt index.
		IntBuffer status = IntBuffer.allocate(1);
		m_interrupt = InterruptJNI.initializeInterrupts(m_interruptIndex,
				(byte) (watcher ? 1 : 0), status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Cancel interrupts on this device. This deallocates all the chipobject
	 * structures and disables any interrupts.
	 */
	public void cancelInterrupts() {
		if (m_interrupt == null || m_manager == null) {
			throw new IllegalStateException();
		}
		IntBuffer status = IntBuffer.allocate(1);
		InterruptJNI.cleanInterrupts(m_interrupt, status);
		HALUtil.checkStatus(status);
		
	}

	/**
	 * In synchronous mode, wait for the defined interrupt to occur.
	 * 
	 * @param timeout
	 *            Timeout in seconds
	 */
	public void waitForInterrupt(double timeout) {
		if (m_interrupt == null || m_manager == null) {
			throw new IllegalStateException();
		}
		IntBuffer status = IntBuffer.allocate(1);
		InterruptJNI.waitForInterrupt(m_interrupt, (float) timeout, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Enable interrupts to occur on this input. Interrupts are disabled when
	 * the RequestInterrupt call is made. This gives time to do the setup of the
	 * other options before starting to field interrupts.
	 */
	public void enableInterrupts() {
		if (m_interrupt == null || m_manager == null) {
			throw new IllegalStateException();
		}
		IntBuffer status = IntBuffer.allocate(1);
		InterruptJNI.enableInterrupts(m_interrupt, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Disable Interrupts without without deallocating structures.
	 */
	public void disableInterrupts() {
		if (m_interrupt == null || m_manager == null) {
			throw new IllegalStateException();
		}
		IntBuffer status = IntBuffer.allocate(1);
		InterruptJNI.disableInterrupts(m_interrupt, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Return the timestamp for the interrupt that occurred most recently. This
	 * is in the same time domain as getClock().
	 * 
	 * @return Timestamp in seconds since boot.
	 */
	public double readInterruptTimestamp() {
		if (m_interrupt == null || m_manager == null) {
			throw new IllegalStateException();
		}
		IntBuffer status = IntBuffer.allocate(1);
		double timestamp = InterruptJNI.readInterruptTimestamp(m_interrupt, status);
		HALUtil.checkStatus(status);
		return timestamp;
	}
}
