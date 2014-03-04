#include "networktables2/NetworkTableEntry.h"
#include "networktables2/AbstractNetworkTableEntryStore.h"


NetworkTableEntry::NetworkTableEntry(std::string& _name, NetworkTableEntryType* _type, EntryValue _value)
  : name(_name), type(_type){
	id = UNKNOWN_ID;
	sequenceNumber = 0;
	value = type->copyValue(_value);
	m_isNew = true;
	m_isDirty = false;
}

NetworkTableEntry::NetworkTableEntry(EntryId _id, std::string& _name, SequenceNumber _sequenceNumber, NetworkTableEntryType* _type, EntryValue _value)
 :name(_name), type(_type){
	id = _id;
	sequenceNumber = _sequenceNumber;
	value = type->copyValue(_value);
	m_isNew = true;
	m_isDirty = false;
}

NetworkTableEntry::NetworkTableEntry(const NetworkTableEntry &other) :
	name(other.name),
	id(other.id),
	sequenceNumber(other.sequenceNumber),
	type(other.type),
	m_isNew(other.m_isNew),
	m_isDirty(other.m_isDirty)
{
	value = type->copyValue(other.value);
}

NetworkTableEntry::~NetworkTableEntry(){
  type->deleteValue(value);
}

EntryId NetworkTableEntry::GetId() {
	return id;
}

EntryValue NetworkTableEntry::GetValue(){
	return value;
}

NetworkTableEntryType* NetworkTableEntry::GetType(){
	return type;
}

bool NetworkTableEntry::PutValue(SequenceNumber newSequenceNumber, EntryValue newValue) {
	if( (sequenceNumber < newSequenceNumber && newSequenceNumber - sequenceNumber < HALF_OF_SEQUENCE_NUMBERS)
			|| (sequenceNumber > newSequenceNumber && sequenceNumber - newSequenceNumber > HALF_OF_SEQUENCE_NUMBERS) ){
	  EntryValue newValueCopy = type->copyValue(newValue);
	  type->deleteValue(value);
	  value = newValueCopy;
	  sequenceNumber = newSequenceNumber;
	  return true;
	}
	return false;
}
/**
 * force a value and new sequence number upon an entry
 * @param newSequenceNumber
 * @param newValue
 */
void NetworkTableEntry::ForcePut(SequenceNumber newSequenceNumber, EntryValue newValue) {
  EntryValue newValueCopy = type->copyValue(newValue);
  type->deleteValue(value);
  value = newValueCopy;
  sequenceNumber = newSequenceNumber;
}
/**
 * force a value and new sequence number upon an entry, Will also set the type of the entry
 * @param newSequenceNumber
 * @param type
 * @param newValue
 */
void NetworkTableEntry::ForcePut(SequenceNumber newSequenceNumber, NetworkTableEntryType* newType, EntryValue newValue) {
        type->deleteValue(value);
	type = newType;
        value = newType->copyValue(newValue);
	sequenceNumber = newSequenceNumber;
}


void NetworkTableEntry::MakeDirty() {
	m_isDirty = true;
}
void NetworkTableEntry::MakeClean() {
	m_isDirty = false;
}
bool NetworkTableEntry::IsDirty(){
	return m_isDirty;
}

/**
 * Send the value of the entry over the output stream
 * @param os
 * @throws IOException
 */
void NetworkTableEntry::SendValue(DataIOStream& iostream){
	type->sendValue(value, iostream);
}

/**
 * @return the current sequence number of the entry
 */
SequenceNumber NetworkTableEntry::GetSequenceNumber() {
	return sequenceNumber;
}
/**
 * Sets the id of the entry
 * @param id the id of the entry
 * @throws IllegalStateException if the entry already has a known id
 */
void NetworkTableEntry::SetId(EntryId _id){
	if(id!=UNKNOWN_ID)
		throw IllegalStateException("Cannot set the Id of a table entry that already has a valid id");
	id = _id;
}
/**
 * clear the id of the entry to unknown
 */
void NetworkTableEntry::ClearId() {
	id = UNKNOWN_ID;
}

void NetworkTableEntry::Send(NetworkTableConnection& connection) {
	connection.sendEntryAssignment(*this);
}
void NetworkTableEntry::FireListener(TableListenerManager& listenerManager) {
	listenerManager.FireTableListeners(name, value, m_isNew);
	m_isNew = false;
}
