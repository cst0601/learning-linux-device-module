#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shao-Tse -Chikuma Chien");
MODULE_DESCRIPTION("Register a device number and implement some callback functions");


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
	.release = driver_close
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
