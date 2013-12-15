#ifndef __BAIOS__
#define __BAIOS__

#include "networktables2/stream/IOStream.h"
#include <vector>
#include <stdint.h>
#include <cstring>


#define UPDATE_BAIOS(len, ...) { uint8_t tmp[] = {__VA_ARGS__}; \
	ByteArrayIOStream *input = new ByteArrayIOStream(tmp, len); \
	stream = new DataIOStream(input); \
        }

#define NEW_BAIOS(len, ...)  \
        DataIOStream* stream; \
	UPDATE_BAIOS(len, __VA_ARGS__)
	


class ByteArrayIOStream : public IOStream
{
	std::vector<uint8_t> buff;
public:
	ByteArrayIOStream(uint8_t buf[], size_t length);
	ByteArrayIOStream():buff(){}
	virtual ~ByteArrayIOStream(){}
	virtual int read(void* ptr, int numbytes);
	virtual int write(const void* ptr, int numbytes);
	virtual void flush(){}
	virtual void close(){}
	uint8_t* toByteArray(){return &buff[0];}
};

#endif
