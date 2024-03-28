/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple GPIO driver explanation
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/err.h>

#include <linux/ctype.h>

//LEDs are connected to these GPIOs
#define LED_0 (6)
#define LED_1 (13)
#define LED_2 (19)
#define LED_3 (26)
static struct gpio leds_gpios[] =
{
  {LED_0, GPIOF_OUT_INIT_LOW, "LED_0"},
  {LED_1, GPIOF_OUT_INIT_LOW, "LED_1"},
  {LED_2, GPIOF_OUT_INIT_LOW, "LED_2"},
  {LED_3, GPIOF_OUT_INIT_LOW, "LED_3"}
};
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
 
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
/******************************************************/
 
//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = etx_read,
  .write          = etx_write,
  .open           = etx_open,
  .release        = etx_release,
};

/*
** This function will be called when we open the Device file
*/ 
static int etx_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}

/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}

/*
** This function will be called when we read the Device file
*/ 
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
  char msg[2] = "0\0";
  uint8_t gpio_state = 0;
  uint8_t n = 0;
  
  // https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html#read-and-write
  len = min(2 - *off, len);
  if (len <= 0)
    return 0;

  // The device has only 2 byte.
  if (*off > 1)
    return 0;
  else if (*off == 1)
  {
    if (put_user('\0', buf))
      return -EFAULT;
    return 1;
  }
  
  //reading GPIO values
  gpio_state = gpio_get_value(LED_0);
  pr_info("Read function : LED_0 = %d \n", gpio_state);
  n |= gpio_state;

  gpio_state = gpio_get_value(LED_1);
  pr_info("Read function : LED_1 = %d \n", gpio_state);
  n |= gpio_state << 1;
  
  gpio_state = gpio_get_value(LED_2);
  pr_info("Read function : LED_2 = %d \n", gpio_state);
  n |= gpio_state << 2;

  gpio_state = gpio_get_value(LED_3);
  pr_info("Read function : LED_3 = %d \n", gpio_state);
  n |= gpio_state << 3;
  
  //write to user
  msg[0] = n + '0'; // convert to ASCII
  if ( copy_to_user(buf, msg, len) ) {
    return -EFAULT;
  }
  
  *off += len;
  return len;
}

/*
** This function will be called when we write the Device file
*/ 
static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
  uint8_t c = 0;
  int n = 0;

  printk(KERN_INFO "etx_write(%p, %d, %lu, %lld)", filp, (int)buf[0], len, *off);

  // copy_from_user returns number of bytes that could not be copied. On success, this will be zero.
  if (copy_from_user(&c, buf, 1) > 0)
  {
    pr_err("ERROR: Not all the bytes have been copied from user\n");
    return 0;
  }

  n = isdigit(c) ? c - '0' : 0;
  
  // write to GPIOs
  gpio_set_value(LED_0, n&1 ? 1 : 0);
  pr_info("Write Function : LED_0 Set = %d\n", n&1 ? 1 : 0);
  gpio_set_value(LED_1, n&2 ? 1 : 0);
  pr_info("Write Function : LED_1 Set = %d\n", n&2 ? 1 : 0);
  gpio_set_value(LED_2, n&4 ? 1 : 0);
  pr_info("Write Function : LED_2 Set = %d\n", n&4 ? 1 : 0);
  gpio_set_value(LED_3, n&8 ? 1 : 0);
  pr_info("Write Function : LED_3 Set = %d\n", n&8 ? 1 : 0);
  
  *off += len;
  return len;
}

/*
** Module Init function
*/ 
static int __init etx_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
    pr_err("Cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
  /*Creating cdev structure*/
  cdev_init(&etx_cdev,&fops);
 
  /*Adding character device to the system*/
  if((cdev_add(&etx_cdev,dev,1)) < 0){
    pr_err("Cannot add the device to the system\n");
    goto r_del;
  }
 
  /*Creating struct class*/
  if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
 
  /*Creating device*/
  if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
    pr_err( "Cannot create the Device \n");
    goto r_device;
  }
  
  //Checking the GPIO is valid or not
  if (gpio_is_valid(LED_0) == false)
  {
    pr_err("GPIO %d is not valid\n", LED_0);
    goto r_device;
  }
  if (gpio_is_valid(LED_1) == false)
  {
    pr_err("GPIO %d is not valid\n", LED_1);
    goto r_device;
  }
  if (gpio_is_valid(LED_2) == false)
  {
    pr_err("GPIO %d is not valid\n", LED_2);
    goto r_device;
  }
  if (gpio_is_valid(LED_3) == false)
  {
    pr_err("GPIO %d is not valid\n", LED_3);
    goto r_device;
  }
  
  // request multiple GPIOs in a single call
  if (gpio_request_array(leds_gpios, ARRAY_SIZE(leds_gpios)) < 0)
  {
    pr_err("Unable to request GPIOs\n");
    goto r_gpio;
  }
  
  /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
  ** Now you can change the gpio values by using below commands also.
  ** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
  ** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
  ** cat /sys/class/gpio/gpio21/value  (read the value LED)
  ** 
  ** the second argument prevents the direction from being changed.
  */
  // gpio_export(GPIO_21, false);
  
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;
 
r_gpio:
  gpio_free_array(leds_gpios, ARRAY_SIZE(leds_gpios));
r_device:
  device_destroy(dev_class,dev);
r_class:
  class_destroy(dev_class);
r_del:
  cdev_del(&etx_cdev);
r_unreg:
  unregister_chrdev_region(dev,1);
  
  return -1;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
  // gpio_unexport(GPIO_21);
  gpio_free_array(leds_gpios, ARRAY_SIZE(leds_gpios));
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&etx_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");
