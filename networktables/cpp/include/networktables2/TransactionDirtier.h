/*
 * TransactionDirtier.h
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#ifndef TRANSACTIONDIRTIER_H_
#define TRANSACTIONDIRTIER_H_


class TransactionDirtier;


#include "networktables2/OutgoingEntryReceiver.h"



/**
 * A transaction receiver that marks all Table entries as dirty in the entry store. Entries will not be passed to the continuing receiver if they are already dirty
 * 
 * @author Mitchell
 *
 */
class TransactionDirtier : public OutgoingEntryReceiver {
private:
	OutgoingEntryReceiver& continuingReceiver;
	
public:
	TransactionDirtier(OutgoingEntryReceiver& continuingReceiver);
	void offerOutgoingAssignment(NetworkTableEntry* entry);
	void offerOutgoingUpdate(NetworkTableEntry* entry);

};

#endif /* TRANSACTIONDIRTIER_H_ */
