/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fcntl.h>

#include <linux/fs.h> //For file_operations
#include <linux/cdev.h> //For cdevs
#include <linux/device.h> //For device
#include <linux/uaccess.h> //copy_to_user
#include <asm/io.h>
#include <linux/ioport.h>

#include "efm32gg.h"

#define DRIVER_NAME "GPIO_buttons"
#define DEV_NR_COUNT 1 

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int gamepad_open(struct inode *inode, struct file *filp);
ssize_t gamepad_read(struct file *filp, char *buff, size_t count, loff_t *offp);

static int gamepad_write(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);

/* Static variables */
static dev_t device_nr;
struct cdev gamepad_cdev;
struct fasync_struct* async_queue;
struct class* cl;


static struct file_operations game_fops = { 	//As in LDD3, ch3, p53
	.owner = THIS_MODULE, 			//found in module.h
	.open = gamepad_open,
	.read = gamepad_read,
	.write = gamepad_write,
	.release = gamepad_release,
};



static int __init gamepad_init(void)
{
printk(KERN_ALERT "Attempting to load gamepad driver module\n");

    int result;

    /* Dynamically allocate device numbers */
    result = alloc_chrdev_region(&device_nr, 0, DEV_NR_COUNT, DRIVER_NAME);

    if (result < 0) {
        printk(KERN_ALERT "Failed to allocate device numbers\n");
        return -1;
    }

      if (request_mem_region(GPIO_PC_MODEL, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_MODEL memory region, already in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DOUT, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DOUT memory region, already in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DIN, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DIN memory region, already in use?\n");
        return -1;
	}

	iowrite32(0x33333333, GPIO_PC_MODEL);
    iowrite32(0xFF, GPIO_PC_DOUT);
	iowrite32(0x22222222, GPIO_EXTIPSELL);


    /* add device */
    cdev_init(&gamepad_cdev, &game_fops);
    gamepad_cdev.owner = THIS_MODULE;
    cdev_add(&gamepad_cdev, device_nr, DEV_NR_COUNT);
    cl = class_create(THIS_MODULE, DRIVER_NAME);
    device_create(cl, NULL, device_nr, NULL, DRIVER_NAME);

    printk(KERN_INFO "Gamepad driver loaded.\n");

	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit gamepad_cleanup(void)
{
	 printk("Short life for a small module...\n");
}


static int gamepad_open(struct inode *inode, struct file *filp){
	printk(KERN_INFO "open");
	return 0;
}

ssize_t gamepad_read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
       unsigned long ret;
       printk("Inside read \n");
       uint32_t data = ioread32(GPIO_PC_DIN);
       ret = copy_to_user(buff, &data, 1);
       return ret;
}

static int gamepad_write(struct inode *inode, struct file *filp){
	printk(KERN_INFO "write");
	return 0;
}

static int gamepad_release(struct inode *inode, struct file *filp){
	printk(KERN_INFO "release");
	return 0;
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

