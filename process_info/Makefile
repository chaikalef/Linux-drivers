# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
	obj-m := process_info.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
	KERNEL_SOURCE := /usr/src/linux-headers-4.18.0-18-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	sudo insmod process_info.ko

check:
	lsmod | grep process_info
	echo
	ls /dev | grep process_info
	echo
	cat /proc/devices | grep process_info
	echo
	sudo cat /dev/process_info
	echo
	dmesg | tail

delete:
	sudo rmmod process_info
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif