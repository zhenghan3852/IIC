#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <math.h>

#define DEVICE_NAME "/dev/i2c-0"
#define I2C_ADDR  0x4a
#define REG_ADDR  0x00
int transfer(u_int8_t buf[], int size);
int lux;

int main(){
  u_int8_t luxBuff[4];
  int fd = -1;
  struct i2c_rdwr_ioctl_data data;
  data.msgs = (struct i2c_msg *)malloc(2 * sizeof(struct i2c_msg));
  unsigned char sendbuf[sizeof(unsigned char) + 1] = {0};
  unsigned char recvbuf[sizeof(unsigned char) + 1] =  {0};
  //0.open
  fd = open(DEVICE_NAME, O_RDWR);
  if(fd < 0) {
    printf("Failed to open device %s.\n", DEVICE_NAME);
    return -1;
  }
  printf("open device %s Success.\n", DEVICE_NAME);
  while (1) {
      /*-----0x00-----*/
        data.nmsgs = 2;
        data.msgs[0].buf = sendbuf;//申请内存为2个Byte
        data.msgs[0].len = 1;//目标reg的地址的长度
        data.msgs[0].addr = I2C_ADDR; //i2c设备地址
        data.msgs[0].flags = 0; //write flag
        data.msgs[0].buf[0] = REG_ADDR;//目标reg地址

        data.msgs[1].len = 1;//目标reg的地址的长度
        data.msgs[1].addr = I2C_ADDR; //i2c设备地址
        data.msgs[1].flags = 1; //read flag
        data.msgs[1].buf = recvbuf;//分配内存

        ioctl(fd,I2C_RDWR,(unsigned long)&data);
        luxBuff[0] = data.msgs[1].buf[0];
      /*-----0x01-----*/
        data.nmsgs = 2;
        data.msgs[0].buf = sendbuf;//申请内存为2个Byte
        data.msgs[0].len = 1;//目标reg的地址的长度
        data.msgs[0].addr = I2C_ADDR; //i2c设备地址
        data.msgs[0].flags = 0; //write flag
        data.msgs[0].buf[0] = 0x01;//目标reg地址

        data.msgs[1].len = 1;//目标reg的地址的长度
        data.msgs[1].addr = I2C_ADDR; //i2c设备地址
        data.msgs[1].flags = 1; //read flag
        data.msgs[1].buf = recvbuf;//分配内存

        ioctl(fd,I2C_RDWR,(unsigned long)&data);
        luxBuff[1] = data.msgs[1].buf[0];
        //printf("buf[1] = %x\n",data.msgs[1].buf[0]);
      /*-----0x02-----*/
        data.nmsgs = 2;
        data.msgs[0].buf = sendbuf;//申请内存为2个Byte
        data.msgs[0].len = 1;//目标reg的地址的长度
        data.msgs[0].addr = I2C_ADDR; //i2c设备地址
        data.msgs[0].flags = 0; //write flag
        data.msgs[0].buf[0] = 0x02;//目标reg地址

        data.msgs[1].len = 1;//目标reg的地址的长度
        data.msgs[1].addr = I2C_ADDR; //i2c设备地址
        data.msgs[1].flags = 1; //read flag
        data.msgs[1].buf = recvbuf;//分配内存

        ioctl(fd,I2C_RDWR,(unsigned long)&data);
        luxBuff[2] = data.msgs[1].buf[0];
        //printf("buf[2] = %x\n",data.msgs[1].buf[0]);
      /*-----0x03-----*/
        data.nmsgs = 2;
        data.msgs[0].buf = sendbuf;//申请内存为2个Byte
        data.msgs[0].len = 1;//目标reg的地址的长度
        data.msgs[0].addr = I2C_ADDR; //i2c设备地址
        data.msgs[0].flags = 0; //write flag
        data.msgs[0].buf[0] = 0x03;//目标reg地址

        data.msgs[1].len = 1;//目标reg的地址的长度
        data.msgs[1].addr = I2C_ADDR; //i2c设备地址
        data.msgs[1].flags = 1; //read flag
        data.msgs[1].buf = recvbuf;//分配内存

        ioctl(fd,I2C_RDWR,(unsigned long)&data);
        luxBuff[3] = data.msgs[1].buf[0];
        for(int i = 0; i < 4; i++)
        printf("%x\n",luxBuff[i]);
        //printf("buf[3] = %x\n",data.msgs[1].buf[0]);
        printf("The senser data is %d\n", transfer(luxBuff,4));
        lux = transfer(luxBuff,4) / 1000;
        printf("equal to %.3f lux\n", float(transfer(luxBuff,4)) / 1000);
        printf("equal to %.3f umol/m^2-s\n", float(transfer(luxBuff,4)) / (1000*55.6));
        printf("Set the exposure to %f us\n", 8562846.7 * pow(lux / 55.6, -1.1349));     //lux / 55.6
  }

  close(fd);

  return 0;
}

int transfer(u_int8_t buf[], int size)
{
    u_int32_t la, lb, lc ,ld, sum;

    la = buf[size-1];
    lb = buf[size-2];
    lc = buf[size-3];
    ld = buf[size-4];

    la = 0xFFFFFF | (la << 24);
    lb = 0xFF00FFFF | (lb << 16);
    lc = 0xFFFF00FF | (lc << 8);
    ld = 0xFFFFFF00 | ld;

    sum = la & lb & lc & ld;

    return sum;
}
