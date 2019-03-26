#include <linux/module.h> 	
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>	 	/* for file operations struct*/
#include <linux/cdev.h>
#include <linux/device.h>
#include "my_device.h"		/* functions write_to_device(), read_from_device() are defined in this header*/

#define MAXUSERS 5	
#define BUFFERSIZE 300

struct device_data {
	struct cdev cdev;
	char *buffer[MAXUSERS][BUFFERSIZE];
	int users;
};

static int __init driver_init(void);
static void __exit driver_exit(void);

static int driver_open(struct inode *inode, struct file *file);
static int driver_release(struct inode *inode, struct file *file);
static ssize_t driver_read(struct file *filp, char __user *buf, size_t len, loff_t  *off);
static ssize_t driver_write(struct file *filp, char __user *buf, size_t len, loff_t  *off);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev char_cdev;

DEFINE_MUTEX(device_list_lock);

unsigned long *kbuffer = 0;

static int driver_open(struct inode *inode, struct file *file)
{
        unsigned int maj = imajor(inode);
	unsigned int min = iminor(inode);

	struct device_data *device_1 = NULL;
	device_1 = container_of(inode->i_cdev, struct device_data, cdev);
	if(device_1 == 0)
		return -ENODEV;
	
	if (maj < 0 || min < 0) {
		pr_err("Open fn: device not found\n");
		return -ENODEV;
	}

	/*count the number of users that are opening the device*/
	if (device_1->users < 5) {
		kbuffer = device_1->buffer[device_1->users][BUFFERSIZE];
		device_1->users++;
		}
	
	mutex_lock(&device_list_lock);
	file->private data = device_1;	
	mutex_unlock(&device_list_lock);	

        return 0;
}

static int driver_release(struct inode *inode, struct file *file)
{
        struct device_data *device_release = NULL;
	device_release = container_of(inode->i_cdev, struct device_data, cdev);
	
	device_release->users = device_release->users - 1;

	if(device_release->users < 0) {
		mutex_lock(&device_list_lock);
		file->private_data = NULL;
		mutex_unlock(&device_list_lock);
	}
}
 
static ssize_t driver_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	int register_address = 0x000A142F;

        struct device_data *device_read = filp->private_data;
	if(!device_read)
		return -ENODEV;

	if(*off >= BUFFERSIZE) {
		pr_err("Write fn: Writing beyond the buffer size");
		return -EINVAL;
	}
	
	if(*off + len >= BUFFERSIZE)
		len = BUFFERSIZE - *off;

	if (copy_to_user(buf, device_write->buffer[device_write->users][BUFFFERSIZE], len)
		return -EIO;

	if (read_from_device(register_address, device_write->buffer[device_write->users][BUFFERSIZE], len) < 0) {
		pr_err("Write_to_device fn: write error\n");
		return -EFAULT;
	}

	*off += count;
	return count;
}
static ssize_t driver_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t retval = 0;
	int register_address = 0x000A142F;

        struct device_data *device_write = filp->private_data;
	if(!device_write)
		return -ENODEV;

	if(*off >= BUFFERSIZE) {
		pr_err("Write fn: Writing beyond the buffer size");
		return -EINVAL;
	}
	
	if(*off + len >= BUFFERSIZE)
		len = BUFFERSIZE - *off;

	if (copy_from_user(device_write->buffer[device_write->users][BUFFFERSIZE], buf, len)
		return -EIO;

	if (write_to_device(register_address, device_write->buffer[device_write->users][BUFFERSIZE], len) < 0) {
		pr_err("Write_to_device fn: write error\n");
		return -EFAULT;
	}

	*off += count;
	return count;
}
 
static struct file operations fops =
{
.owner = THIS_MODULE,
.read = driver_read,
.write = driver_write,
.open = driver_open,
.release = driver_release,
};
 
 
static int __init driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "char_dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&char_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&char_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"char_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");

	static struct device_data *device = NULL;
	device = kzalloc(sizeof(struct device_data), GFP_KERNEL);
	if(device == NULL) {
		pr_err("Device data memory allocation failed\n");
		return -ENOMEM;
	}

	device->cdev = char_cdev;
	device->buffer_size = sizeof(unsigned int) * 20;
	
    return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 
void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&char_cdev);
        unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(driver_init);
module_exit(driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gulam Hussain Qureshi");
MODULE_DESCRIPTION("character device driver");
