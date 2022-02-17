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
	
	retval = register_chrdev(MAJOR_NUMBER, "dev_number", &fops);	/* major device number, name, file_operations to associate with */
	if (retval == 0)	// if the major device number is free
		printk("dev_number - registered device number MAJOR: %d, MINOR: %d\n", MAJOR_NUMBER, 0);
	else if (retval > 0)
		printk("dev_number - registered device number MAJOR: %d, MINOR: %d\n", retval>>20, retval&0xFFFFF);
	else {
		printk("could not register device number");
		return -1;
	}

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	unregister_chrdev(MAJOR_NUMBER, "dev_number");
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
