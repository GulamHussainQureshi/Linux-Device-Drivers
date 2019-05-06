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
#include <linux/regmap.h>

#define REG_INIT 0x3F
#define REG_POWER 0x61
#define REG_MODE 0x20

static struct custom_drv_data 
{
  struct regmap *map;
  struct mutex lock;
}

/* if writeable_reg and readable_reg are set, 
 * there is no need to provide wr_table and rd_table  

static const struct regmap_range wr_rd_range[] =
{
  {
    .range_min = 0x20,
    .range_max = 0x4F,
  },
  {
    .range_min = 0x60,
    .range_max = 0x7f,
  },
};

struct regmap_access_table drv_wr_table =
{
  .yes_ranges = wr_rd_range,
  .n_yes_ranges = ARRAY_SIZE(wr_rd_range),
};


struct regmap_access_table drv_rd_table =
{
  .yes_ranges = wr_rd_range,
  .n_yes_ranges = ARRAY_SIZE(wr_rd_range),
};

*/

static bool writeable_reg(struct device *dev, unsigned int reg)
{
  if (reg >= 0x20 && reg <= 0x4F)
    return true;
  if (reg >= 0x60 && reg <= 0x7F)
    return true;
};

static bool readable_reg(struct device *dev, unsinged int reg)
{
  if (reg >= 0x20 && reg <= 0x4F)
    return true;
  if (reg >= 0x60 && reg <= 0x7F)
    return true;
};

static const struct reg_sequence dummy_defaults_regs[] = {
  { REG_INIT, 0x8B },
  { REG_POWER, 0x01 },
  { REG_MODE, 0x02 },
};

static int spi_regmap_probe(struct spi_device *spi)
{
  struct regmap_config config;
  struct custom_drv_data *priv;
  unsigned char data;
  
  /* setup the regmap configuration */
  memset(&config, 0, sizeof(config)); /* zeroing the memory area of struct regmap_config */
  config.reg_bits = 8;
  config.val_bits = 8;
  config.write_flag_mask = 0x80;
  config.max_register = 0x80;
  config.fast_io = true;
  config.writeable_reg = spi_writeable_reg;
  config.readable_reg = spi_readable_reg;
 
  /* can use read & write tables instead of writeable_reg() & readable_reg() */
  //config.wr_table = drv_wr_table;
  //config.rd_table = drv_rd_table;
  
  priv = kzalloc(sizeof(struct custom_drv_data), GFP_KERNEL);
  if(!priv) {
        pr_err("No memory.");
        return -ENOMEM;
  }
  
  mutex_init(priv->lock);

  priv->map = regmap_init_spi(spi, &config);
  spi_set_drvdata(priv);
    
  mutex_lock(priv->lock);
  
  err = regmap_multi_reg_write(priv->map, dummy_defaults_regs, ARRAY_SIZE(dummy_defaults_regs));
  
  if(err <= 0)
  {
    mutex_unlock(priv->lock)
    return -1;
  }
  
  kprintf("Register values changed!!! \n");
  mutex_unlock(priv->lock);
 
  return 0;
}
  
static int spi_regmap_remove(struct spi_device *spi)
{
   struct custom_drv_data *device = spi_get_drvdata(spi);
   mutex_destroy(device->lock);
   kfree(device);
  
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

MODULE_DEVICE_TABLE(of, spi_of_ids);

struct spi_driver spidriver {
  .driver = {
    .owner = THIS_MODULE,
    .name = DEVICE_NAME,
    .of_match_table = of_match_ptr(spi_of_ids),
  },
  .probe = spi_regmap_probe,
  .remove = spi_regmap_remove,
  .id_table = spi_id_table,
}
