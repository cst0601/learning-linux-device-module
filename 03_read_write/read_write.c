#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shao-Tse -Chikuma Chien");
MODULE_DESCRIPTION("Register a device number and implement some callback functions");

/* Buffer for data */
static char buffer[255];
static int buffer_pointer;

/* variables for device and device class */
static dev_t device_number;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "test_driver"
#define DRIVER_CLASS "test_module_class"

/**
 * @brief Read data out of the buffer
 */
static ssize_t driver_read(struct file *filp, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, buffer_pointer);	// size = count or 255, depends on which is smaller

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, buffer, to_copy);

	/* calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *filp, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));	// size = count or 255, depends on which is smaller

	/* Copy data to user */
	not_copied = copy_to_user(buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called when the device file is opened
 * Name of the function could be named to any name preferred, but the arguments should be the same.
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_number - open was called\n");
	return 0;
}

/**
 * @brief This function is called when the device file is closed 
 * Name of the function could be named to any name preferred, but the arguments should be the same.
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_number - close was called\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

#define MAJOR_NUMBER 90

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	int retval;

	printk("Hello, Kernel!\n");
	
	/* register device number */
	/* allocate a device number */
	if (alloc_chrdev_region(&device_number, 0, 1, DRIVER_NAME) < 0) {
		printk("Device number could not be allocated.\n");
		return -1;
	}
	printk("read_write - device number major: %d, minor: %d, is registered.\n", device_number >> 20, device_number && 0xFFFFF);

	/* create device class */
	if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("device class could not be created.\n");
		goto ClassError;
	}

	/* create device file */
	if (device_create(my_class, NULL, device_number, NULL, DRIVER_NAME) == NULL) {
		printk("cannot create device file\n");
		goto FileError;
	}

	/* initialize device file */
	cdev_init(&my_device, &fops);	// links with file operations

	/* registering device to kernel */
	if (cdev_add(&my_device, device_number, 1) == -1) {
		printk("registering of device to kernel failed\n");
		goto AddError;
	} 

	return 0;

	/* goto destinations, but with good intentions */
AddError:
	device_destroy(my_class, device_number);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev(device_number, DRIVER_NAME);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	cdev_del(&my_device);
	device_destroy(my_class, device_number);
	class_destroy(my_class);
	unregister_chrdev(device_number, DRIVER_NAME);

	unregister_chrdev(MAJOR_NUMBER, "dev_number");
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
