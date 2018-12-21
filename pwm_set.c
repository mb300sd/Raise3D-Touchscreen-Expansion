/*
* PWM test program
*/
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

#define PWMTMR_START    (0x101)
#define PWMTMR_STOP     (0x102)
#define PWMTMR_FUNC     (0x103)
#define PWMTMR_TONE     (0x104)
#define PWM_CONFIG      (0x105)
#define HWPWM_DUTY      (0x106)
#define PWM_FREQ        (0x107)

#define MAX_DUTY (720)
#define MIN_DUTY (0)
#define MAX_FREQ (2000000)
#define MIN_FREQ (1)

static const char *pwm_dev = "/dev/pwmtimer";

typedef struct tagPWM_Config {
    int channel;
    int dutycycle;
} PWM_Config,*pPWM_Config;

typedef struct tagPWM_Freq {
    int channel;
    int step;
    int pre_scale;
    unsigned int freq;
} PWM_Freq,*pPWM_Freq;

void pabort(const char *s)
{
    perror(s);
    abort();
}

int main(int argc, char **argv)
{
    if ( argc != 3 )
    {
        printf("Usage %s Frequency[%d-%d]Hz Duty Level [%d-%d]\n\n", argv[0], MIN_FREQ, MAX_FREQ, MIN_DUTY, MAX_DUTY);
        exit(-1);
    }

    uint8_t pwm_pin = 5;// atoi(argv[1]);
    unsigned int freq = atoi(argv[1]);
    unsigned int duty = atoi(argv[2]);
    int fd = -1;

    if (duty < MIN_DUTY || duty > MAX_DUTY)
        pabort("Invalid duty cycle");
    if (freq < MIN_FREQ || freq > MAX_FREQ)
        pabort("Invalid frequency");

    //pwmfreq_set
    PWM_Freq pwmfreq;
    pwmfreq.channel = pwm_pin;
    pwmfreq.freq = (unsigned int)((double)1000000000.0/freq);
    pwmfreq.step = 0;
    if ((fd = open(pwm_dev, O_RDONLY)) < 0)
        pabort("open pwm device fail");
    if (ioctl(fd, PWM_FREQ, &pwmfreq) < 0)
        pabort("can't set PWM_FREQ");
    if(fd)
        close(fd);

    //analogWrite
    PWM_Config pwmconfig;
    pwmconfig.channel = pwm_pin;
    pwmconfig.dutycycle = duty;

    if ((fd = open(pwm_dev, O_RDONLY)) < 0)
        pabort("open pwm device fail");
    if (ioctl(fd, PWM_CONFIG, &pwmconfig) < 0)
        pabort("can't set PWM_CONFIG");
    if (ioctl(fd, PWMTMR_START, &pwmconfig) < 0)
        pabort("can't set PWMTMR_START");
    if(fd)
        close(fd);

    printf("PWM%d: [%dHz, %d/%d]\n", pwm_pin, freq, duty, MAX_DUTY);

    return 0;
}
