#!/bin/bash

arm-linux-gnueabihf-gcc adc_read.c -o adc_read
arm-linux-gnueabihf-gcc pwm_set.c -o pwm_set
