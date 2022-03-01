// SPDX-License-Identifier: GPL-3.0
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include <linux/list.h>
#include <linux/module.h>    // module_{init,exit}, MODULE_*
#include <linux/string.h>    // strcpy, strcmp
#include <linux/types.h>     // u32 etc.

#include "linked_list_example.h"

/* Meta information */
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("Example of using hash table in linux kernel");
MODULE_AUTHOR("Shao-Tse, Chien");
MODULE_LICENSE("GPL");

// entry of the linked list
struct mystruct {
    pid_t pid;
    int32_t data;
    struct list_head node;
};

//struct hlist_node head;     // head of the linked list
LIST_HEAD(my_linked_list);

void list_elements(void) {
    printk("linked_list_example: list all elements in linkedlist\n");
    struct list_head* position = NULL;
    struct mystruct* current_node = NULL;

    list_for_each(position, &my_linked_list) {
        current_node = list_entry(position, struct mystruct, node);
        printk("linked_list_example: Iterating the linked list, data=%d\n",
               current_node->data);
    }
}

void add_element_to_linkedlist(pid_t pid, int32_t data) 
{
    struct mystruct* new_node = kmalloc(sizeof(struct mystruct), GFP_KERNEL);
    new_node->pid = pid;
    new_node->data = data;
    INIT_LIST_HEAD(&new_node->node);     // init the node pointer structure
    list_add_rcu(&new_node->node, &my_linked_list);
}

void remove_element_from_linkedlist(int32_t data)
{
    printk("linked_list_example: removing all nodes with data=%d\n", data);
    struct list_head* position = NULL;
    struct list_head* n = NULL;
    struct mystruct* current_node = NULL;

    list_for_each_safe(position, n, &my_linked_list) {
        current_node = list_entry(position, struct mystruct, node);
        if (current_node->data == data) {
            printk("linked_list_example: node with data=%d found, delete\n", data);
            list_del(position);
            kfree(current_node);
        }
    }
}

void remove_all_element_from_linkedlist(void) {
    printk("linked_list_example: removing all nodes\n");
    struct list_head* position = NULL;
    struct list_head* n = NULL;
    struct mystruct* current_node = NULL;

    list_for_each_safe(position, n, &my_linked_list) {
        current_node = list_entry(position, struct mystruct, node);
        list_del(position);
        kfree(current_node);
    }
}

static long int ioctl_call(struct file *filp, unsigned cmd, unsigned long args)
{
    struct task_struct *p = current;
    int data;
    switch (cmd) {
        case LIST:
            list_elements();
        break;
        case INSERT: 
            if (copy_from_user(&data, (int32_t *)args, sizeof(int32_t))) {
                printk("linked_list_example: Error copying data from user\n");
            }
            else {
                printk("linked_list_example: Caller process PID=%d, Copied data "
                       "%d from user\n", p->pid, data);
                add_element_to_linkedlist(p->pid, data);
            }
        break;
        case REMOVE:
            if (copy_from_user(&data, (int32_t *)args, sizeof(int32_t))) {
                printk("linked_list_example: Error copying data from user\n");
            }
            else {
                remove_element_from_linkedlist(data);    
            }
            
        break;
    }
    return 0;
} 

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = ioctl_call
};

static int __init hash_table_init(void)
{
    int retval;
    printk("linked_list_example: module loaded\n");
    
    // initialize the linked list
    

    // major device number, name, file_operations to associate with 
    retval = register_chrdev(MAJOR_NUMBER, "linked_list_example", &fops);
	if (retval == 0)	                   // if the major device number is free
		printk("linked_list_example: registered device number MAJOR: %d, MINOR: "
               "%d\n", MAJOR_NUMBER, 0);
	else if (retval > 0)
		printk("linked_list_example: registered device number MAJOR: %d, MINOR: "
               "%d\n", retval>>20, retval&0xFFFFF);
	else {
		printk("linked_list_example: could not register device number");
		return -1;
	}

    return 0;
}

static void __exit hash_table_exit(void)
{
    remove_all_element_from_linkedlist();
    unregister_chrdev(MAJOR_NUMBER, "linked_list_example");
    printk("linked_list_example: module unloaded\n");
}

module_init(hash_table_init);
module_exit(hash_table_exit);