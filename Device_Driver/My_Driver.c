#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

/* Thong tin */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("Character Device Driver");

/* Tao buffer cho data */
static char buffer[255];
static int buffer_pointer = 0;

/* Bien cau hinh character device driver  */
static dev_t my_device_nr; //So hieu cdev=major,minor
static struct class *my_class; //Con tro cua lop quan ly chua cdev  
static struct cdev my_device; //Bien quan li cdev

#define DRIVER_NAME "mydriver"
#define DRIVER_CLASS "MyModuleClass"


static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* So luong du lieu sao chep */
	to_copy = min(count, buffer_pointer);

	/* Copy data tu bufferlieu */
	delta = to_copy - not_copied;

	return delta;
}


static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* So luong du lieu sao chep */
	to_copy = min(count, sizeof(buffer));

	/* Copy data tu userbuffee vao buffer */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* Tinh toan du lieu */
	delta = to_copy - not_copied;

	return delta;
}


static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}


static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};


static int __init ModuleInit(void) {
	int retval;
	printk("Hello, Kernel!\n");

	/*Xin cap phat so hieu cho cdev */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr & 0xfffff);

	/* Tao ra class chua cdev */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* Tao ra file device driver */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Cau hinh cho driver */
	cdev_init(&my_device, &fops);

	/* Dang ki character device vao trong  */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	return 0;
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

static void __exit ModuleExit(void) {
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
