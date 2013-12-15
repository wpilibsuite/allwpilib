package edu.wpi.first.wpilibj.networktables2;



public interface OutgoingEntryReceiver {
	OutgoingEntryReceiver NULL = new OutgoingEntryReceiver() {
		public void offerOutgoingUpdate(NetworkTableEntry entry) {
		}
		public void offerOutgoingAssignment(NetworkTableEntry entry) {
		}
	};
	
	public void offerOutgoingAssignment(NetworkTableEntry entry);
	public void offerOutgoingUpdate(NetworkTableEntry entry);
}
