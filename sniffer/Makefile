# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
	obj-m := sniffer.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
	KERNEL_SOURCE := /usr/src/linux-headers-4.18.0-18-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	sudo insmod sniffer.ko

check:
	lsmod | grep sniffer
	echo
	ls /sys/class | grep sniffers
	echo
	ls /dev | grep sniffer
	echo
	cat /proc/devices | grep sniffer
	echo
	sudo cat /dev/sniffer
	echo
	dmesg | tail

delete:
	sudo rmmod sniffer
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif