/* hello.c – Простой загружаемый модуль ядра */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>

static void print_process_info(void){
    struct task_struct *task;
    for_each_process(task) {
        printk("Task %s (pid = %d)\n", task->comm, task->pid);
    }
}


static int __init hello_init(void) /* Инициализация */
{
    printk(KERN_INFO "Hello: registered\n");
    print_process_info();
    return 0;
}

static void __exit hello_exit(void) /* Деинициализаия */
{
    printk(KERN_INFO "Hello: unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Sidyakin");
MODULE_DESCRIPTION("Simple loadable kernel module");
