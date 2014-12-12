#ifndef __I2C_LIB_H__
#define __I2C_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
int i2clib_open(const char *device);
void i2clib_close(int handle);
int i2clib_read(int handle, uint8_t dev_addr, char *recv_buf, int32_t recv_size);
int i2clib_write(int handle, uint8_t dev_addr, const char *send_buf, int32_t send_size);
int i2clib_writeread(int handle, uint8_t dev_addr, const char *send_buf, int32_t send_size, char *recv_buf, int32_t recv_size);
#ifdef __cplusplus
}
#endif

#endif /* __I2C_LIB_H__ */