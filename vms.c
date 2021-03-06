#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>

struct input_dev *vms_input_dev;
static struct platform_device *vms_dev;

static ssize_t write_vms (struct device *dev,
				struct device_attribute *attr,
				const char *buffer, size_t count) {
	int x, y;
	sscanf (buffer, "%d%d", &x, &y);
	input_report_rel (vms_input_dev, REL_X, x);
	input_report_rel (vms_input_dev, REL_Y, y);
	input_sync (vms_input_dev);

	return count;
}
DEVICE_ATTR (coordinates, 0664, NULL, write_vms);

static struct attribute *vms_attrs[] = {
	&dev_attr_coordinates.attr,
	NULL
};
static struct attribute_group vms_attr_group = {
	.attrs = vms_attrs,
};

int __init vms_init (void) {

	vms_dev = platform_device_register_simple ("vms", -1, NULL, 0);
	if (IS_ERR (vms_dev)) {
		printk ("vms_init: error \n");
		return PTR_ERR (vms_dev);
	}
	
	sysfs_create_group (&vms_dev->dev.kobj, &vms_attr_group);
	vms_input_dev = input_allocate_device ();
	if (!vms_input_dev) {
		printk ("Bad input_allocate_device \n");
		return -ENOMEM;
	}

	set_bit (EV_REL, vms_input_dev->evbit);
	set_bit (REL_X, vms_input_dev->relbit);
	set_bit (REL_Y, vms_input_dev->relbit);

	input_register_device (vms_input_dev);
	printk ("Virtual Mouse Driver Initialized. \n");

	return 0;
}

void vms_cleanup (void) {

	input_unregister_device (vms_input_dev);
	sysfs_remove_group (&vms_dev->dev.kobj, &vms_attr_group);
	platform_device_unregister (vms_dev);

	return ;
}

module_init (vms_init);
module_exit (vms_cleanup);
MODULE_LICENSE ("GPL");
