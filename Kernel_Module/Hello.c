#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/io.h>



static struct proc_dir_entry *driver_proc = NULL;

static int driver_open(struct inode *device_file,struct file *instance){
    printk("device open was called\n");
    return 0;
}

static int driver_close(struct inode *device_file,struct file *instance){
    printk("device close was called\n");
    return 0;
}

ssize_t driver_read(struct file *file,char __user *user,size_t size,loff_t *off){
    return copy_to_user(user,"Hello\n",7)?0:7;
}

ssize_t driver_write(struct file *file,const char __user *user_buffer,size_t size,loff_t *off){
    char user_data[10];
    memset(user_data ,0,10);
    if (copy_from_user(user_data, user_buffer, size)) 
        return 0;
 
    printk("user write : %s", user_data);
    return size;
}

static const struct proc_ops driver_proc_fops=
{
    .proc_open=driver_open,
    .proc_release=driver_close,
    .proc_read=driver_read,
    .proc_write=driver_write,

};

static int my_driver_init(void){
    printk("Hello Kernel \n");
    driver_proc=proc_create("mymodule",0666,NULL,&driver_proc_fops);
    if(driver_proc==NULL){
        return -1;
    }
    return 0;
}



static void my_driver_exit(void){
    printk("Goodbye Kernel \n");
    proc_remove(driver_proc);
    return ;
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_VERSION("1.0");
