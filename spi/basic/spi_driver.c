#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/spi.h>


#define MAX_SPEED = 20000000
#define BITS_PER_WORD = 16
#define DEVICE_NAME = "simple_spi_driver"
#define MAX_LEN = 256


struct device_data {
  struct mutex lock;
  struct spi_device *spi;
  char name;
  struct cdev spi_cdev;
  char *buffer;
}

static struct class *spi_class = NULL;
dev_t devno;

static int spi_open(struct inode *inode, struct file *file) {
  
  struct device_data *dev = NULL;
	dev = container_of(inode->i_cdev, struct device_data, cdev);
	
	if (dev == NULL){
	    pr_err("Container_of did not found any valid data\n");
		return -ENODEV; /* No such device */
	}

    filp->private_data = dev;

    if (inode->i_cdev != &dev->cdev) {
        pr_err("Device open: internal error\n");
        return -ENODEV; /* No such device */
    }
	
    dev->buffer = (unsigned char*)kzalloc(MAX_BUFFER_SIZE, GFP_KERNEL);
    if (dev->data == NULL){
        pr_err("Error allocating memory\n");
        return -ENOMEM;
	}

	return 0;
}

static int spi_release(struct inode *inode, struct file *file)
{
	
	struct device_data *dev = filp->private_data;
	
   	    if (dev->buffer != NULL) {
     		    kfree(dev->buffer);
  		    dev->buffer = NULL ;
   		 }
	
        return 0;
}


static ssize_t spi_write(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
  int retval;
  struct device_data *dev = filp->private_data;
  
  if(len > MAX_LEN) 
      len = MAX_LEN;
  
  if (mutex_lock_killable(&dev->lock))    /* locking mutex to avoid other devices from accessing buffer and resources */
      return -EINTR;
  
  retval = spi_write(dev->spi, buf, len); /* helper function built around spi_sync() */
  if(retval <= 0) {
    pr_err("Unable to write);
    goto end_read;
           }

 dev->buffer = NULL;
           
 end_read:
    mutex_unlock(&dev->lock);
    return retval;
}
   
static ssize_t spi_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
  int retval;
  struct device_data *dev = filp->private_data;
  
  if(len > MAX_LEN) 
      len = MAX_LEN;
  
  if (mutex_lock_killable(&dev->lock))    /* locking mutex to avoid other devices from accessing buffer and resources */
      return -EINTR;
  
  retval = spi_read(dev->spi, dev->buffer, len); /* helper function built around spi_sync() */
  if(retval < 0) {
    pr_err("Unable to read);
    goto end_read;
           }
  
 if(copy_to_user(buf, dev->buffer, count) != 0){
        retval = -EIO;
        goto end_read;
 }
 
 dev->buffer = NULL;
           
 end_read:
    mutex_unlock(&dev->lock);
    return retval;
}
           
           
struct file_operations eep_fops = {
	.owner =    THIS_MODULE,
	.read =     spi_read,
	.write =    spi_write,
	.open =     spi_open,
	.release =  spi_release,
};


static int spi_probe (struct spi_device *spi)
{
  int err;
  
  struct device_data *data;
  
  data = devm_kzalloc(&spi->dev, sizeof(struct device_data), GFP_KERNEL);
    if(!data) {
     return -ENOMEM;  
    }
  
  mutex_init(data->lock);  /* initialize mutex */
  
  data->spi = spi;
  data->name = DEVICE_NAME;
  
  spi->mode = SPI_MODE_0;
  spi->max_speed_hz = MAX_SPEED;
  spi->bits_per_word = 16;
  
  err = spi_setup(spi); /* re-initialising struct spi_device with new values */
      if(err < 0)
          return err;
  
  spi_set_drvdata(spi,data);
  	
  /* allot device number */
  err = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
    if (err < 0) {
        pr_err("alloc_chrdev_region() failed for %s\n", DEVICE_NAME);
        return err;
    }

    /* Create device class */
    spi_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(eep_class)) {    /* since class_create() returns a pointer, IS_ERR() is used determine if its a invalid pointed */
        err = PTR_ERR(eep_class);  /* PTR_ERR() will give the pointer error type */
        goto fail_class;
    }
 
    cdev_init(data->spi_cdev, &spi_fops);
    data->spi_cdev.owner = THIS_MODULE;
	
    err = cdev_add(&data->spi_cdev, devno, 1);
    if(err<0) {
	    printk(KERN_INFO "Cannot add the device to the system\n");
            goto fail_class;
    }
  
        /*Creating device*/
     if((device_create(spi_class,NULL,devno,NULL,"device_sensor")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto fail_device;
     }
		
  
return 0;	

fail_device:
        class_destroy(spi_class);

fail_class:
        cdev_del(&device_data->spi_cdev);
        unregister_chrdev_region(devno,1);
        return -1;
	
}

static int spi_remove (struct spi_device *spi) {
  
    struct device_data *device = spi_get_drvdata(spi);
  
    device_destroy(spi_class, devno);
    kfree(device->buffer);
    mutex_destroy(&device->lock);
    kfree(device);
    class_destroy(spi_class);
    unregister_chrdev_region(devno, 1);
  
	return 0;
}


static struct spi_device_id spi_id_table[] = {
  { "xdevice", 0},
  { },
};

MODULE_DEVICE_TABLE(spi, spi_id_table);

static struct of_device_id spi_of_ids[] = {
  { .compatible = "xdevice" },
  { /*sentinel */ },
}

MODULE_DEVICE_TABLE(of, spi_of_ids(;

struct spi_driver spidriver {
  .driver = {
    .owner = THIS_MODULE,
    .name = DEVICE_NAME,
    .of_match_table = of_match_ptr(spi_of_ids),
  },
  .probe = spi_probe,
  .remove = spi_remove,
  .id_table = spi_id_table,
}

                                   
module_spi_driver(spidriver);
                                   
MODULE_LICENSE("GPL");
                                 
