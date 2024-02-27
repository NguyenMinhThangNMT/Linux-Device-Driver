#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>


//Dinh nghia Deviece name
#define DRIVER_NAME "my_driver"
#define DRIVER_CLASS "MymoduleClass"

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

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
    memset(user_data, 0, 10);
    if (copy_from_user(user_data, user_buffer, size)) 
        return 0;
 
    printk("user write : %s", user_data);
    return size;
}

static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write,
};
static int my_driver_init(void)
{
    printk("Hello Device Driver\n");
    //Dang ki 
   if(alloc_chrdev_region(&my_device_nr,0,1,DRIVER_NAME)<0){
    printk("Device could not be allocated\n");
    return -1;
   }
   printk("my_driver : Major:%d,Minor :%d \n",my_device_nr>>20,my_device_nr&0xfffff);
   if((my_class = class_create (THIS_MODULE,DRIVER_CLASS))==NULL){
    printk("Device class can not be created");
    unregister_chrdev_region(my_device_nr,1);
    return -1;
   }
   if((device_create(my_class,NULL,my_device_nr,NULL,DRIVER_NAME))==NULL){
    printk("can not create device file");
    class_destroy(my_class);
    return -1;
   }
   cdev_init(&my_device, &my_fops);

   if(cdev_add(&my_device,my_device_nr,1)==1){
    printk("can not create device file");
    device_destroy(my_class,my_device_nr);
    return -1;
   }
    return 0;
}

static void my_driver_exit(void)
{
    printk("Goodbye Device Driver!\n");
    cdev_del(&my_device);
    device_destroy(my_class,my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr,1);
    return;
}

module_init(my_driver_init);
module_exit(my_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_VERSION("1.0");
