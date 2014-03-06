/*
 * WriteManager.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/WriteManager.h"
#include "networktables2/util/System.h"
#include "networktables2/AbstractNetworkTableEntryStore.h"
#include <iostream>



WriteManager::WriteManager(FlushableOutgoingEntryReceiver& _receiver, NTThreadManager& _threadManager, AbstractNetworkTableEntryStore& _entryStore, unsigned long _keepAliveDelay)
	: receiver(_receiver), threadManager(_threadManager), entryStore(_entryStore), keepAliveDelay(_keepAliveDelay){
	
	thread = NULL;
	lastWrite = 0;
	
	incomingAssignmentQueue = new std::queue<NetworkTableEntry*>();
	incomingUpdateQueue = new std::queue<NetworkTableEntry*>();
	outgoingAssignmentQueue = new std::queue<NetworkTableEntry*>();
	outgoingUpdateQueue = new std::queue<NetworkTableEntry*>();
}

WriteManager::~WriteManager(){
	stop();

	//Note: this must occur after stop() to avoid deadlock
	transactionsLock.take();
	
	delete incomingAssignmentQueue;
	delete incomingUpdateQueue;
	delete outgoingAssignmentQueue;
	delete outgoingUpdateQueue;
}

void WriteManager::start(){
	if(thread!=NULL)
		stop();
	lastWrite = currentTimeMillis();
	thread = threadManager.newBlockingPeriodicThread(this, "Write Manager Thread");
}

void WriteManager::stop(){
  if(thread!=NULL){
    thread->stop();
    delete thread;
    thread = NULL;
  }
}


void WriteManager::offerOutgoingAssignment(NetworkTableEntry* entry) {
	{ 
		bool test_queue_overflow=false;
		{
			NTSynchronized sync(transactionsLock);
			((std::queue<NetworkTableEntry*>*)incomingAssignmentQueue)->push(entry);

			test_queue_overflow=(((std::queue<NetworkTableEntry*>*)incomingAssignmentQueue)->size()>=queueSize);
		}
		if (test_queue_overflow)
		{
			run();
			writeWarning("assignment queue overflowed. decrease the rate at which you create new entries or increase the write buffer size");
		}
	}
}


void WriteManager::offerOutgoingUpdate(NetworkTableEntry* entry) {
	{ 
		bool test_queue_overflow=false;
		{
			NTSynchronized sync(transactionsLock);
			((std::queue<NetworkTableEntry*>*)incomingUpdateQueue)->push(entry);
			test_queue_overflow=(((std::queue<NetworkTableEntry*>*)incomingUpdateQueue)->size()>=queueSize);

		}
		if (test_queue_overflow)
		{
			run();
			writeWarning("update queue overflowed. decrease the rate at which you update entries or increase the write buffer size");
		}
	}
}


void WriteManager::run() {
	{
		NTSynchronized sync(transactionsLock);
		//swap the assignment and update queue
		volatile std::queue<NetworkTableEntry*>* tmp = incomingAssignmentQueue;
		incomingAssignmentQueue = outgoingAssignmentQueue;
		outgoingAssignmentQueue = tmp;
		
		tmp = incomingUpdateQueue;
		incomingUpdateQueue = outgoingUpdateQueue;
		outgoingUpdateQueue = tmp;
	}
	
	bool wrote = false;
	NetworkTableEntry* entry;
	
	while(!((std::queue<NetworkTableEntry*>*)outgoingAssignmentQueue)->empty()){
		entry = ((std::queue<NetworkTableEntry*>*)outgoingAssignmentQueue)->front();
		((std::queue<NetworkTableEntry*>*)outgoingAssignmentQueue)->pop();
		{
			NetworkTableEntry * entryCopy;

			{
				NTSynchronized sync(entryStore.LOCK);
				entry->MakeClean();
				wrote = true;
				entryCopy = new NetworkTableEntry(*entry);
			}

			receiver.offerOutgoingAssignment(entryCopy);
			delete entryCopy;
		}
	}
	
	while(!((std::queue<NetworkTableEntry*>*)outgoingUpdateQueue)->empty()){
		entry = ((std::queue<NetworkTableEntry*>*)outgoingUpdateQueue)->front();
		((std::queue<NetworkTableEntry*>*)outgoingUpdateQueue)->pop();
		{ 
			NetworkTableEntry * entryCopy;

			{
				NTSynchronized sync(entryStore.LOCK);
				entry->MakeClean();
				wrote = true;
				entryCopy = new NetworkTableEntry(*entry);
			}

			receiver.offerOutgoingUpdate(entryCopy);
			delete entryCopy;
		}
	}
	
	
	
	if(wrote){
		receiver.flush();
		lastWrite = currentTimeMillis();
	}
	else if(currentTimeMillis()-lastWrite>keepAliveDelay)
		receiver.ensureAlive();
	
	sleep_ms(20);
}

