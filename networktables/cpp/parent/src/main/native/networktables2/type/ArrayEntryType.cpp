/*
 * ArrayEntryType.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/ArrayEntryType.h"

ArrayEntryType::ArrayEntryType(TypeId id, NetworkTableEntryType& elementType)
	: ComplexEntryType(id, "Array"), m_elementType(elementType){ //TODO super(id, "Array of [" + elementType.name + "]");
}

EntryValue ArrayEntryType::copyElement(EntryValue value){
  return m_elementType.copyValue(value);
}
void ArrayEntryType::deleteElement(EntryValue value){
  m_elementType.deleteValue(value);
}
bool ArrayEntryType::areElementsEqual(EntryValue v1, EntryValue v2){
  return m_elementType.areEqual(v1, v2);
}


void ArrayEntryType::sendValue(EntryValue value, DataIOStream& os) {
	ArrayEntryData* dataArray = (ArrayEntryData*) value.ptr;
	/*if (dataArray->length > 255) {//TODO throw better exception
		throw IOException("Cannot write " + value + " as " + name + ". Arrays have a max length of 255 values");
	}*/
	os.writeByte(dataArray->length);
	for (int i = 0; i < dataArray->length; ++i) {
		m_elementType.sendValue(dataArray->array[i], os);
	}
}

EntryValue ArrayEntryType::readValue(DataIOStream& is) {
	uint8_t length = is.readByte();
	EntryValue* array = (EntryValue*)malloc(length*sizeof(EntryValue));//TODO cache object arrays
	for (int i = 0; i < length; ++i) {
		array[i] = m_elementType.readValue(is);
	}
	
	ArrayEntryData* dataArray = (ArrayEntryData*)malloc(sizeof(ArrayEntryData));
	dataArray->length = length;
	dataArray->array = array;
	
	EntryValue eValue;
	eValue.ptr = dataArray;
	return eValue;
}

EntryValue ArrayEntryType::copyValue(EntryValue value){
	ArrayEntryData* otherDataArray = (ArrayEntryData*) value.ptr;

	EntryValue* array = (EntryValue*)malloc(otherDataArray->length*sizeof(EntryValue));
	for (int i = 0; i < otherDataArray->length; ++i) {
		array[i] = copyElement(otherDataArray->array[i]);
	}
	
	ArrayEntryData* dataArray = (ArrayEntryData*)malloc(sizeof(ArrayEntryData));
	dataArray->length = otherDataArray->length;
	dataArray->array = array;
	
	EntryValue eValue;
	eValue.ptr = dataArray;
	return eValue;
}
void ArrayEntryType::deleteValue(EntryValue value){
	ArrayEntryData* dataArray = (ArrayEntryData*) value.ptr;
	if(dataArray!=NULL){
	  for (int i = 0; i < dataArray->length; ++i) {
	    deleteElement(dataArray->array[i]);
	  }
	  if(dataArray->array != NULL)
	    free(dataArray->array);

	  free(dataArray);
	}
}
bool ArrayEntryType::areEqual(EntryValue v1, EntryValue v2) {
  ArrayEntryData* a1 = (ArrayEntryData*) v1.ptr;
  ArrayEntryData* a2 = (ArrayEntryData*) v2.ptr;
  if(a1->length != a2->length)
    return false;
  for (int i = 0; i < a1->length; ++i) {
    if(!areElementsEqual(a1->array[i], a2->array[i]))
      return false;
  }
  return true;
}

EntryValue ArrayEntryType::internalizeValue(std::string& key, ComplexData& externalRepresentation, EntryValue currentInteralValue) {
	// TODO: Argument 'key' appears unused.
	ArrayData& externalArrayData = (ArrayData&)externalRepresentation;

	EntryValue eValue = currentInteralValue;
	ArrayEntryData* internalArray = (ArrayEntryData*) currentInteralValue.ptr;
	if(internalArray == NULL){
		internalArray = (ArrayEntryData*)malloc(sizeof(ArrayEntryData));
		internalArray->length = 0;
		internalArray->array = NULL;
		eValue.ptr = internalArray;
	}

	if(internalArray->length==externalArrayData.size()){
		for(int i = 0; i<internalArray->length; ++i){
			deleteElement(internalArray->array[i]);
			internalArray->array[i] = copyElement(externalArrayData._get(i));
		}
	}
	else{
	        if(internalArray->array != NULL)
	                free(internalArray->array);
		internalArray->length = externalArrayData.size();
		if(internalArray->length == 0)
		  internalArray->array = NULL;
		else
		  internalArray->array = (EntryValue*)malloc(externalArrayData.size()*sizeof(EntryValue));
		for (int i = 0; i < internalArray->length; ++i) {
			internalArray->array[i] = copyElement(externalArrayData._get(i));
		}
	}
	return eValue;
}

void ArrayEntryType::exportValue(std::string& key, EntryValue internalData, ComplexData& externalRepresentation) {
	ArrayEntryData* internalArray = (ArrayEntryData*) internalData.ptr;
	ArrayData& externalArrayData = (ArrayData&)externalRepresentation;
	externalArrayData.setSize(internalArray->length);
	for(int i = 0; i<internalArray->length; ++i){
		externalArrayData._set(i, copyElement(internalArray->array[i]));
	}
}
