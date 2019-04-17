# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
	obj-m := process_info_userbuf.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
	KERNEL_SOURCE := /usr/src/linux-headers-4.18.0-18-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	sudo insmod process_info_userbuf.ko

check:
	lsmod | grep process_info_userbuf
	echo
	ls /sys/class | grep process_info_userbuf
	echo
	ls /dev | grep process_info_userbuf
	echo
	cat /proc/devices | grep process_info_userbuf
	echo
	sudo cat /dev/process_info_userbuf
	echo
	dmesg | tail

delete:
	sudo rmmod process_info_userbuf
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif