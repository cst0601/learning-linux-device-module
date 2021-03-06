#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "task_example.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shao-Tse -Chikuma Chien");
MODULE_DESCRIPTION("task_struct example");

int counter;


/**
 * @brief This function is called when the device file is opened
 * Name of the function could be named to any name preferred, but the arguments should be the same.
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("task_example - open was called\n");
	return 0;
}

/**
 * @brief This function is called when the device file is closed 
 * Name of the function could be named to any name preferred, but the arguments should be the same.
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("task_example - close was called\n");
	return 0;
}

int32_t answer = 42;

void getTestData(struct TestData* data, const void* __user u_list) {
	printk("n=%d\n", data->n);
	data->list = kmalloc(data->n * sizeof(int), GFP_KERNEL);
	copy_from_user(data->list, u_list, data->n * sizeof(int));

	size_t i;
	for (i = 0; i < data->n; i++) {
		printk("element %d in list: %d\n", i, data->list[i]);
	}
	printk("counter: %d\n", counter);
	counter++;

	kfree(data->list);
}

void getCallerProcessInfo(void) {
	struct task_struct *p = current;

	printk("task_example - Caller process PID=%d\n", p->pid);
}

static long int my_ioctl(struct file *filp, unsigned cmd, unsigned long args) {
	struct mystruct test;
	struct TestData data;
	struct TestData* __user u_data = args;
	switch (cmd) {
		case WR_VALUE:
			if (copy_from_user(&answer, (int32_t *)args, sizeof(answer)))
				printk("task_example - Error copying data from user\n");
			else printk("task_example - Update the answer to %d\n", answer);
			break;
		case RD_VALUE:
			if (copy_to_user(&answer, (int32_t *)args, sizeof(answer)))
				printk("task_example - Error copying data from user\n");
			else printk("task_example - The answer was copied %d\n", answer);
			break;
		case GREETER:
			if (copy_from_user(&test, (int32_t *)args, sizeof(test)))
				printk("task_example - Error copying data from user\n");
			else printk("task_example - %d greets to %s\n", test.repeat, test.name);
			break;
		case READ:
			if (copy_from_user(&data, u_data, sizeof(struct TestData))) {
				printk("task_example - error copying test data from user\n");
			}
			else getTestData(&data, u_data->list);
			break;
		case INFO:
			getCallerProcessInfo();
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
	counter = 0;
	printk("Hello, Kernel!\n");
	/* register device number */
	
	retval = register_chrdev(MAJOR_NUMBER, "task_example", &fops);	/* major device number, name, file_operations to associate with */
	if (retval == 0)	// if the major device number is free
		printk("task_example - registered device number MAJOR: %d, MINOR: %d\n", MAJOR_NUMBER, 0);
	else if (retval > 0)
		printk("task_example - registered device number MAJOR: %d, MINOR: %d\n", retval>>20, retval&0xFFFFF);
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
	unregister_chrdev(MAJOR_NUMBER, "task_example");
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
