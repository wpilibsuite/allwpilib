#ifndef DATAIOSTREAM_H_
#define DATAIOSTREAM_H_

#include <stdlib.h>
#include "networktables2/stream/IOStream.h"
#include <exception>
#include <string>

#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

#include <stdlib.h>
#include <memory>

//WindRiver is 3.4 GCC. Recent GCC can  do 0b01010101 style format, WR cannot

#define TO_HEX__(n) 0x##n##LU // LU for unsigned long
#define BINARY_LITERAL_VIA_HEX__(n) (((n & 0x00000001LU) ? 1 : 0)\
	+ ((n & 0x00000010LU) ? 2 : 0) \
	+ ((n & 0x00000100LU) ? 4 : 0) \
	+ ((n & 0x00001000LU) ? 8 : 0) \
	+ ((n & 0x00010000LU) ? 16 : 0) \
	+ ((n & 0x00100000LU) ? 32 : 0) \
	+ ((n & 0x01000000LU) ? 64 : 0) \
	+ ((n & 0x10000000LU) ? 128 : 0))
	
class DataIOStream{
public:
	DataIOStream(IOStream* stream);
	virtual ~DataIOStream();
	void writeByte(uint8_t b);
	void write2BytesBE(uint16_t s);
	void writeString(std::string& str);
	void flush();
	
	uint8_t readByte();
	uint16_t read2BytesBE();
	std::string* readString();
	
	void close();
	void SetIOStream(IOStream* stream);

//private:
	IOStream *iostream;
};


#endif

