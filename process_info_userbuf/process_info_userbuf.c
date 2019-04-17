#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/string.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	#include <linux/signal.h>
#else
	#include <linux/sched/signal.h>
#endif
 
#define MAX_MESSAGE_LENGTH 4096
char message [MAX_MESSAGE_LENGTH];
int count;

static dev_t dev;
static struct cdev c_dev;
static struct class * cl;

static int my_open(struct inode *inode, struct file *file);
static int my_close(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char *buffer, size_t length, loff_t * offset);
static ssize_t my_write(struct file *filp, const char *buff, size_t len, loff_t * off);

void show_proc_list(void) //Функция для вывода всех имеющихся процессов
{
   struct task_struct *task;
   for_each_process(task)
   {
	
    strcat(message, task->comm);
    strcat(message,"\n");
    
   }
}

static struct file_operations process_info_userbuf_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init process_info_userbuf_init(void) /* Инициализация */
{
	int retval;
	bool allocated = false;
	bool created = false;
	cl = NULL;

	retval = alloc_chrdev_region(&dev, 0, 1, "process_info_userbuf");
	if (retval)
		goto err;

	allocated = true;
	printk(KERN_INFO "Major number = %d Minor number = %d\n", MAJOR(dev), MINOR(dev));
	cl = class_create(THIS_MODULE, "process_info_userbuf");
	if (!cl) {
		retval = -1;
		goto err;
	}

	if (device_create(cl, NULL, dev, NULL, "process_info_userbuf") == NULL)
	{
		retval = -1;
		goto err;
	}

	created = true;
	cdev_init(&c_dev, &process_info_userbuf_fops);
	retval = cdev_add(&c_dev, dev, 1);
	if (retval)
		goto err;

	printk(KERN_INFO "Process_info_userbuf: regisered");
	return 0;

err:
	printk("Process_info_userbuf: initialization failed with code %08x\n", retval);
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
    show_proc_list();
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
    if(message[count]) {	

    char ch = message[count];
    count++;
    if (copy_to_user(buffer, &ch, sizeof(ch)))
		return -EFAULT;
	return sizeof(ch);
    } /* количество байт возвращаемых драйвером в буфере */
    else
        return 0;
}

static ssize_t my_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
        return -EINVAL;
}

static void __exit process_info_userbuf_exit(void) /* Деинициализаия */
{
    printk(KERN_INFO "Process_info_userbuf: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, 1);
    class_destroy (cl);
}

module_init(process_info_userbuf_init);
module_exit(process_info_userbuf_exit);

MODULE_LICENSE("AGPL");
MODULE_AUTHOR("Sergey Chaika");
MODULE_DESCRIPTION("Simple loadable kernel module");
