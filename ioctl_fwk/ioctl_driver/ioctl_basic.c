#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> // required for various structures related to files liked fops. 
#include <linux/semaphore.h>
#include <linux/cdev.h> 
#include "ioctl_basic.h"
#include <linux/version.h>
#include <linux/device.h>

static int Major;
//static struct class *cl;

#define CDEV_NAME "ioctl_dev"

int open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Inside open \n");
    return 0;
}

int release(struct inode *inode, struct file *filp)
{
    printk (KERN_INFO "Inside close \n");
    return 0;
}

//int ioctl_funcs(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
int ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    printk(KERN_INFO "%s: enter\n", __FUNCTION__);
    printk(KERN_INFO "%s: cmd 0x%08x arg 0x%08x\n", __FUNCTION__, cmd, arg);
    switch(cmd)
    {
        case IOCTL_HELLO: 
            printk(KERN_INFO "Hello ioctl world");
            break;
    } 
 
    printk(KERN_INFO "%s: exit %d\n", __FUNCTION__, ret);
    return ret;
}

struct file_operations fops = {
    open:    open,
    release: release,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
    unlocked_ioctl:  ioctl_funcs // compat_ioctl doesn't always work
#else
    ioctl:   ioctl_funcs
#endif // #if
};

struct cdev *kernel_cdev; 

int char_arr_init(void) 
{
    int ret;
    dev_t dev_no,dev;

    kernel_cdev = cdev_alloc(); 
    kernel_cdev->ops = &fops;
    kernel_cdev->owner = THIS_MODULE;
    printk (" Inside init module\n");
    ret = alloc_chrdev_region( &dev_no , 0, 1, CDEV_NAME);
    if (ret < 0)
    {
        printk("Major number allocation is failed\n");
        return ret; 
    }
 
//    cl = class_create(THIS_MODULE, CDEV_NAME);
//    ret = device_create(cl, NULL, dev_no, NULL, CDEV_NAME);

    Major = MAJOR(dev_no);
    dev = MKDEV(Major,0);
    printk (" The major number for your device is %d\n", Major);
    ret = cdev_add( kernel_cdev,dev,1);
    if(ret < 0 ) 
    {
        printk(KERN_INFO "Unable to allocate cdev");
        return ret;
    }

    return 0;
}

void char_arr_cleanup(void)
{
    printk(KERN_INFO " Inside cleanup_module\n");
    cdev_del(kernel_cdev);
//    device_destroy(cl, Major);
//    class_destroy(cl);
    unregister_chrdev_region(Major, 1);
}

MODULE_LICENSE("GPL"); 
module_init(char_arr_init);
module_exit(char_arr_cleanup);
