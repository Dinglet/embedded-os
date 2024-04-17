#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define FILE_MAX_SIZE 4
#define FILENAME "segments"

static struct proc_dir_entry *entry;

static int __init segments_init(void);
static void __exit segments_exit(void);
// static int segments_open(struct inode *inode, struct file *file);
// static int segments_release(struct inode *inode, struct file *file);
static ssize_t segments_read(struct file *file, char __user *buffer, size_t size, loff_t *offset);
static ssize_t segments_write(struct file *file, const char __user *buffer, size_t size, loff_t *offset);

#ifdef HAVE_PROC_OPS
static const struct proc_ops operations = {
    .proc_read = segments_read,
    .proc_write = segments_write,
};
#else
static const struct file_operations operations = {
    .read = segments_read,
    .write = segments_write,
};
#endif

static int __init segments_init(void)
{
    entry = proc_create(FILENAME, 0664, NULL, &operations);
    if (NULL == entry)
    {
        proc_remove(entry);
        pr_alert("Error:Could not initialize /proc/%s\n", FILENAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", FILENAME);
    return 0;
}

static void __exit segments_exit(void)
{
    proc_remove(entry);
    pr_info("/proc/%s removed\n", FILENAME);
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
