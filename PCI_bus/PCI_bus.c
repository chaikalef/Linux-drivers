#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h> 
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/pci.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	#include <linux/signal.h>
#else
	#include <linux/sched/signal.h>
#endif

#define MAX_MESSAGE_LENGTH 10024

static char message[MAX_MESSAGE_LENGTH];
static int count;

static dev_t dev;
static struct cdev c_dev;
static struct class * cl;

static int my_open(struct inode *inode, struct file *file);
static int my_close(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char *buffer,
					   size_t length, loff_t * offset);
static ssize_t my_write(struct file *filp, const char *buff,
						size_t len, loff_t * off);


static struct file_operations PCI_bus_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init PCI_bus_init(void) /* Инициализация */
{
	int retval;
	bool allocated = false;
	bool created = false;
	cl = NULL;

	retval = alloc_chrdev_region(&dev, 0, 1, "PCI_bus");
	if (retval)
		goto err;

	allocated = true;
	printk(KERN_INFO "Major number = %d Minor number = %d\n",
	 	   MAJOR(dev), MINOR(dev));
	

	cl = class_create(THIS_MODULE, "PCI_buses");
	if (!cl) {
		retval = -1;
		goto err;
	}

	if (device_create(cl, NULL, dev, NULL, "PCI_bus") == NULL) {
		retval = -1;
		goto err;
	}
	created = true;

	cdev_init(&c_dev, &PCI_bus_fops);

	retval = cdev_add(&c_dev, dev, 1);
	if (retval)
		goto err;

	printk(KERN_INFO "PCI_bus: registered");
	return 0;

err:
	printk("PCI_bus: initialization failed with code %08x\n", retval);

	if (created)
		device_destroy(cl, dev);

	if (allocated)
		unregister_chrdev_region(dev, 1);

	if (cl)
		class_destroy(cl);

	return retval;
}

static int my_open(struct inode *inode, struct file *file)
{
	/*
	 * The VFS inode data structure holds 
	 * information about a file or directory on disk.
	 */
	struct pci_dev* pdev = NULL;
	const char * name;
	unsigned short vendor;
	unsigned short device;
	unsigned int class;

	for_each_pci_dev(pdev) /* цикл по всем устройствам */
	{
		int i = 0;
		int info_len;
		char info[100];

		/* Переменная pdev содержит указатель 
		 * на структуру pci_dev текущего устройства
		 */
		name = pci_name(pdev);	/* номер шины устройства */
		vendor = pdev->vendor;
		device = pdev->device;
		class = pdev->class; 

		sprintf(info, "%s %x %x %x\n",
				name, vendor, device, class);
        /* len - фактическая длина info(без нулей) */
		info_len = strlen(info);

		while (i < info_len && count < MAX_MESSAGE_LENGTH)
			message[count++] = info[i++];

    }
	count = 0;
	return 0;
}

static int my_close(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t my_read(struct file *filp,  
					   char *buffer, /* буфер данных */
					   size_t length, /* длина буфера */
					   loff_t * offset)
{

	if (count <= MAX_MESSAGE_LENGTH) {
		if (message[count] == 0) {
			count = 0;
			return 0;
		}

		if (copy_to_user(buffer, &message[count], sizeof(char)))
			return -EFAULT;
		else {
			count++;
			return sizeof(char);
		}
	}
	count = 0;
	return 0;
}

static ssize_t my_write(struct file *filp, const char *buff,
						size_t len, loff_t * off)
{
	//#define EINVAL          22      /* Invalid argument */
    return -EINVAL;
}


static void __exit PCI_bus_exit(void) /* Деинициализаия */
{
    printk(KERN_INFO "PCI_bus: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, 1);
    class_destroy (cl);
}

module_init(PCI_bus_init);
module_exit(PCI_bus_exit);

MODULE_LICENSE("AGPL");
MODULE_AUTHOR("Sergey Chaika");
MODULE_DESCRIPTION("Simple loadable kernel module");
