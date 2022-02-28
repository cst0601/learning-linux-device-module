// SPDX-License-Identifier: GPL-3.0
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include <linux/hashtable.h> // hashtable API
#include <linux/module.h>    // module_{init,exit}, MODULE_*
#include <linux/string.h>    // strcpy, strcmp
#include <linux/types.h>     // u32 etc.

#include "hash_table_example.h"

/* Meta information */
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("Example of using hash table in linux kernel");
MODULE_AUTHOR("Shao-Tse, Chien");
MODULE_LICENSE("GPL");

// entry of the hash table
struct entry {
    pid_t pid;
    int data;
    struct hlist_node node;
};

// Declaration of a hash table named user_prog_ht with 2^3 buckets
DECLARE_HASHTABLE(user_prog_ht, 3); 

// Just to demonstrate the behavior when two keys are equal.
static u32 myhash(const char *s) {
    u32 key = 0;
    char c;

    while ((c = *s++))
        key += c;

    return key;
}

void list_element_in_hashtable(void) {
    printk("hash_table_example: list all elements in hashtable\n");
    int32_t bkt;            // integer to use as bucket loop cursor
    struct entry* cur;      // the type * to use as a loop cursor for each entry
    hash_for_each_rcu(user_prog_ht, bkt, cur, node) {
        printk("hash_table_example: element: data = %d, pid = %d\n",
               cur->data, cur->pid);
    }
}

void add_element_to_hashtable(pid_t pid, int32_t data) 
{
    struct entry* new_node = kmalloc(sizeof(struct entry), GFP_KERNEL);
    new_node->data = data;
    new_node->pid = pid;

    // added to hash table, using pid as key.
    hash_add_rcu(user_prog_ht, &(new_node->node), pid);
}

void remove_element_from_hashtable(pid_t pid) {
    int32_t bkt;
    struct entry* cur;
    hash_for_each_possible_rcu(user_prog_ht, cur, node, pid) {
        printk("hash_table_example: match for key %u: data = %d, remove\n",
               pid, cur->data);

        // delete the entry, probably cause memory leak for the hash table implmented in linux
        hash_del_rcu(&(cur->node));
    }
}

static long int ioctl_call(struct file *filp, unsigned cmd, unsigned long args)
{
    struct task_struct *p = current;
    int data;
    switch (cmd) {
        case LIST:
            list_element_in_hashtable();
        break;
        case INSERT: 
            if (copy_from_user(&data, (int32_t *)args, sizeof(int32_t))) {
                printk("hash_table_example: Error copying data from user\n");
            }
            else {
                printk("hash_table_example: Caller process PID=%d, Copied data "
                       "%d from user\n", p->pid, data);
                add_element_to_hashtable(p->pid, data);
            }
        break;
        case REMOVE:
            remove_element_from_hashtable(p->pid);
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
    printk("hash_table_example: module loaded\n");
    
    // initialize the hash table
    hash_init(user_prog_ht);

    // major device number, name, file_operations to associate with 
    retval = register_chrdev(MAJOR_NUMBER, "hash_table_example", &fops);
	if (retval == 0)	                   // if the major device number is free
		printk("hash_table_example: registered device number MAJOR: %d, MINOR: "
               "%d\n", MAJOR_NUMBER, 0);
	else if (retval > 0)
		printk("hash_table_example: registered device number MAJOR: %d, MINOR: "
               "%d\n", retval>>20, retval&0xFFFFF);
	else {
		printk("hash_table_example: could not register device number");
		return -1;
	}

    return 0;
}

static void __exit hash_table_exit(void)
{
    unregister_chrdev(MAJOR_NUMBER, "hash_table_example");
    printk("hash_table_example: module unloaded\n");
}

module_init(hash_table_init);
module_exit(hash_table_exit);