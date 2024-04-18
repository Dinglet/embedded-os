#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/version.h>

#define FILE_MAX_SIZE 4
#define DEVICE_NAME "segments"

static int __init segments_init(void);
static void __exit segments_exit(void);
static int segments_open(struct inode *inode, struct file *file);
static int segments_release(struct inode *inode, struct file *file);
static ssize_t segments_read(struct file *file, char __user *buffer, size_t size, loff_t *offset);
static ssize_t segments_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset);

static dev_t device_number = 0;
static struct cdev segments_cdev;
static struct class *cls = NULL;
static struct device *dev = NULL;

static const struct file_operations operations = {
    .owner = THIS_MODULE,
    .open = segments_open,
    .release = segments_release,
    .read = segments_read,
    .write = segments_write,
};

static int __init segments_init(void)
{
    int alloc_ret = -1;
    int cdev_ret = -1;
    printk(KERN_INFO "segments: init()\n");

    // register a char device number
    alloc_ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);
    if (alloc_ret)
    {
        printk(KERN_ERR "segments: failed to register a char device number\n");
        goto error;
    }

    // initialize a cdev structure
    cdev_init(&segments_cdev, &operations);

    // add a char device to the system
    cdev_ret = cdev_add(&segments_cdev, device_number, 1);
    if (cdev_ret)
    {
        printk(KERN_ERR "segments: failed to add a char device to the system\n");
        goto error;
    }

    printk(KERN_INFO "segments: registered with major number %d\n", MAJOR(device_number));

    // create a struct class structure
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(DEVICE_NAME);
#else
    cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif
    if (IS_ERR(cls))
    {
        printk(KERN_ERR "segments: failed to create a struct class\n");
        goto error;
    }

    // creates a device and registers it with sysfs
    dev = device_create(cls, NULL, device_number, NULL, DEVICE_NAME);
    if (IS_ERR(dev))
    {
        printk(KERN_ERR "segments: failed to create a device and register it with sysfs\n");
        goto error;
    }

    printk(KERN_INFO "segments: created on /dev/%s\n", DEVICE_NAME);

    return 0;

error:
    if (!IS_ERR(dev))
        device_destroy(cls, device_number);
    if (!IS_ERR(cls))
        class_destroy(cls);
    if (cdev_ret == 0)
        cdev_del(&segments_cdev);
    if (alloc_ret == 0)
        unregister_chrdev_region(device_number, 1);
    return -1;
}

static void __exit segments_exit(void)
{
    printk(KERN_INFO "segments: exit()\n");
    device_destroy(cls, device_number);
    class_destroy(cls);
    cdev_del(&segments_cdev);
    unregister_chrdev_region(device_number, 1);
}

int segments_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "segments: open()\n");
    return 0;
}

int segments_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "segments: release()\n");
    return 0;
}

static ssize_t segments_read(struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
    return -EINVAL;
}

static ssize_t segments_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
    return -EINVAL;
}

module_init(segments_init);
module_exit(segments_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUANG, SHIEN-TING <sthuang.ee11@nycu.edu.tw>");
MODULE_DESCRIPTION("A Linux driver for homework 1");
MODULE_VERSION("0.1");
