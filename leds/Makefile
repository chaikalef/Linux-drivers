# Constanst definition
CC     = gcc
FLAGS = -g -Wall -Wstrict-prototypes -ansi -pedantic


# Find out the magic with KERNELRELEASE and right way to compile
# several binaries at once

ifneq (${KERNELRELEASE},)
	obj-m := hello.o
else
	KERNEL_SOURCE := /usr/src/linux-headers-3.19.0-25-generic
	PWD := $(shell pwd)
	
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
	rm test.o test

install:
	sudo insmod hello.ko
	gcc test.o -o test

test.o: test.c
	gcc -c test.c

check:
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
	sudo rmmod hello

see:
	lsmod | head -10 | grep hello
	echo
	ls /sys/class | grep teach_devices
	echo
	ls /dev | grep hello
	echo
	cat /proc/devices | grep hello
	echo
	sudo cat /dev/hello
	echo
	dmesg | tail
endif

