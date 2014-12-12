#ifndef __SPI_LIB_H__
#define __SPI_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
int spilib_open(const char *device);
void spilib_close(int handle);
int spilib_setspeed(int handle, uint32_t speed);
int spilib_setbitsperword(int handle, uint8_t bpw);
int spilib_setopts(int handle, int msb_first, int sample_on_trailing, int clk_idle_high);
int spilib_read(int handle, char *recv_buf, int32_t size);
int spilib_write(int handle, const char *send_buf, int32_t size);
int spilib_writeread(int handle, const char *send_buf, char *recv_buf, int32_t size);
#ifdef __cplusplus
}
#endif

#endif /* __SPI_LIB_H__ */