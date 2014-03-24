/*
 * OutgoingEntryReciever.cpp
 *
 *  Created on: Nov 3, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/OutgoingEntryReceiver.h"

OutgoingEntryReceiver_NULL_t OutgoingEntryReceiver_NULL;

void OutgoingEntryReceiver_NULL_t::offerOutgoingAssignment(NetworkTableEntry* entry){}
void OutgoingEntryReceiver_NULL_t::offerOutgoingUpdate(NetworkTableEntry* entry){}
