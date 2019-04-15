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
  unsigned char *buffer;    /*array pointer for buffer  */
  struct i2c_client *i2c;
}



static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  
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
