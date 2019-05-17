#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h> /* for kmalloc() */
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

struct custom_data {
  int irq_num;
  struct i2c_client *i2c;
  char *buffer;
  struct mutex my_mutex;
  spinlock_t my_spinlock;
}

#define DRV_NAME "my_driver"

static irqreturn_t irq_handler(int irq, void *dev_id) 
{
  pr_info("module handler: %s, irq number is %d\n", __FUNCTION__, dev_id->irq_num);
  
 return IRQ_WAKE_THREAD;
}

static irqreturn_t threaded_irq_handler(int irq, void *dev_id)
{
  pr_info(" module handler: %s, irq number is %d\n", __FUNCTION__, dev_id->irq_num);
    
  return IRQ_HANDLED;
}

static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int err;
  struct custom_data *data;
  
  data = devm_kmalloc(client->dev, sizeof(struct custom_data), GFP_KERNEL);
  data->i2c = client;
  
  mutex_init(data->my_mutex);
  
  spinlock_int(data->my_spinlock);
  
  /* get irq number from struct i2c_client */
  data->irq_num = client->irq;
  
  /* request threaded irq */
  err = request_threaded_irq(data->irq_num, irq_handler, threaded_irq_handler, IRQF_TRIGGER_LOW | IRQF_ONESHOT, DRV_NAME, data);
  
  if(err) {
    dev_err(&client->dev, "IRQ %d is not free.\n", client->irq);
    pr_info("IRQ %d is not avaialbe, exiting module\n", client->irq_num);
    
    return 0;
  }
  
  
  return 0;
  
}

static int i2c_remove(struct i2c_client *client)
{
  free_irq(client->irq);
 
  return 0;
}

static const struct i2c_device_id irq_driver_ids[] = {
	{"simple_irq", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, irq_driver_ids);

static const struct of_device_id of_irq_ids[] = {
	{ .compatible = "simple_device", },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, of_irq_ids);

static struct i2c_driver i2c_irq_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "my_driver",
		.of_match_table = of_match_ptr(of_irq_ids),
	},
	.probe = i2c_probe,
	.remove = i2c_remove,
	.id_table = irq_driver_ids,
};

module_i2c_driver(i2c_irq_driver);

MODULE_LICENSE("GPL");
