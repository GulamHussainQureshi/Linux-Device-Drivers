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

struct fake_sensor {
	unsigned address;
	int buffer_size;
	
	

struct device_data {
  struct i2c_client *i2c;
  struct fake_sensor *xdevice;
}

/*
struct i2c_device_data {
  	int irq;
  	struct gpio_chip;
	struct dma_chan *dma_rx, *dma_tx;
}
*/

unsigned char *buffer[MAX_BUFFER_SIZE]; /*array pointer for buffer  */
#define MAX_BUFFER_SIZE 128  /* buffer size */


static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  struct device_data *i2c_data;
  struct fake_sensor *sensor_data;
	
  int num;
 
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA))
	  return -EIO;
  
  i2c_data = devm_kzalloc(&client->dev, sizeof(*i2c_data), GFP_KERNEL)
  if(!i2c_data)
	  return -ENOMEM;
	
  sensor_data = devm_kzalloc(&client->dev, sizeof(fake_sensor), GFP_KERNEL)
  if(!sensor_data)
	  return -ENOMEM;
	
  const struct of_device_id *match;
  match = of_match_device(i2c_of_ids, &client->dev);

  if (match)
  {
	sensor_data = match->data;
  }
  else {
	sensor_data = dev_get_platdata(&client->dev);
  }
 	
  client->addr = sensor_data->address;
	
  i2c_data->i2c = client;
  i2c_data->fake_sensor = sensor_data;
	
  i2c_set_clientdata(client, i2c_data);  /* sets the void *driver_data field of the struct device substructure in the 
  						struct i2c_client structure */

  
}

static int i2c_remove(struct i2c_client *client)
{
  
}
  
static const struct of_device_id i2c_of_ids[] = {
  {.compatible = "microchip,devicename", .data = &device1},
  {},
}
  
static struct fake_device device1 = {
	.address = 0x00010A3F;
	.buffer_size = 128;
}

MODULE_DEVICE_TABLE(of, i2c_of_ids);
  
struct i2c_device_id i2c_ids[] {
  {"devicename", device1},
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
