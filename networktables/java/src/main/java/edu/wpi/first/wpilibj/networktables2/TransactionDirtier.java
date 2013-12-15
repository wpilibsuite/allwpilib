package edu.wpi.first.wpilibj.networktables2;


/**
 * A transaction receiver that marks all Table entries as dirty in the entry store. Entries will not be passed to the continuing receiver if they are already dirty
 * 
 * @author Mitchell
 *
 */
public class TransactionDirtier implements OutgoingEntryReceiver {
	private final OutgoingEntryReceiver continuingReceiver;
	
	public TransactionDirtier(final OutgoingEntryReceiver continuingReceiver) {
		this.continuingReceiver = continuingReceiver;
	}



	public void offerOutgoingAssignment(NetworkTableEntry entry) {
		if(entry.isDirty())
			return;
		entry.makeDirty();
		continuingReceiver.offerOutgoingAssignment(entry);
	}




	public void offerOutgoingUpdate(NetworkTableEntry entry) {
		if(entry.isDirty())
			return;
		entry.makeDirty();
		continuingReceiver.offerOutgoingUpdate(entry);
	}

}
