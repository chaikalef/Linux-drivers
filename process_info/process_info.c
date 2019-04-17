#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	#include <linux/signal.h>
#else
	#include <linux/sched/signal.h>
#endif

static void print_process_info(void){
    struct task_struct *task;
    for_each_process(task) {
        printk("Task %s (pid = %d)\n", task->comm, task->pid);
    }
}


static int __init process_info_init(void) /* Инициализация */
{
    printk(KERN_INFO "Process_info: registered\n");
    print_process_info();
    return 0;
}

static void __exit process_info_exit(void) /* Деинициализаия */
{
    printk(KERN_INFO "Process_info: unregistered\n");
}

module_init(process_info_init);
module_exit(process_info_exit);

MODULE_LICENSE("AGPL");
MODULE_AUTHOR("Sergey Chaika");
MODULE_DESCRIPTION("Simple loadable kernel module");
