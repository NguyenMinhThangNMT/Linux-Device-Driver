#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include<linux/string.h>

#define BASE_GPIO_ADRESS     0x3F200000
static struct proc_dir_entry *driver_proc = NULL;
static unsigned int *gpio_register = NULL;

static void gpio_on(void)
{
    unsigned int *gpiosel =(unsigned int*)((char*)gpio_register+0x08);
    unsigned int* gpio_on_register =(unsigned int*)((char*)gpio_register+0x1C);
    *gpiosel =0x01;
    *gpio_on_register |= (1<<20);
}

static void gpio_off(void)
{
    unsigned int* gpio_off_register =(unsigned int*)((char*)gpio_register+0x28);
    *gpio_off_register |= (1<<20);
}
// Ham open file
static int driver_open(struct inode *device_file, struct file *instance) {
    printk("device open was called\n");
    return 0;
}

// Ham close file
static int driver_close(struct inode *device_file, struct file *instance) {
    printk("device close was called\n");
    return 0;
}

ssize_t driver_read(struct file *file, char __user *user, size_t size, loff_t *off)
{
    return copy_to_user(user, "Hello!\n", 7) ? 0 : 7;
}

ssize_t driver_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *off)
{
    char user_data[10];
    memset(user_data, 0x0, 10);
    if (copy_from_user(user_data, user_buffer, size)) 
        return 0;
 
    printk("user write : %s", user_data);
    
    if(strncmp(user_data,"on",2)==0){
        printk("write on");
        gpio_on();
    }
    if(strncmp(user_data,"off",3)==0){
        printk("write off");
        gpio_off();
    }
    return size;

}

static const struct proc_ops driver_proc_fops = 
{
    .proc_open = driver_open,
	.proc_release = driver_close,
    .proc_read = driver_read,
    .proc_write = driver_write,
};

static int my_driver_init(void)
{
    printk("Hello Kernel\n");
    gpio_register=(int*)ioremap(BASE_GPIO_ADRESS,PAGE_SIZE);
    if(gpio_register==NULL){
        printk("Failed to map gpio\n");
    }
    // create an entry in the proc-fs
    driver_proc = proc_create("gpio", 0666, NULL, &driver_proc_fops);
    if (driver_proc == NULL)
    {
        return -1;
    }
    gpio_on();

    return 0;
}

static void my_driver_exit(void)
{
    printk("Goodbye Kernel!\n");
    proc_remove(driver_proc);
    return;
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_VERSION("1.0");
