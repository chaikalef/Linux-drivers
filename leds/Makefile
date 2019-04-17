# Constanst definition
CC     = gcc
FLAGS = -g -Wall -Wstrict-prototypes -ansi -pedantic


# Find out the magic with KERNELRELEASE and right way to compile
# several binaries at once

ifneq (${KERNELRELEASE},)
	obj-m := leds.o
else
	KERNEL_SOURCE := /usr/src/linux-headers-4.18.0-18-generic
	PWD := $(shell pwd)
	
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	sudo insmod leds.ko
	gcc -c test.c
	gcc test.o -o test

check:
	lsmod | grep leds
	echo
	ls /sys/class | grep leds
	echo
	ls /dev | grep leds
	echo
	cat /proc/devices | grep leds
	echo
	sudo cat /dev/leds
	echo
	dmesg | tail
	sudo ./test ledstate
	sudo ./test ledstate 0
	sudo ./test ledstate 1
	sudo ./test ledstate 2
	sudo ./test ledstate 3
	sudo ./test ledstate 4
	sudo ./test ledstate 5
	sudo ./test ledstate 6
	sudo ./test ledstate 7
	sudo ./test on 0
	sudo ./test on 1
	sudo ./test on 2
	sudo ./test on 3
	sudo ./test on 4
	sudo ./test on 5
	sudo ./test on 6
	sudo ./test on 7
	sudo ./test off 0
	sudo ./test off 1
	sudo ./test off 2
	sudo ./test off 3
	sudo ./test off 4
	sudo ./test off 5
	sudo ./test off 6
	sudo ./test off 7
	sudo ./test reset
	sudo ./test

delete:
	sudo rmmod leds
	rm test.o test
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif