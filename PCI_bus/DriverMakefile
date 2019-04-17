# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
	obj-m := PCI_bus.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
	KERNEL_SOURCE := /usr/src/linux-headers-4.18.0-18-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	sudo insmod PCI_bus.ko

check:
	lsmod | grep PCI_bus
	echo
	ls /sys/class | grep PCI_buses
	echo
	ls /dev | grep PCI_bus
	echo
	cat /proc/devices | grep PCI_bus
	echo
	sudo cat /dev/PCI_bus
	echo
	dmesg | tail

delete:
	sudo rmmod PCI_bus
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif