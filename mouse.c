#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>   
#include <linux/aio.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/input.h>
 
int vmouse_major = 200;
module_param(vmouse_major, int, 0);
MODULE_AUTHOR("Li XianJing <xianjimli@hotmail.com>");
MODULE_LICENSE("GPL");
 
static struct input_dev* vmouse_idev = NULL;
 
int vmouse_open(struct inode* inode, struct file* filp)
{
    return 0;
}

int vmouse_release(struct inode* inode, struct file* filp)
{
    return 0;
}
 
ssize_t vmouse_read(struct file* filp, char __user *buf, size_t count, loff_t* f_pos)
{
    printk(KERN_INFO"%s/n", __func__);
    return count;
}
 
struct mouse_event
{
    int press;
    int x;
    int y;
};

ssize_t vmouse_write(struct file* filp, const char __user * buf, size_t count, loff_t* f_pos)
{
    int ret = 0;
    struct mouse_event event;
 
    while(ret < count)
    {
        if(copy_from_user(&event, buf + ret, sizeof(event)))
        {
            return -EFAULT;
        }
        ret += sizeof(event);
 
        if(event.press == 1)
        {
            input_report_key(vmouse_idev, BTN_TOUCH, 1);
            input_report_abs(vmouse_idev, ABS_X, event.x & 0xfff);
            input_report_abs(vmouse_idev, ABS_Y, event.y & 0xfff);
            input_report_abs(vmouse_idev, ABS_PRESSURE, 0xff);
            input_sync (vmouse_idev);
        }
        else if(event.press == -1)
        {
            input_report_abs(vmouse_idev, ABS_X, event.x & 0xfff);
            input_report_abs(vmouse_idev, ABS_Y, event.y & 0xfff);
            input_sync (vmouse_idev);
        }
        else
        {
            input_report_abs(vmouse_idev, ABS_PRESSURE, 0);
            input_report_key(vmouse_idev, BTN_TOUCH, 0);
            input_sync (vmouse_idev);
        }
        printk(KERN_INFO"%s p=%d x=%d y=%d/n", __func__, event.press, event.x, event.y);
    }
 
    return ret;
}

static struct file_operations vmouse_fops =
{
    .owner = THIS_MODULE,
    .open    = vmouse_open,
    .release = vmouse_release,
    .read    = vmouse_read,
    .write   = vmouse_write,
};

static struct miscdevice vmouse_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "vmouse",
    .fops = &vmouse_fops
};

static int vmouse_input_dev_open(struct input_dev* idev)
{
    printk(KERN_INFO"%s/n", __func__);
 
    return 0;
}
 
static void vmouse_input_dev_close(struct input_dev* idev)
{
    printk(KERN_INFO"%s/n", __func__);
 
    return;
}
 
static int vmouse_input_dev_setup(void)
{
    int ret = 0;
    vmouse_idev = input_allocate_device();
 
    if(vmouse_idev == NULL)
    {
        return -ENOMEM;
    }
 
    vmouse_idev->name = "vmouse";
    vmouse_idev->phys = "vmouse/input0";
    vmouse_idev->open = vmouse_input_dev_open;
    vmouse_idev->close = vmouse_input_dev_close;
 
    __set_bit(EV_ABS, vmouse_idev->evbit);
    __set_bit(EV_KEY, vmouse_idev->evbit);
    __set_bit(ABS_X, vmouse_idev->absbit);
    __set_bit(ABS_Y, vmouse_idev->absbit);
    __set_bit(ABS_PRESSURE, vmouse_idev->absbit);
    __set_bit(BTN_TOUCH, vmouse_idev->keybit);
 
    ret = input_register_device(vmouse_idev);
 
    return ret;
}

static int __init vmouse_init(void)
{
//    int result = register_chrdev(vmouse_major, "vmouse", &vmouse_fops);
    printk ("mouse_event = %d \n", sizeof (struct mouse_event));
    misc_register(&vmouse_misc);
    vmouse_input_dev_setup();
 
    return 0;
}
 
static void __exit vmouse_cleanup(void)
{
    input_unregister_device(vmouse_idev);
    unregister_chrdev(vmouse_major, "vmouse");
 
    return;
}
 
module_init(vmouse_init);
module_exit(vmouse_cleanup);
