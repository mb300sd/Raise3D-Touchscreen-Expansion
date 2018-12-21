#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/time.h>
#include <stdarg.h>
#include <linux/spi/spidev.h>

#define SPI_CS_GPIO_VALUE "/sys/class/gpio/gpio105/value"
#define SPI_CS_GPIO_DIRECTION "/sys/class/gpio/gpio105/direction"

static const char *spi_dev = "/dev/spidev32766.0";
static const int spi_cs_pin = 105;

uint8_t mode;
uint8_t bits = 8;
uint32_t speed = 50000;

int mcp300x_spi_transfer(int fd_dev ,uint8_t channel)
{
    int adc_value;
    uint8_t mcp300x_tx[3];
    uint8_t mcp300x_rx[3];
    struct spi_ioc_transfer tr;
    mcp300x_tx[0] = 1;                                   //first  byte transmitted ->start bit
    mcp300x_tx[1] = 0b10000000 |( ((channel & 7) << 4)); //second byte transmitted ->(SGL/DIF = 1, D2=D1=D0=channel)
    mcp300x_tx[2] = 0;                                   //third  byte transmitted ->don't care

    tr.tx_buf = (unsigned long)mcp300x_tx;
    tr.rx_buf = (unsigned long)mcp300x_rx;
    tr.len    = sizeof(mcp300x_tx);
    tr.delay_usecs = 0;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;

    if (ioctl(fd_dev, SPI_IOC_MESSAGE(1), &tr) == 1)
        printf("spi tx message failed\n");

    adc_value = (mcp300x_rx[1]<< 8) & 0b1100000000;
    adc_value |= (mcp300x_rx[2] & 0xff);
    close(fd_dev);
    return adc_value;
}

int mcp300x_spi_init(void)
{
    int fd_dev;

    //pinMode(GPIO14,INPUT);
    //pinMode(GPIO15,INPUT);
    //pinMode(GPIO16,INPUT);
    //pinMode(GPIO17,INPUT);
    //pinMode(GPIO18,INPUT);
    //pinMode(GPIO19,INPUT);

    //hw_pinMode(SPIEX_CS, IO_SPIEX_FUNC);   //CS
    //hw_pinMode(SPIEX_MOSI, IO_SPIEX_FUNC); //MOSI
    //hw_pinMode(SPIEX_MISO, IO_SPIEX_FUNC); //MISO
    //hw_pinMode(SPIEX_CLK,  IO_SPIEX_FUNC);  //CLK

    if ((fd_dev = open(spi_dev, O_RDWR)) < 0)
        printf("can't open device\n");
    if (ioctl(fd_dev, SPI_IOC_WR_MODE, &mode) < 0)
        printf("can't set spi mode\n");
    if (ioctl(fd_dev, SPI_IOC_RD_MODE, &mode) < 0)
        printf("can't get spi mode\n");
    if (ioctl(fd_dev, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
        printf("can't set bits per word\n");
    if (ioctl(fd_dev, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
        printf("can't get bits per word\n");
    if (ioctl(fd_dev, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
        printf("can't set max speed hz\n");
    if (ioctl(fd_dev, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
        printf("can't get max speed hz\n");
#if 0
    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
#endif

    return fd_dev;
}

int main(int argc, char **argv)
{
    int fd, cs_fd;
    int value = 0;
    int channel = 0;

    if (argc < 2)
    {
        printf("Usage: %s [channel]", argv[0]);
        exit(-1);
    }

    channel = atoi(argv[1]);

    fd = open(SPI_CS_GPIO_DIRECTION, O_RDWR);
    write(fd, "out", 3);
    close(fd);

    cs_fd = open(SPI_CS_GPIO_VALUE, O_RDWR);

    fd = mcp300x_spi_init();
    write(cs_fd, "1", 1);
    usleep(1);
    write(cs_fd, "0", 1);
    value = mcp300x_spi_transfer(fd, channel);
    write(cs_fd, "1", 1);
    close(cs_fd);

    printf("ADC%d: %d\n", channel, value);

    return 0;
}
