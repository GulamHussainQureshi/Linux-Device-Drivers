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

struct device_data {
  struct i2c_client *i2c;
  
}

/*
struct i2c_device_data {
  	int irq;
  	struct gpio_chip;
	struct dma_chan *dma_rx, *dma_tx;
}
*/

unsigned char *buffer; /*array pointer for buffer  */
#define MAX_BUFFER_SIZE 16*1024  /* 64Kb buffer size */


static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  struct device_data *i2c_data;
	
  int num;
  dev_t dev_no;
 
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA))
	  return -EIO;
	
/*	
  struct i2c_device_data *my_data;

  const struct of_device_id *match;
  match = of_match_device(i2c_of_ids, &client->dev);

  if (match)
  {
	my_data = match->data;
  }
  else {
	my_data = dev_get_platdata(&client->dev);
  }
  */

  i2c_data = devm_kzalloc(&client->dev, sizeof(*i2c_data), GFP_KERNEL)
  if(!i2c_data)
	  return -ENOMEM;
 	
  i2c_data->i2c = client;
 
  buffer = kzalloc
	
  i2c_set_clientdata(client, i2c_data);  /* sets the void *driver_data field of the struct device substructure in the 
  						struct i2c_client structure */
}

static int i2c_remove(struct i2c_client *client)
{
  
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
