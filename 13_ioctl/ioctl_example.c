#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>

#include "ioctl_example.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shao-Tse -Chikuma Chien");
MODULE_DESCRIPTION("IOCTL example");


/**
 * @brief This function is called when the device file is opened
 * Name of the function could be named to any name preferred, but the arguments should be the same.
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("ioctl_example - open was called\n");
	return 0;
}

/**
 * @brief This function is called when the device file is closed 
 * Name of the function could be named to any name preferred, but the arguments should be the same.
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("ioctl_example - close was called\n");
	return 0;
}

int32_t answer = 42;

static long int my_ioctl(struct file *filp, unsigned cmd, unsigned long args) {
	struct mystruct test;
	switch (cmd) {
		case WR_VALUE:
			if (copy_from_user(&answer, (int32_t *)args, sizeof(answer)))
				printk("ioctl_example - Error copying data from user\n");
			else printk("icotl_example - Update the answer to %d\n", answer);
			break;
		case RD_VALUE:
			if (copy_to_user(&answer, (int32_t *)args, sizeof(answer)))
				printk("ioctl_example - Error copying data from user\n");
			else printk("icotl_example - The answer was copied %d\n", answer);
			break;
		case GREETER:
			if (copy_from_user(&test, (int32_t *)args, sizeof(test)))
				printk("ioctl_example - Error copying data from user\n");
			else printk("icotl_example - %d greets to %s\n", test.repeat, test.name);
			break;
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.unlocked_ioctl = my_ioctl	
};

#define MAJOR_NUMBER 90

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	int retval;

	printk("Hello, Kernel!\n");
	/* register device number */
	
	retval = register_chrdev(MAJOR_NUMBER, "ioctl_example", &fops);	/* major device number, name, file_operations to associate with */
	if (retval == 0)	// if the major device number is free
		printk("ioctl_example - registered device number MAJOR: %d, MINOR: %d\n", MAJOR_NUMBER, 0);
	else if (retval > 0)
		printk("ioctl_example - registered device number MAJOR: %d, MINOR: %d\n", retval>>20, retval&0xFFFFF);
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
	unregister_chrdev(MAJOR_NUMBER, "ioctl_example");
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
