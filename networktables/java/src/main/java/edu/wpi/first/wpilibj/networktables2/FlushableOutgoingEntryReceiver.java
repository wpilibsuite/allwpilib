package edu.wpi.first.wpilibj.networktables2;



public interface FlushableOutgoingEntryReceiver extends OutgoingEntryReceiver{
	public void flush();

	public void ensureAlive();
}
