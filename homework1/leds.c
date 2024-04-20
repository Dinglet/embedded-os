#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/gpio.h>

#define FILE_SIZE 4
#define CLASS_NAME "mygpio" // /sys/class/gpio/<CLASS_NAME>
#define DEVICE_NAME "leds" // /dev/<DEVICE_NAME>

static int __init leds_init(void);
static void __exit leds_exit(void);
static int leds_open(struct inode *inode, struct file *file);
static int leds_release(struct inode *inode, struct file *file);
static ssize_t leds_read(struct file *file, char __user *buffer, size_t size, loff_t *offset);
static ssize_t leds_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset);

static dev_t device_number = 0;
static struct cdev leds_cdev;
static struct class *cls = NULL;
static struct device *dev = NULL;

static const struct file_operations operations = {
    .owner = THIS_MODULE,
    .open = leds_open,
    .release = leds_release,
    .read = leds_read,
    .write = leds_write,
};

uint8_t leds[FILE_SIZE] = {0};

#define LED_0 (6)
#define LED_1 (13)
#define LED_2 (19)
#define LED_3 (26)
#define LED_4 (12)
#define LED_5 (16)
#define LED_6 (20)
#define LED_7 (21)
static struct gpio leds_gpios[] =
{
    {LED_0, GPIOF_OUT_INIT_LOW, "LED_0"},
    {LED_1, GPIOF_OUT_INIT_LOW, "LED_1"},
    {LED_2, GPIOF_OUT_INIT_LOW, "LED_2"},
    {LED_3, GPIOF_OUT_INIT_LOW, "LED_3"},
    {LED_4, GPIOF_OUT_INIT_LOW, "LED_4"},
    {LED_5, GPIOF_OUT_INIT_LOW, "LED_5"},
    {LED_6, GPIOF_OUT_INIT_LOW, "LED_6"},
    {LED_7, GPIOF_OUT_INIT_LOW, "LED_7"},
};
void set_leds(int32_t n)
{
    n = n < 0 ? 0 : n;
    n = n > 8 ? 8 : n;

    for (int i = 0; i < 8; i++)
    {
        gpio_set_value(leds_gpios[i].gpio, i < n);
    }
}

static int __init leds_init(void)
{
    int alloc_ret = -1;
    int cdev_ret = -1;
    int request_ret = -1;
    printk(KERN_INFO "leds: init()\n");

    // register a char device number
    alloc_ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);
    if (alloc_ret)
    {
        printk(KERN_ERR "leds: failed to register a char device number\n");
        goto err_alloc_chrdev_region;
    }

    // initialize a cdev structure
    cdev_init(&leds_cdev, &operations);

    // add a char device to the system
    cdev_ret = cdev_add(&leds_cdev, device_number, 1);
    if (cdev_ret)
    {
        printk(KERN_ERR "leds: failed to add a char device to the system\n");
        goto err_cdev_add;
    }

    printk(KERN_INFO "leds: registered with major number %d\n", MAJOR(device_number));

    // create a struct class structure
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(CLASS_NAME);
#else
    cls = class_create(THIS_MODULE, CLASS_NAME);
#endif
    if (IS_ERR(cls))
    {
        printk(KERN_ERR "leds: failed to create a struct class\n");
        goto err_class_create;
    }

    // creates a device and registers it with sysfs
    dev = device_create(cls, NULL, device_number, NULL, DEVICE_NAME);
    if (IS_ERR(dev))
    {
        printk(KERN_ERR "leds: failed to create a device and register it with sysfs\n");
        goto err_device_create;
    }

    printk(KERN_INFO "leds: created on /dev/%s\n", DEVICE_NAME);

    // request multiple GPIOs in a single call
    request_ret = gpio_request_array(leds_gpios, ARRAY_SIZE(leds_gpios));
    if (request_ret)
    {
        printk(KERN_ERR "leds: failed to request multiple GPIOs in a single call\n");
        goto err_gpio_request_array;
    }

    return 0;

// error:
    gpio_free_array(leds_gpios, ARRAY_SIZE(leds_gpios));
err_gpio_request_array:
    device_destroy(cls, device_number);
err_device_create:
    class_destroy(cls);
err_class_create:
    cdev_del(&leds_cdev);
err_cdev_add:
    unregister_chrdev_region(device_number, 1);
err_alloc_chrdev_region:

    return -1;
}

static void __exit leds_exit(void)
{
    printk(KERN_INFO "leds: exit()\n");
    gpio_free_array(leds_gpios, ARRAY_SIZE(leds_gpios));
    device_destroy(cls, device_number);
    class_destroy(cls);
    cdev_del(&leds_cdev);
    unregister_chrdev_region(device_number, 1);
}

int leds_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "leds: open()\n");
    return 0;
}

int leds_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "leds: release()\n");
    return 0;
}

static ssize_t leds_read(struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
    size_t i = *offset % FILE_SIZE;
    printk(KERN_INFO "leds: read()\n");

    size = size > (FILE_SIZE - i) ? (FILE_SIZE - i) : size;

    if (copy_to_user(buffer, leds + i, size))
    {
        printk(KERN_ERR "leds: failed to copy data to user space\n");
        return -EFAULT;
    }

    *offset += size;
    return size;
}

static ssize_t leds_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset)
{
    size_t i = *offset % FILE_SIZE;
    printk(KERN_INFO "leds: write()\n");

    size = size > (FILE_SIZE - i) ? (FILE_SIZE - i) : size;

    if (copy_from_user(leds + i, buffer, size))
    {
        printk(KERN_ERR "leds: failed to copy data from user space\n");
        return -EFAULT;
    }

    set_leds(*(int32_t *)leds);

    *offset += size;
    return size;
}

module_init(leds_init);
module_exit(leds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUANG, SHIEN-TING <sthuang.ee11@nycu.edu.tw>");
MODULE_DESCRIPTION("A Linux driver for homework 1");
MODULE_VERSION("0.1");
