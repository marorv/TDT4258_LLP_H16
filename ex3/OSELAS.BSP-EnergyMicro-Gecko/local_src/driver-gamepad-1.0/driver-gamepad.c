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

#include <asm/signal.h>
#include <asm/siginfo.h>
#include <linux/interrupt.h>

#include "efm32gg.h"

#define DRIVER_NAME "GPIO_buttons"
#define DEV_NR_COUNT 1 

static int gamepad_open(struct inode *inode, struct file *filp);
static unsigned long gamepad_read(struct file *filp, uint16_t *buff, size_t count, loff_t *offp);

static int gamepad_write(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);

static irqreturn_t gpio_interrupt_handler(int, void*, struct pt_regs*);
static int gamepad_fasync(int, struct file*, int mode);

int result;

/* Static variables */
static dev_t device_nr;
struct cdev gamepad_cdev;
struct fasync_struct* async_queue;
struct class* cl;


static struct file_operations game_fops = {     //As in LDD3, ch3, p53
    .owner = THIS_MODULE,                       //found in module.h
    .open = gamepad_open,
    .read = gamepad_read,
    .write = gamepad_write,
    .release = gamepad_release,
    .fasync = gamepad_fasync,
};


//Interrupt handler
irqreturn_t gpio_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs)
{
    //printk(KERN_ALERT "Handling GPIO interrupt\n");
    iowrite32(ioread32(GPIO_IF), GPIO_IFC);
    if (async_queue) {
        kill_fasync(&async_queue, SIGIO, POLL_IN);
    }
    return IRQ_HANDLED;
}

//fasync function
static int gamepad_fasync(int fd, struct file* filp, int mode) 
{
    return fasync_helper(fd, filp, mode, &async_queue);
}

static int __init gamepad_init(void)
{
    printk(KERN_ALERT "Attempting to load gamepad driver module\n");

    /* Dynamically allocate device numbers */
    result = alloc_chrdev_region(&device_nr, 0, DEV_NR_COUNT, DRIVER_NAME);

    if (result < 0) {
        printk(KERN_ALERT "Could not allocate device numbers\n");
        return -1;
    }

    if (request_mem_region(GPIO_PC_MODEL, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Could not request GPIO_PC_MODEL memory region. Is it in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DOUT, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Could not request GPIO_PC_DOUT memory region. Is it in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DIN, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Could not request GPIO_PC_DIN memory region. Is it in use?\n");
        return -1;
    }

    iowrite32(0x33333333, GPIO_PC_MODEL);
    iowrite32(0xFF, GPIO_PC_DOUT);
    iowrite32(0x22222222, GPIO_EXTIPSELL);

    
    //Interrupt setup
    request_irq(GPIO_EVEN_IRQ_LINE, (irq_handler_t)gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);
    request_irq(GPIO_ODD_IRQ_LINE, (irq_handler_t)gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);
    

    //Add char device
    cdev_init(&gamepad_cdev, &game_fops);
    gamepad_cdev.owner = THIS_MODULE;
    cdev_add(&gamepad_cdev, device_nr, DEV_NR_COUNT);
    cl = class_create(THIS_MODULE, DRIVER_NAME);
    device_create(cl, NULL, device_nr, NULL, DRIVER_NAME);

    
    //Enable interrupts 
    iowrite32(0xFF, GPIO_EXTIFALL);
    iowrite32(0x00FF, GPIO_IEN);
    iowrite32(0xFF, GPIO_IFC);
    

    printk(KERN_INFO "Gamepad driver loaded.\n");

    return 0;
}

static void __exit gamepad_cleanup(void)
{
    printk("Exiting driver\n");

    //Disable interrupts
    iowrite32(0x0000, GPIO_IEN);

    //Free interrupt lines
    free_irq(GPIO_EVEN_IRQ_LINE, &gamepad_cdev);
    free_irq(GPIO_ODD_IRQ_LINE, &gamepad_cdev);

    //Release memory regions
    release_mem_region(GPIO_PC_MODEL, 1);
    release_mem_region(GPIO_PC_DIN, 1);
    release_mem_region(GPIO_PC_DOUT, 1);


    //Destroy device
    device_destroy(cl, device_nr);
    class_destroy(cl);
    cdev_del(&gamepad_cdev);
    
    //Disallocate device numbers
    unregister_chrdev_region(device_nr, DEV_NR_COUNT);

    printk("Buh bye.\n");
}


static int gamepad_open(struct inode *inode, struct file *filp){
    printk(KERN_INFO "open\n");
    return 0;
}

static unsigned long gamepad_read(struct file *filp, uint16_t *buff, size_t count, loff_t *offp)
{
   //printk("Inside read \n");
   uint32_t data = ioread32(GPIO_PC_DIN);
   return copy_to_user(buff, &data, 1);
}

static int gamepad_write(struct inode *inode, struct file *filp){
    printk(KERN_INFO "write\n");
    return 0;
}

static int gamepad_release(struct inode *inode, struct file *filp){
    printk(KERN_INFO "release\n");
    return 0;
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Gamepad driver for TDT4258 project, group 13.");
MODULE_LICENSE("GPL");
