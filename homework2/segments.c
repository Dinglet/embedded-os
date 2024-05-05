#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/gpio.h>

#define FILE_SIZE 1
#define CLASS_NAME "gpio-segments" // /sys/class/gpio/<CLASS_NAME>
#define DEVICE_NAME "segments" // /dev/<DEVICE_NAME>

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

static char segments[FILE_SIZE] = {0};

static struct gpio segments_gpios[] =
{
    { 4, GPIOF_OUT_INIT_LOW, "SEGMENT_A"},
    { 3, GPIOF_OUT_INIT_LOW, "SEGMENT_B"},
    {14, GPIOF_OUT_INIT_LOW, "SEGMENT_C"},
    {15, GPIOF_OUT_INIT_LOW, "SEGMENT_D"},
    {18, GPIOF_OUT_INIT_LOW, "SEGMENT_E"},
    {17, GPIOF_OUT_INIT_LOW, "SEGMENT_F"},
    {27, GPIOF_OUT_INIT_LOW, "SEGMENT_G"},
};
// static struct gpio segments_gpios[] =
// {
//     {6, GPIOF_OUT_INIT_LOW, "SEGMENT_A"},
//     {13, GPIOF_OUT_INIT_LOW, "SEGMENT_B"},
//     {19, GPIOF_OUT_INIT_LOW, "SEGMENT_C"},
//     {26, GPIOF_OUT_INIT_LOW, "SEGMENT_D"},
//     {12, GPIOF_OUT_INIT_LOW, "SEGMENT_E"},
//     {16, GPIOF_OUT_INIT_LOW, "SEGMENT_F"},
//     {20, GPIOF_OUT_INIT_LOW, "SEGMENT_G"},
// };

/*
Character : Binary
-------------------
    0     : 0111111
    1     : 0000110
    2     : 1011011
    3     : 1001111
    4     : 1100110
    5     : 1101101
    6     : 1111101
    7     : 0000111
    8     : 1111111
    9     : 1101111
    A     : 1110111
    b     : 1111100
    C     : 0111001
    d     : 1011110
    E     : 1111001
    F     : 1110001
*/
static const uint8_t segments_table[] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111, // 9
    0b1110111, // A
    0b1111100, // b
    0b0111001, // C
    0b1011110, // d
    0b1111001, // E
    0b1110001, // F
    0b0000000, // blank
};
#define MAX_SEGMENTS (ARRAY_SIZE(segments_table) - 1)

static void set_segments(char c)
{
    int n = MAX_SEGMENTS;
    if (c >= '0' && c <= '9')
    {
        n = c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        n = c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        n = c - 'a' + 10;
    }

    n = segments_table[n];

    for (int i = 0; i < ARRAY_SIZE(segments_gpios); i++)
    {
        gpio_set_value(segments_gpios[i].gpio, (n >> i) & 1);
    }
}

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
        goto err_alloc_chrdev_region;
    }

    // initialize a cdev structure
    cdev_init(&segments_cdev, &operations);

    // add a char device to the system
    cdev_ret = cdev_add(&segments_cdev, device_number, 1);
    if (cdev_ret)
    {
        printk(KERN_ERR "segments: failed to add a char device to the system\n");
        goto err_cdev_add;
    }

    printk(KERN_INFO "segments: registered with major number %d\n", MAJOR(device_number));

    // create a struct class structure
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(CLASS_NAME);
#else
    cls = class_create(THIS_MODULE, CLASS_NAME);
#endif
    if (IS_ERR(cls))
    {
        printk(KERN_ERR "segments: failed to create a struct class\n");
        goto err_class_create;
    }

    // creates a device and registers it with sysfs
    dev = device_create(cls, NULL, device_number, NULL, DEVICE_NAME);
    if (IS_ERR(dev))
    {
        printk(KERN_ERR "segments: failed to create a device and register it with sysfs\n");
        goto err_device_create;
    }

    printk(KERN_INFO "segments: created on /dev/%s\n", DEVICE_NAME);

    // request multiple GPIOs in a single call
    if (gpio_request_array(segments_gpios, ARRAY_SIZE(segments_gpios)) < 0)
    {
        printk(KERN_ERR "segments: failed to request multiple GPIOs in a single call\n");
        goto err_gpio_request_array;
    }

    return 0;

// error:
    gpio_free_array(segments_gpios, ARRAY_SIZE(segments_gpios));
err_gpio_request_array:
    device_destroy(cls, device_number);
err_device_create:
    class_destroy(cls);
err_class_create:
    cdev_del(&segments_cdev);
err_cdev_add:
    unregister_chrdev_region(device_number, 1);
err_alloc_chrdev_region:

    return -1;
}

static void __exit segments_exit(void)
{
    printk(KERN_INFO "segments: exit()\n");
    gpio_free_array(segments_gpios, ARRAY_SIZE(segments_gpios));
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
    size_t i = *offset % FILE_SIZE;
    printk(KERN_INFO "segments: read()\n");

    if (*offset >= FILE_SIZE)
    {
        return 0;
    }

    size = size > (FILE_SIZE - i) ? (FILE_SIZE - i) : size;

    if (copy_to_user(buffer, segments + i, size))
    {
        printk(KERN_ERR "segments: failed to copy data to user space\n");
        return -EFAULT;
    }

    *offset += size;
    return size;
}

static ssize_t segments_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
    size_t i = *offset % FILE_SIZE;
    printk(KERN_INFO "segments: write()\n");

    size = size > (FILE_SIZE - i) ? (FILE_SIZE - i) : size;

    if (copy_from_user(segments + i, buffer, size))
    {
        printk(KERN_ERR "segments: failed to copy data from user space\n");
        return -EFAULT;
    }

    set_segments(*segments);

    *offset += size;
    return size;
}

module_init(segments_init);
module_exit(segments_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUANG, SHIEN-TING <sthuang.ee11@nycu.edu.tw>");
MODULE_DESCRIPTION("A Linux driver for homework 1");
MODULE_VERSION("0.1");
