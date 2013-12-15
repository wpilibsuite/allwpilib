#include "ByteArrayIOStream.h"
#include <cstring>
#include "networktables2/util/IOException.h"


ByteArrayIOStream::ByteArrayIOStream(uint8_t buf[], size_t len) : buff()
{
	buff.assign(buf, buf + len);
}

int ByteArrayIOStream::read(void* ptr, int numbytes)
{
	if(numbytes>buff.size())
		throw IOException("read past end of stream");
	memcpy(ptr, &buff[0], numbytes);
	buff.erase (buff.begin(),buff.begin()+numbytes);
}
int ByteArrayIOStream::write(const void* ptr, int numbytes)
{
	for (int i = 0; i < numbytes; i++)
		buff.push_back(*((char*)ptr+i));
}
