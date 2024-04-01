/*
In this lab, you are required to write a Linux device driver that can control a 16-segment display to display the characters from A to Z.
*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include <linux/ctype.h>

// define the file operations
static int mydev_open(struct inode *inode, struct file *file);
static int mydev_release(struct inode *inode, struct file *file);
static ssize_t mydev_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t mydev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset);

static int major = 0;

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

// default display is off
static uint16_t segments = alphabet_segments[ALPHABET_SIZE];

uint16_t get_segment_display(void)
{
    return segments;
}

void set_segment_display(char c)
{
    if (isupper(c))
        segments = alphabet_segments[c - 'A'];
    else if (islower(c))
        segments = alphabet_segments[c - 'a'];
    else
        segments = alphabet_segments[ALPHABET_SIZE];
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
    static char str_segments[16] = {0};
    uint16_t segments = alphabet_segments[ALPHABET_SIZE];
    int i;
    
    printk(KERN_INFO "mydev: read()\n");

    len = len > 16 ? 16 : len;

    segments = get_segment_display();

    for (i = 0; i < 16; ++i)
    {
        str_segments[i] = (segments >> i) & 1 ? '1' : '0';
    }

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

    set_segment_display(c);

    return len;
}

#define DEVICE_NAME "mydev"

static int __init mydev_init(void)
{
    printk(KERN_INFO "mydev: init()\n");
    
    // create and register a cdev occupying a range of minor numbers
    major = register_chrdev(0, DEVICE_NAME, &mydev_fops);
    if (major < 0)
    {
        printk(KERN_ERR "mydev: failed to register device\n");
        return -1;
    }

    printk(KERN_INFO "mydev: registered device with major number %d\n", major);
    printk(KERN_INFO "mydev: create a device file with `mknod /dev/%s c %d 0`\n", DEVICE_NAME, major);
    printk(KERN_INFO "Remove the device file and module `rm /dev%s && rmmod %s`when done.\n", DEVICE_NAME, DEVICE_NAME);
    
    return 0;
}

static void __exit mydev_exit(void)
{
    printk(KERN_INFO "mydev: exit()\n");
    unregister_chrdev(major, DEVICE_NAME);
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUANG, SHIEN-TING <sthuang.ee11@nycu.edu.tw>");
MODULE_DESCRIPTION("A Linux driver for lab 4");
MODULE_VERSION("0.1");
