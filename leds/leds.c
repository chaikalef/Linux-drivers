#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	#include <linux/signal.h>
#else
	#include <linux/sched/signal.h>
#endif

#define HELLO_MAJOR 250

#define LEDCMD_RESET_STATE _IO(HELLO_MAJOR, 1)
#define LEDCMD_GET_STATE _IOR(HELLO_MAJOR, 2, unsigned char *)
#define LEDCMD_GET_LED_STATE _IOWR(HELLO_MAJOR, 3,  led_t *)
#define LEDCMD_SET_LED_STATE _IOW(HELLO_MAJOR, 4, led_t *)

/* define name of the drivers */
#define FILENAME "/dev/leds"

/* initial state of LEDs */
#define INITIAL_STATE 0x00

#define BP "%d%d%d%d%d%d%d%d"
#define BB(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)


typedef struct led {
    int pin;
    unsigned char value;
} led_t;

#define PARALLEL_PORT 0x378
#define EOK 0

#define PORT_EMULATION

#ifdef PORT_EMULATION
#define inb(port) prn_data_em
#define outb(data, port) prn_data_em = ((unsigned char)data);
unsigned char prn_data_em;
#endif

#define MAX_MESSAGE_LENGTH 65536
#define START_MINOR_CODE 0
#define ALLOC_MINOR 1
#define UNREG_DEV 1

char message[MAX_MESSAGE_LENGTH];
int count;

static dev_t dev;
static struct cdev c_dev;
static struct class * cl;

static int leds_open(struct inode *inode, struct file *file);
static int leds_close(struct inode *inode, struct file *file);
static ssize_t leds_read(struct file *filp, char *buffer,
                       size_t length, loff_t * offset);
static ssize_t leds_write(struct file *filp, const char *buff,
                        size_t len, loff_t * off);

static long leds_ioctl(struct file *f, unsigned int cmd,
                        unsigned long arg);

static long leds_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int retval = -EINVAL;
    led_t led;

    unsigned char state;
    char mask = 0xff;
    int ret;

    switch (cmd) {

        case LEDCMD_GET_LED_STATE:

            if (copy_from_user(&led, (led_t *)arg, sizeof(led_t))) {
                retval = -EACCES;
            }

            else if ((led.pin < 0) || (led.pin > 7))
                retval = -EINVAL;
            else {

                led.value = (inb(PARALLEL_PORT) >> led.pin) & 1;

                ret = copy_to_user((led_t *)arg, &led, sizeof(led_t));

                if(ret)
                    retval = -EACCES;
                else
                    retval = EOK;
            }
            break;

        case LEDCMD_GET_STATE:

            state = inb(PARALLEL_PORT);

            if(copy_to_user((unsigned char *)arg, &state, sizeof(state)))
                retval = -EACCES;
            else
                retval = EOK;
            break;

        case LEDCMD_RESET_STATE:
            outb(INITIAL_STATE, PARALLEL_PORT);
            retval = EOK;
            break;

        case LEDCMD_SET_LED_STATE:
            if (copy_from_user(&led, (led_t *)arg, sizeof(led_t)))
                retval = -EACCES;
            else
                if ((led.pin < 0) || (led.pin > 7))
                    retval = -EINVAL;
                else {
                    state = inb(PARALLEL_PORT);

                    /* mask = 0b00010000 */
                    mask = (1 << led.pin);

                    state &= ~ mask;

                    mask = (led.value << led.pin);

                    state |= mask;

                    outb(state, PARALLEL_PORT);
                    retval = EOK;
                }
            break;

        default:
            printk("Bad ioctl command!\n");
            break;
    }

    return retval;
}

static int leds_open(struct inode *inode, struct file *file) {
    return EOK;
}

static ssize_t leds_read(struct file *filp, char *buffer,
                       size_t length, loff_t * offset) {
    return EOK;
}

static ssize_t leds_write(struct file *filp, const char *buff,
                        size_t len, loff_t * off) {
        return -EINVAL;
}

static int leds_close(struct inode *inode, struct file *file) {
    return EOK;
}

static struct file_operations leds_fops = {
    .owner = THIS_MODULE,
    .open = leds_open,
    .release = leds_close,
    .read = leds_read,
    .write = leds_write,
    .unlocked_ioctl = leds_ioctl
};

static int __init leds_init(void) {
    int retval;
    bool allocated = false;
    bool created = false;
    cl = NULL;

    retval = alloc_chrdev_region(&dev, START_MINOR_CODE,
                                 ALLOC_MINOR, "leds");
    if (retval)
        goto err;

    allocated = true;
    printk(KERN_INFO "Major number = %d Minor number = %d\n",
            MAJOR(dev), MINOR(dev));
    cl = class_create(THIS_MODULE, "leds");
    if (!cl) {
        retval = -1;
        goto err;
    }

    if (device_create(cl, NULL, dev, NULL, "leds") == NULL) {
        retval = -1;
        goto err;
    }

    created = true;
    cdev_init(&c_dev, &leds_fops);
    retval = cdev_add(&c_dev, dev, 1);
    if (retval)
        goto err;

    printk(KERN_INFO "Leds: regisered\n");
    return EOK;

err:
    printk(KERN_INFO "Leds: initialization failed with code %d\n",
           retval);
    if (created)
        device_destroy(cl, dev);

    if (allocated)
        unregister_chrdev_region(dev, UNREG_DEV);

    if (cl)
        class_destroy(cl);

    return retval;
}


static void __exit leds_exit(void) {
    printk(KERN_INFO "Leds: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, UNREG_DEV);
    class_destroy (cl);
}

module_init(leds_init);
module_exit(leds_exit);

MODULE_LICENSE("AGPL");
MODULE_AUTHOR("Sergey Chaika");
MODULE_DESCRIPTION("Simple loadable kernel module");
