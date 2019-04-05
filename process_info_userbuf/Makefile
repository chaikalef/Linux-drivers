# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
	obj-m := hello.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
	KERNEL_SOURCE := /usr/src/linux-headers-3.19.0-25-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	sudo insmod hello.ko

check:
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

delete:
	sudo rmmod hello
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif