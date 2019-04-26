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
#include <linux/i2c.h>
#include <linux/delay.h>
#include <asm/uaccess.h>


#define MAX_BUFFER_SIZE 128  /* buffer size */
#define DEVICE_NAME "fake_sensor"
#define READ_COMMAND_SLAVE 0x0034

struct device_data {
  struct i2c_client *i2c;
  /* struct sensor_data *xdevice; */
  struct mutex device_mutex;
  struct cdev device_sensor;
  unsigned char *data;      /*array pointer for buffer  */
}

/*
struct i2c_device_data {
  	int irq;
  	struct gpio_chip;
	struct dma_chan *dma_rx, *dma_tx;
}

struct sensor_data {
	unsigned address;
	int buffer_size;
	void *data;
}

*/

static struct class *device_class = NULL;
dev_t devno;

static int sensor_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Open Function Called...!!!\n");
	
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
	
    dev->data = (unsigned char*)kzalloc(MAX_BUFFER_SIZE, GFP_KERNEL);
    if (dev->data == NULL){
        pr_err("Error allocating memory\n");
        return -ENOMEM;
	}

	return 0;
}
 
static int sensor_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Release Function Called...!!!\n");
	
	struct device_data *dev = filp->private_data;
	
   	    if (dev->data != NULL) {
     		    kfree(dev->data);
  		    dev->data = NULL ;
   		 }
	
        return 0;
}
 
static ssize_t sensor_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Driver Read Function Called...!!!\n");
	
	int err;
	char *output;
	ssize_t size;
	int len_bytes;
	int last_bits;
		
	struct device_data *dev = NULL;
	struct i2c_client *client;
	
	dev = filp->private_data;
	client = dev->i2c;
	
	if (mutex_lock_killable(&dev->device_mutex))
        return -EINTR;

	err = i2c_smbus_read_block_data(client, READ_COMMAND_SLAVE, dev->data);
		if(err < MAX_BUFFER_SIZE) {
			printk(KERN_ERR "I2C read error. \n");
		}
	
	/*
	output = dev->data;
	size = strlen(output);
	len_bytes = size/8;
	last_bits = size%8;
	*/
	
	if(copy_to_user(buf, dev->data, strlen(dev->data)) != 0){
        retval = -EIO;

	mutex_unlock(&dev->device_mutex);
		
        return strlen(dev->data);
}

static ssize_t sensor_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Cannot write to the sensor...!!!\n");
	
        return 0;
}

static struct file_operations sensor_fops =
{
.owner          = THIS_MODULE,
.read           = sensor_read,
.write          = sensor_write,
.open           = sensor_open,
.release        = sensor_release,
};

static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  struct device_data *i2c_data;
  struct sensor_data *matched_data;
  
    int err = 0;

 
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA))
	  return -EIO;
  
  i2c_data = devm_kzalloc(&client->dev, sizeof(*i2c_data), GFP_KERNEL)
  if(!i2c_data)
	  return -ENOMEM;
	
  mutex_init(i2c_data->device_mutex);

/*
  matched_data = devm_kzalloc(&client->dev, sizeof(sensor_data), GFP_KERNEL)
  if(!sensor_data)
	  return -ENOMEM;
	
  const struct of_device_id *match;
  match = of_match_device(i2c_of_ids, &client->dev);

  if (match)
  {
	matched_data = match->data;
  }
  else {
	matched_data = dev_get_platdata(&client->dev);
  }
 	
  client->addr = matched_data->address;
	
  i2c_data->xdevice = matched_data;
*/
	
  i2c_data->i2c = client;

  i2c_set_clientdata(client, i2c_data);  /* sets the void *driver_data field of the struct device substructure in the 
  						struct i2c_client structure */

	
  err = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
    if (err < 0) {
        pr_err("alloc_chrdev_region() failed for %s\n", DEVICE_NAME);
        return err;
    }

    /* Create device class */
    device_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(eep_class)) {
        err = PTR_ERR(eep_class);
        goto fail_class;
    }

	    
    cdev_init(i2c_data->device_sensor, &sensor_fops);
    i2c_data->device_sensor.owner = THIS_MODULE;
	
    err = cdev_add(&i2c_data->device_sensor, devno, 1);
    if(err<0) {
	    printk(KERN_INFO "Cannot add the device to the system\n");
            goto fail_class;
    }
  
        /*Creating device*/
     if((device_create(device_class,NULL,devno,NULL,"device_sensor")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto fail_device;
     }
		

return 0;	

fail_device:
        class_destroy(device_class);

fail_class:
        unregister_chrdev_region(devno,1);
        return -1;
	
}

static int i2c_remove(struct i2c_client *client)
{
  
    struct device_data *device = i2c_get_clientdata(client);
    device_destroy(device_class, devno);

    kfree(device->data);
    mutex_destroy(&device->device_mutex);
    kfree(device);
    class_destroy(device_class);
    unregister_chrdev_region(devno, 1);
return 0;
	
}
  
static const struct of_device_id i2c_of_ids[] = {
  {.compatible = "microchip,devicename"},
  {},
}
  
MODULE_DEVICE_TABLE(of, i2c_of_ids);
  
struct i2c_device_id i2c_ids[] {
  {"devicename", 0},
  {},
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static struct i2c_driver sample_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "sample driver",
		.of_match_table = of_match_ptr(i2c_of_ids),
	},
	.probe = i2c_probe,
	.remove = i2c_remove,
	.id_table = i2c_ids,
};

module_i2c_driver(sample_i2c_driver);
