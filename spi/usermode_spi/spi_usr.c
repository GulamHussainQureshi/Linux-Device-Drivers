#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static int pabort (const char *s)
{
  perror(s)
  return -1;
}

static int spi_device_setup (int fd)
{
int mode, speed, a, b, i=0, bits = 8;

/* spi mode = mode 0 */
mode = SPI_MODE_0;

a = ioctl(fd, SPI_IOC_WR_MODE, &mode);
b = ioctl(fd, SPI_IOC_RD_MODE, &mode);

if((a < 0) || (b < 0 )) {
    return pabort("can't set spi mode \n");
    }
    
/*clock max speed in Hz */
speed = 8000000; /* 8 Mhz */

a = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
b = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

if((a < 0) || (b < 0 )) {
    return pabort("can't set spi speed \n");
    }
    
/* setting SPI to MSB first.
 * Here, 0 means "not to use LSB first".
 * /
 
a = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &i);
b = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &i);

if((a < 0) || (b < 0 )) {
    return pabort("fail to set MSB first \n");
    }
    
/* setting SPI to 8 bits per word */

a = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
b = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);

if((a < 0) || (b < 0 )) {
    return pabort("can't set bits per word");
    }
    
return 0;

}


static void do_transfer (int fd)
{
  int ret;
  char txbuf[] = {0x01, 0x4C, 0x9F}; /* can use user mode scan funtions to fill txbuf[] instead of pre-defining it */
  char rxbuf[3] = {0, };
  char cmd_buff = 0x3D;
  
  struct spi_ioc_transfer tr[2] = {
   [0]= {
        .tx_buf = (unsigned long)&cmd_buff,
        .len = 1,
        .cs_change = 1,
        .delay_usecs = 50,
        .bits_per_word = 8,
        },
   [1]= {
        .tx_buf = (unsigned long)txbuf
        .rx_buff = (unsigned long)rxbuf
        .len = 3,
        .bits_per_word = 8,
        },
};

ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
if (ret == 1) {
    perror("can't send spi message");
    exit(1);
}

for (ret = 0, ret < sizeof(rxbuff), ret++)
    printf("%.2X \n", rxbuff[ret]);

}

int main(int argc, char **argv)
{
  char *device = "/dev/spidev0.0";
  int fd, error;
  
  fd = open(device, O_RDWR);
  if (fd < 0)
      return pabort(" Can't open device");
      
  error = spi_device_setup(fd);
  if(error)
      exit(1);
      
  do_transfer(fd);
  close(fd)
  
  return 0;
 
}
