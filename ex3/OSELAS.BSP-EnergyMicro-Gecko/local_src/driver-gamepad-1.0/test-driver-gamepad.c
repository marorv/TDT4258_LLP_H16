/*
 * This is a demo Linux kernel module.
 */
//Use Embedded Operating Systems: A Practical Approach for reference

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fcntl.h>

#include <linux/fs.h> //For file_operations

static int __init gamepad_init(void);
static void __exit gamepad_cleanup(void);
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_read(struct inode *inode, struct file *filp);
static int gamepad_write(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);

MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION("Small module, demo only, not very useful.");

module_init(gamepad_init);
module_exit(gamepad_cleanup);

/* file pointer */
static struct file_operations game_fops = { 	//As in LDD3, ch3, p53
	.owner = THIS_MODULE, 			//found in module.h
	.open = gamepad_open,
	.read = gamepad_read,
	.write = gamepad_write,
	.release = gamepad_release,
};

static int gamepad_open(struct inode *inode, struct file *filp){
	printk(KERN_INFO "open");
	return 0;
}

static int gamepad_read(struct inode *inode, struct file *filp){
	printk(KERN_INFO "read");
	return 0;
}

static int gamepad_write(struct inode *inode, struct file *filp){
	printk(KERN_INFO "write");
	return 0;
}

static int gamepad_release(struct inode *inode, struct file *filp){
	printk(KERN_INFO "release");
	return 0;
}

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init gamepad_init(void)
{
	printk("Hello World, here is your module speaking\n");
	//int k = O_RDWR;
	//int whatever = sys_open("/dev/fb0", 0x0002);
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
