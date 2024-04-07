/*
In this lab, you are required to write a Linux device driver that can control a 16-segment display to display the characters from A to Z.
*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/version.h>

#include <linux/ctype.h>

// define the file operations
static int mydev_open(struct inode *inode, struct file *file);
static int mydev_release(struct inode *inode, struct file *file);
static ssize_t mydev_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t mydev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset);


static dev_t device_number = 0;
static struct cdev mydev_cdev;
static struct class *cls = NULL;
static struct device *dev = NULL;

// declare the file operations structure
static struct file_operations mydev_fops = {
    .owner = THIS_MODULE,
    .open = mydev_open,
    .release = mydev_release,
    .read = mydev_read,
    .write = mydev_write,
};

// define the 16-segment display
#define ALPHABET_SIZE 26
const static uint16_t alphabet_segments[ALPHABET_SIZE + 1] = {
    0b1111001100010001, // A
    0b0000011100000101, // b
    0b1100111100000000, // C
    0b0000011001000101, // d
    0b1000011100000001, // E
    0b1000001100000001, // F
    0b1001111100010000, // G
    0b0011001100010001, // H
    0b1100110001000100, // I
    0b1100010001000100, // J
    0b0000000001101100, // K
    0b0000111100000000, // L
    0b0011001110100000, // M
    0b0011001110001000, // N
    0b1111111100000000, // O
    0b1000001101000001, // P
    0b0111000001010000, // q
    0b1110001100011001, // R
    0b1101110100010001, // S
    0b1100000001000100, // T
    0b0011111100000000, // U
    0b0000001100100010, // V
    0b0011001100001010, // W
    0b0000000010101010, // X
    0b0000000010100100, // Y
    0b1100110000100010, // Z
    0b0000000000000000
};

// default display is off, with all segments off
static char str_segments[16] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

void set_segments(char c)
{
    int i;
    uint16_t segment_bits = alphabet_segments[ALPHABET_SIZE];

    if (isupper(c))
        segment_bits = alphabet_segments[c - 'A'];
    else if (islower(c))
        segment_bits = alphabet_segments[c - 'a'];
    else
        segment_bits = alphabet_segments[ALPHABET_SIZE];

    i = ARRAY_SIZE(str_segments);
    while (i--)
    {
        str_segments[i] = (segment_bits & 1) ? '1' : '0';
        segment_bits >>= 1;
    }
}

// define the file operations

static int mydev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydev: open()\n");
    return 0;
}

static int mydev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydev: release()\n");
    return 0;
}

static ssize_t mydev_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    printk(KERN_INFO "mydev: read()\n");

    len = len > 16 ? 16 : len;

    if (copy_to_user(buf, str_segments, len))
    {
        printk(KERN_ERR "mydev: failed to copy data to user space\n");
        return -EFAULT;
    }
    return len;
}

static ssize_t mydev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    char c;

    printk(KERN_INFO "mydev: write()\n");

    if (copy_from_user(&c, buf, 1))
    {
        printk(KERN_ERR "mydev: failed to copy data from user space\n");
        return -EFAULT;
    }

    set_segments(c);

    return len;
}

#define DEVICE_NAME "mydev"

static int __init mydev_init(void)
{
    int alloc_ret = -1;
    int cdev_ret = -1;
    printk(KERN_INFO "mydev: init()\n");
    
    // register a char device number
    alloc_ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);
    if (alloc_ret)
    {
        printk(KERN_ERR "mydev: failed to register a char device number\n");
        goto error;
    }

    // initialize a cdev structure
    cdev_init(&mydev_cdev, &mydev_fops);

    // add a char device to the system
    cdev_ret = cdev_add(&mydev_cdev, device_number, 1);
    if (cdev_ret)
    {
        printk(KERN_ERR "mydev: failed to add a char device to the system\n");
        goto error;
    }

    printk(KERN_INFO "mydev: registered with major number %d\n", MAJOR(device_number));
    
    // create a struct class structure
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(DEVICE_NAME);
#else
    cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif
    if (IS_ERR(cls))
    {
        printk(KERN_ERR "mydev: failed to create a struct class\n");
        goto error;
    }

    // creates a device and registers it with sysfs
    dev = device_create(cls, NULL, device_number, NULL, DEVICE_NAME);
    if (IS_ERR(dev))
    {
        printk(KERN_ERR "mydev: failed to create a device and register it with sysfs\n");
        goto error;
    }

    printk(KERN_INFO "mydev: created on /dev/%s\n", DEVICE_NAME);

    return 0;

error:
    if (!IS_ERR(dev))
        device_destroy(cls, device_number);
    if (!IS_ERR(cls))
        class_destroy(cls);
    if (cdev_ret == 0)
        cdev_del(&mydev_cdev);
    if (alloc_ret == 0)
        unregister_chrdev_region(device_number, 1);
    return -1;
}

static void __exit mydev_exit(void)
{
    printk(KERN_INFO "mydev: exit()\n");
    device_destroy(cls, device_number);
    class_destroy(cls);
    cdev_del(&mydev_cdev);
    unregister_chrdev_region(device_number, 1);
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUANG, SHIEN-TING <sthuang.ee11@nycu.edu.tw>");
MODULE_DESCRIPTION("A Linux driver for lab 4");
MODULE_VERSION("0.1");
