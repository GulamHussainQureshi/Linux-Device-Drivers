#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h> /* for kmalloc() */

struct custom_data {
  int irq_num;
  
}

#define DRV_NAME "my_driver"

static irqreturn_t threaded_irq_handler(int irq, void *dev_id)
{
  struct custom_data *my_data = dev_id;
   pr_info("Work queue module handler: %s, irq number is %d\n", __FUNCTION__, my_data->irq_num);

  return IRQ_HANDLED;
}

static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int err;
  struct custom_data *data;
  
  data = devm_kmalloc(client->dev, sizeof(struct custom_data), GFP_KERNEL);
  
  data->irq_num = client->irq;
  
